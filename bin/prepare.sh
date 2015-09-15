#!/bin/bash

# no longer in use ... don not use me!!!!!!


# Don't use me! ... well, do it if you know what you are doing.
#
# In theory I should work; but in practice I'm almost certain that I will
# fail if you run me in a machine that does not looks like the one
# I'm intended to run it. So, don't use me if you are not sure, I can 
# easily get you into trouble.

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  exit 1;
}

TODAY=`date +%Y-%m-%d`

while getopts ":s:h" opt; do
 case $opt in
  s)
    [ -r $OPTARG ] || die ${LINENO} "test" "Unable to read from ${OPTARG} directory." 
    SOURCE_DIRECTORY=$OPTARG
    ;;
  h)
   echo " "
   echo " Usage: ./prepare.sh -s /content/databases/source -d /content/databases/destination"
   echo " "
   echo " Options:"
   echo "   -h                Show brief help."
   echo "   -d <directory>    Specify path to directory where the script will save the databases."   
   echo "   -s <directory>    Specify path to directory where the script can find the databases."
   echo " "  
   exit 0
   ;;
  esac
done

[ $SOURCE_DIRECTORY ] || die ${LINENO} "error" "No source directory."

echo "Find and remove old databases used for sites migration from ${DESTINATION_DIRECTORY}."

# remove old databases
find $DESTINATION_DIRECTORY -type f -name "*.sql" -exec rm {} \;

# Copy over the latest production database shared and sites databases
# e.g., mysql.mc.tne_prod.2014-12-01-04-35-01.sql.bz2
DATABASES=$SOURCE_DIRECTORY/mysql.mc.*.$TODAY-*-*.sql.bz2

for db in $DATABASES ; do
  BASENAME=`basename $db`
  BASENAME_TRIM=${BASENAME/mysql.mc./}
  FILENAME=${BASENAME_TRIM/.$TODAY-*-*.sql.bz2/}
  cp $db $DESTINATION_DIRECTORY/$FILENAME.sql.bz2
  chmod 775 $DESTINATION_DIRECTORY/$FILENAME.sql.bz2
  echo "Uncompress ${DESTINATION_DIRECTORY}/${FILENAME}.sql.bz2"
  bzip2 -d $DESTINATION_DIRECTORY/$FILENAME.sql.bz2
done

exit 0
