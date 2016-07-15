#!/bin/bash

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  rm ${DIR}/../temp/${BUILD_BASE_NAME}.migrate.pid ;
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

ENVIRONMENT="local"

while getopts ":c:hdb" opt; do
  case $opt in
    c)
      [ -f $OPTARG ] || die "Configuration file does not exist."
      CONF_FILE=$OPTARG
      ;;
    e)
      ENVIRONMENT=$OPTARG
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

# Here I need to test if the migration is running and kill this process
# or remove the pid file and keep going
if [[ -f ${TEMP_DIR}/${BUILD_BASE_NAME}.migrate.pid ]]; then
  rm ${TEMP_DIR}/${BUILD_BASE_NAME}.migrate.pid;
fi

echo $$ > ${TEMP_DIR}/${BUILD_BASE_NAME}.migrate.pid

PROD_CONTENT_DB_BASENAME=`basename $PROD_CONTENT_DB_URL`

PROD_SHARED_DB_BASENAME=`basename $PROD_SHARED_DB_URL`

D6_DATABASE=`echo $BUILD_BASE_NAME | sed 's/[^a-zA-Z]//g'`_d6_content

D6_SHARED=`echo $BUILD_BASE_NAME | sed 's/[^a-zA-Z]//g'`_d6_shared

[ -w $TEMP_DIR ] || die ${LINENO} "test" "Unable to write to ${TEMP_DIR}" ;

if [[ -f $BUILD_DIR/$BUILD_BASE_NAME/index.php ]]; then

  [ -w $BUILD_DIR/$BUILD_BASE_NAME/sites/default ] || die ${LINENO} "test" "Unable to write to ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default directory." ;

  [ -w $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php ] || die ${LINENO} "test" "Unable to write to ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/settings.php file." ;

  # check if this directory looks like Drupal 7
  MATCH=`grep -c 'DRUPAL_ROOT' $BUILD_DIR/$BUILD_BASE_NAME/index.php`

  if [ $MATCH -gt 0 ]; then
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
    DBSTRING+="'realname' => '"$D6_SHARED"."$D6_SHARED_DB_PREFIX"',"
    DBSTRING+="'profile_fields' => '"$D6_SHARED"."$D6_SHARED_DB_PREFIX"',"
    DBSTRING+="'profile_values' => '"$D6_SHARED"."$D6_SHARED_DB_PREFIX"',"
    DBSTRING+="'sequences' => '"$D6_SHARED"."$D6_SHARED_DB_PREFIX"',"
    DBSTRING+="'users' => '"$D6_SHARED"."$D6_SHARED_DB_PREFIX"',"
    if [[ $BUILD_BASE_NAME != *"intransition"* ]] && [[ $BUILD_BASE_NAME != *"imr"* ]]
      then
      DBSTRING+="'vocabulary' => '"$D6_SHARED"."$D6_SHARED_DB_PREFIX"',"
      DBSTRING+="'term_data' => '"$D6_SHARED"."$D6_SHARED_DB_PREFIX"',"
    fi
    DBSTRING+="),"
    DBSTRING+="),"
    DBSTRING+=");"

    # make a copy of the settings file
    cp $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php.${BUILD_DATE}.off
    
    # owner or sudo can read
    chmod 700 $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php.${BUILD_DATE}.off

    # Append database set-up to settings.php file
    echo $DBSTRING >> $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php
    
    cat ${DIR}/shared_fields.txt >> $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php

    # we have what it looks like a Drupal site
    # test if we can connect to the database and login as user 1
    # `drush -d -v core-status` should return:
    #   - Successfully connected to the Drupal database
    #   - Successfully logged into Drupal
    SITE_ONLINE=`drush -d -v core-status --uri=$BASE_URL --root=$BUILD_DIR/$BUILD_BASE_NAME --user=1`
    if [[ $SITE_ONLINE =~ "Connected" ]] && [[ $SITE_ONLINE =~ "Successful" ]] ;
      then
        drush $DEBUG scr $MIGRATION_SCRIPT --uri=$BASE_URL --root=$BUILD_DIR/$BUILD_BASE_NAME --user=1 --environment=${ENVIRONMENT} --strict=0 --task="${MIGRATION_TASK}"
      else
        die ${LINENO} "test" "Drupal: Unable to connect. URI: ${BASE_URL} ROOT: ${BUILD_DIR}/${BUILD_BASE_NAME}"
    fi
  else
    die ${LINENO} "test" "Unable to ${MIGRATION_TASK} make sure ${BUILD_DIR}/${BUILD_BASE_NAME} it's the root directory of Drupal 7 installation."
  fi
fi

if [[ -f ${TEMP_DIR}/${BUILD_BASE_NAME}.migrate.pid ]]; then
  rm ${TEMP_DIR}/${BUILD_BASE_NAME}.migrate.pid;
fi

exit 0
