#!/bin/bash

die () {
  echo $1
  exit 1
}

SOURCE="${BASH_SOURCE[0]}"

# resolve $SOURCE until the file is no longer a symlink
while [ -h "$SOURCE" ]; do 
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  # if $SOURCE was a relative symlink, we need to resolve it relative to the path where
  # the symlink file was located
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done

DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

TODAY=`date +%Y-%m-%d`

CONF_FILE=$DIR/../migrations.conf

CONF_PERM=`ls -l $CONF_FILE | awk '{print $1}'`

# make this a bit more robust
if [ "$CONF_PERM" != "-rwx------@" ]
  then
    echo "Please change configuration file permission to be read only by owner"
fi

. $CONF_FILE

[ -d $DATABASES_DIRECTORY ] || die "Database directory $DATABASES_DIRECTORY does not exist"

[ -d $DATABASES_PROD_DIRECTORY ] || die "Database directory $DATABASES_PROD_DIRECTORY does not exist"

# remove old databases

echo "Find and remove old databases used for sites migration from $DATABASES_DIRECTORY"

find $DATABASES_DIRECTORY -type f -name "*.sql" -exec rm {} \;

# Rasan's script back up database production around 4am on mounted 
# drive: /content/prod/pa/backup/mc/mysql

# Copy over the latest production database shared and sites databases

# e.g., mysql.mc.tne_prod.2014-12-01-04-35-01.sql.bz2

DATABASES=$DATABASES_PROD_DIRECTORY/mysql.mc.*.$TODAY-*-*.sql.bz2

for db in $DATABASES
do

  # get the basename
  BASENAME=`basename $db`

  BASENAME_TRIM=${BASENAME/mysql.mc./}
  
  FILENAME=${BASENAME_TRIM/.$TODAY-*-*.sql.bz2/}

  cp $db $DATABASES_DIRECTORY/$FILENAME.sql.bz2
  
  chmod 775 $DATABASES_DIRECTORY/$FILENAME.sql.bz2
  
  # unzip the file
  echo "Uncompress $DATABASES_DIRECTORY/$FILENAME.sql.bz2"
  
  bzip2 -d $DATABASES_DIRECTORY/$FILENAME.sql.bz2
  
done

echo "Import DB"
for database in $DATABASES_IN_PRODUCTION
do 

  # SQL file to be imported
  TEMP=$DATABASES_DIRECTORY"/"$database"_prod.sql"

  # Database that will be updated with the SQL back-up from production
  TEMP_MIGRATIOB_DATABASE_NAME=$DATABASES_MIGRATION_PREFIX$database
  
  # check if SQL file is in the migration directory
  if [ -f $TEMP ]
    then
      
      # Check if the database exist
      RESULT=`mysql -u $DATABASES_DB_USER -p$DATABASES_DB_PASS -e "SHOW DATABASES" | grep -Fo $TEMP_MIGRATIOB_DATABASE_NAME`
      
      if [ "$RESULT" == "$TEMP_MIGRATIOB_DATABASE_NAME" ]; 
        then

          # database exist; import SQL file
          echo "Import SQL file $TEMP into $TEMP_MIGRATIOB_DATABASE_NAME"

          mysql -u $DATABASES_DB_USER -p$DATABASES_DB_PASS $TEMP_MIGRATIOB_DATABASE_NAME < $TEMP
        
      else  
      
          # database does not exist
          echo "Unable to import SQL file $TEMP into $TEMP_MIGRATIOB_DATABASE_NAME. Database does not exist"        
        
      fi

    fi  

done
