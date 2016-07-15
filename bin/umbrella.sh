#!/bin/bash

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  rm ${TEMP_DIR}/autobuild.pid
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

while getopts ":s" opt; do
  case $opt in
    s)
      SKIP=true
      ;;
  esac
done

ENVIRONMENT="development"

DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

ROOT=/www/sites/drupal/scripts/mediacommons

TEMP_DIR=${ROOT}/temp

DBS=/www/sites/drupal/databases/migrations/dbs

# Take care of importing databasaes

# set-up the shared database;
# mysql shared < ${ROOT}/bin/shared.sql;

if [ ! $SKIP ];
  then
    # ALT-AC
    echo "Import ALT-AC content database"
    mysql altac_d6_content < ${DBS}/alt-ac.sql
    echo "Import ALT-AC shared database"
    mysql altac_d6_shared < ${DBS}/mcshared.sql

    # Fieldguide
    echo "Import Fieldguide content database"
    mysql fieldguide_d6_content < ${DBS}/mediacommons.sql
    echo "Import Fieldguide shared database"
    mysql fieldguide_d6_shared < ${DBS}/mcshared.sql

    # In Media Res
    echo "Import In Media Res content database"
    mysql imr_d6_content < ${DBS}/imr.sql
    echo "Import In Media Res shared database"
    mysql imr_d6_shared < ${DBS}/mcshared.sql

    # [in]Transition
    echo "Import [in]Transition content database"
    mysql intransition_d6_content <${DBS}/intransition.sql
    echo "Import [in]Transition shared database"
    mysql intransition_d6_shared < ${DBS}/mcshared.sql

    # MediaCommons
    echo "Import MediaCommons content database"
    mysql mediacommons_d6_content < ${DBS}/mediacommons.sql
    echo "Import MediaCommons shared database"
    mysql mediacommons_d6_shared < ${DBS}/mcshared.sql

    # The New Everyday
    echo "Import The New Everyday content database"
    mysql tne_d6_content < ${DBS}/tne.sql
    echo "Import The New Everyday content database"
    mysql tne_d6_shared < ${DBS}/mcshared.sql
fi

# Build Umbrella

[ ${ROOT}/configs/mediacommons.conf ] || die ${LINENO} "test" "No configuration file provided."

# load configuration file
. ${ROOT}/configs/mediacommons.conf

${ROOT}/bin/build.sh -c ${ROOT}/configs/mediacommons.conf -m ${ROOT}/mediacommons.make -l -e ${ENVIRONMENT} -k;

if [ $? -eq 0 ];
  then
    echo "Successful: Build MediaCommons Umbrella";
    # Run preprocess task
    echo "Run preprocess task";
    ${ROOT}/bin/utilities/preprocess.sh -c ${ROOT}/configs/mediacommons.conf;

    [ -w $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php ] || die ${LINENO} "test" "Unable to write to ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/settings.php file." ;

    # Umbrella needs information about other all sites

    # Database connection to In Media Res Drupal 6
    IMR6="\$databases['imr6'] = array('default' =>  array('database' => 'imr_d6_content', 'username' => '${DRUPAL_SITE_DB_USER}', 'password' => '${DRUPAL_SITE_DB_PASS}', 'host' => '${DRUPAL_SITE_DB_ADDRESS}', 'port' => '${DRUPAL_SITE_DB_PORT}', 'driver' => 'mysql', 'prefix' => array('default' => 'imr_')));"

    # Append database set-up to settings.php file
    echo $IMR6 >> $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php

    # Database connection to [in]Transition Drupal 6
    INT6="\$databases['int6'] = array('default' =>  array('database' => 'intransition_d6_content', 'username' => '${DRUPAL_SITE_DB_USER}', 'password' => '${DRUPAL_SITE_DB_PASS}', 'host' => '${DRUPAL_SITE_DB_ADDRESS}', 'port' => '${DRUPAL_SITE_DB_PORT}', 'driver' => 'mysql', 'prefix' => array('default' => 'intransition_')));"

    # Append database set-up to settings.php file
    echo $INT6 >> $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php

    # Migrate the content
    ${ROOT}/bin/migrate.sh -c ${ROOT}/configs/mediacommons.conf;

    # Export database
    ${ROOT}/bin/utilities/export_db.sh -c ${ROOT}/configs/mediacommons.conf;

  else
    echo ${LINENO} "build" "Fail: Build ${project}";
fi;

