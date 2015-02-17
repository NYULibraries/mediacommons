#!/bin/bash

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  exit 1;
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

DEBUG=""

while getopts ":c:hdb" opt; do
  case $opt in
    c)
      [ -f $OPTARG ] || die "Configuration file does not exist." 
      CONF_FILE=$OPTARG
      ;;
    d)
      DEBUG="-d -v"
      ;;
    h)
      echo " "
      echo " Usage: ./migrate.sh -c example.conf"
      echo " "
      echo " Options:"
      echo "   -h           Show brief help"
      echo "   -b           Build realpath"
      echo "   -c <file>    Specify the configuration file to use (e.g., -c example.conf)."
      echo " "
      exit 0
      ;;
  esac
done

[ $CONF_FILE ] || die ${LINENO} "test" "No configuration file provided."

# load configuration file
. $CONF_FILE

if [[ -f $BUILD_DIR/$BUILD_BASE_NAME/index.php ]]; then
  
  [ -w $BUILD_DIR/$BUILD_BASE_NAME/sites/default ] || die ${LINENO} "test" "Unable to write to ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default directory." ;
  
  [ -w $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php ] || die ${LINENO} "test" "Unable to write to ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/settings.php file." ;  
  
  # check if this directory looks like Drupal 7
  MATCH=`grep -c 'DRUPAL_ROOT' $BUILD_DIR/$BUILD_BASE_NAME/index.php` 
  
  if [ $MATCH -gt 0 ]; then
    D6_DATABASE=`echo $BUILD_BASE_NAME | sed 's/[^a-zA-Z]//g'`_d6_content
    D6_SHARED=`echo $BUILD_BASE_NAME | sed 's/[^a-zA-Z]//g'`_d6_shared
    DBSTRING="\$databases['drupal6'] = array("
    DBSTRING+="'default' => array("
    DBSTRING+="'database' => '$D6_DATABASE',"
    DBSTRING+="'username' => '$DRUPAL_SITE_DB_USER',"
    DBSTRING+="'password' => '$DRUPAL_SITE_DB_PASS',"
    DBSTRING+="'host' => '$DRUPAL_SITE_DB_ADDRESS',"
    DBSTRING+="'port' => '$DRUPAL_SITE_DB_PORT',"
    DBSTRING+="'driver' => '$DRUPAL_SITE_DB_TYPE',"
    DBSTRING+="'prefix' => array("
    DBSTRING+="'default' => '"$D6_CONTENT_DB_PREFIX"',"
    DBSTRING+="'authmap' => '"$D6_SHARED"."$D6_SHARED_DB_PREFIX"',"
    DBSTRING+="'profile_fields' => '"$D6_SHARED"."$D6_SHARED_DB_PREFIX"',"
    DBSTRING+="'profile_values' => '"$D6_SHARED"."$D6_SHARED_DB_PREFIX"',"
    DBSTRING+="'sequences' => '"$D6_SHARED"."$D6_SHARED_DB_PREFIX"',"
    DBSTRING+="'sessions' => '"$D6_SHARED"."$D6_SHARED_DB_PREFIX"',"
    DBSTRING+="'users' => '"$D6_SHARED"."$D6_SHARED_DB_PREFIX"',"
    DBSTRING+="'vocabulary' => '"$D6_SHARED"."$D6_SHARED_DB_PREFIX"',"
    DBSTRING+="'term_data' => '"$D6_SHARED"."$D6_SHARED_DB_PREFIX"',"
    DBSTRING+="),"
    DBSTRING+="),"
    DBSTRING+=");"
    
    # find if we have the file or we need to get it from source
    find ${BUILD_BASE_NAME}.content.sql -mtime -1 -print
    if [ $? -eq 0 ] ; 
      then 
        echo "Found ${BUILD_BASE_NAME}.content.sql in ${DIR}." ; 
      else 
        echo "Get ${BUILD_BASE_NAME}.content.sql from source" ; 
        curl -u build:+EiBAPkL5L] $PROD_CONTENT_DB_URL > $BUILD_BASE_NAME.content.sql ;
    fi ;
    
    # find if we have the file or we need to get it from source
    find ${BUILD_BASE_NAME}.share.sql -mtime -1 -print
    if [ $? -eq 0 ] ; 
      then 
        echo "Found ${BUILD_BASE_NAME}.share.sql in ${DIR}." ; 
      else 
        echo "Get ${BUILD_BASE_NAME}.share.sql from source" ; 
        curl -u build:+EiBAPkL5L] $PROD_SHARED_DB_URL > $BUILD_BASE_NAME.share.sql ;
    fi ;
    
    mysql --user=$DRUPAL_SITE_DB_USER --password=$DRUPAL_SITE_DB_PASS -e "DROP DATABASE IF EXISTS ${D6_DATABASE}; CREATE DATABASE ${D6_DATABASE}; DROP DATABASE IF EXISTS ${D6_SHARED}; CREATE DATABASE ${D6_SHARED};"
    mysql --user=$DRUPAL_SITE_DB_USER --password=$DRUPAL_SITE_DB_PASS $D6_DATABASE < $BUILD_BASE_NAME.content.sql
    mysql --user=$DRUPAL_SITE_DB_USER --password=$DRUPAL_SITE_DB_PASS $D6_SHARED < $BUILD_BASE_NAME.share.sql
    
    # make a copy of the settings file
    cp $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php.${BUILD_DATE}.off
    
    # owner or sudo can read
    chmo 700 $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php.${BUILD_DATE}.off

    # Append database set-up to settings.php file
    echo $DBSTRING >> $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php

    # we have what it looks like a Drupal site
    # test if we can connect to the database and login as user 1
    # `drush -d -v core-status` should return:
    #   - Successfully connected to the Drupal database
    #   - Successfully logged into Drupal
    SITE_ONLINE=`drush -d -v core-status --uri=$BASE_URL --root=$BUILD_DIR/$BUILD_BASE_NAME --user=1`    
    if [[ $SITE_ONLINE =~ "Connected" ]] && [[ $SITE_ONLINE =~ "Successful" ]] ; 
      then
        echo $MIGRATION_TASK
        drush $DEBUG scr $MIGRATION_SCRIPT --uri=$BASE_URL --root=$BUILD_DIR/$BUILD_BASE_NAME --user=1 --task="${MIGRATION_TASK}"
      else
        die ${LINENO} "test" "Unable to connect to Drupal. URI: ${BASE_URL} ROOT: ${BUILD_DIR}/${BUILD_BASE_NAME}"  
    fi
  else
    die ${LINENO} "test" "Unable to ${MIGRATION_TASK} make sure ${BUILD_DIR}/${BUILD_BASE_NAME} it's the root directory of Drupal 7 installation."
  fi
fi

exit 0
