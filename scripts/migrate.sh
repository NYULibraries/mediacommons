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

DEBUG=""

while getopts ":c:hd" opt; do
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
      echo "   -c <file>    Specify the configuration file to use (e.g., -c example.conf)."
      echo " "
      exit 0
      ;;
  esac
done

[ $CONF_FILE ] || die "No configuration file provided."

# load configuration file
. $CONF_FILE

if [[ -f $BUILD_DIR/$BUILD_BASE_NAME/index.php ]]; then
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
    
    curl -u build:+EiBAPkL5L] $PROD_SHARED_DB_URL > $BUILD_BASE_NAME.share.sql

    curl -u build:+EiBAPkL5L] $PROD_CONTENT_DB_URL > $BUILD_BASE_NAME.content.sql

    mysql --user=$DRUPAL_SITE_DB_USER --password=$DRUPAL_SITE_DB_PASS -e "DROP DATABASE IF EXISTS $D6_DATABASE; CREATE DATABASE $D6_DATABASE;"
    mysql --user=$DRUPAL_SITE_DB_USER --password=$DRUPAL_SITE_DB_PASS -e "DROP DATABASE IF EXISTS $D6_SHARED; CREATE DATABASE $D6_SHARED;"

    mysql --user=$DRUPAL_SITE_DB_USER --password=$DRUPAL_SITE_DB_PASS $D6_DATABASE < $BUILD_BASE_NAME.content.sql
    mysql --user=$DRUPAL_SITE_DB_USER --password=$DRUPAL_SITE_DB_PASS $D6_SHARED < $BUILD_BASE_NAME.share.sql

    chmod 777 $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php

    cp $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php.old

    echo $DBSTRING >> $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php

    chmod 755 $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php

    chmod 755 $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php.old

    # we have what it looks like a Drupal site
    # test if we can connect to the database and login as user 1
    # `drush -d -v core-status` should return:
    #   - Successfully connected to the Drupal database
    #   - Successfully logged into Drupal
    SITE_ONLINE=`drush -d -v core-status --uri=$BASE_URL --root=$BUILD_DIR/$BUILD_BASE_NAME --user=1`    
    if [[ $SITE_ONLINE =~ "Connected" ]] && [[ $SITE_ONLINE =~ "Successful" ]] ; then
      echo $MIGRATION_TASK
      drush $DEBUG scr $MIGRATION_SCRIPT --uri=$BASE_URL --root=$BUILD_DIR/$BUILD_BASE_NAME --user=1 --task="$MIGRATION_TASK"
    fi
  else
    echo "Unable to $MIGRATION_TASK make sure $BUILD_DIR/$BUILD_BASE_NAME it's the root directory of Drupal 7 installation."
  fi
fi

exit 0
