#!/bin/bash

set -x

# if anything in this script fail; FAIL big! WE MUST STOP any other
# process related to autobuild.sh.

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  rm -f ${TEMP_DIR}/autobuild.pid
  exit 1;
}

while getopts ":m:c:s" opt; do
  case $opt in
    c)
      [ -f $OPTARG ] || die "Configuration file does not exist."
      CONF_FILE=$OPTARG
      ;;
    s)
      SKIP=true
      ;;
    m)
      [ -f $OPTARG ] || die "Make file does not exist."
      MAKE_FILE=$OPTARG
      ;;
  esac
done

[ $CONF_FILE ] || die ${LINENO} "fail" "No configuration file provided."

# load configuration file
. $CONF_FILE

# Take care of importing databasaes
if [ ! $SKIP ];
  then
    echo ${DBS}/mcshared.sql
    if [ -r ${DBS}/mcshared.sql ]
      then
        # MediaCommons share tables (user tables and others)
        # we use the same database for each site later on
        # to solve some issues with colliding ids (IMR and
        # [in]T taxonomy) we map them by requesting MediaCommons
        # Umbrela to be the first migrated
        echo "Import shared database"
        mysql altac_d6_shared < ${DBS}/mcshared.sql
        mysql fieldguide_d6_shared < ${DBS}/mcshared.sql
        mysql imr_d6_shared < ${DBS}/mcshared.sql
        mysql intransition_d6_shared < ${DBS}/mcshared.sql
        mysql mediacommons_d6_shared < ${DBS}/mcshared.sql
        mysql tne_d6_shared < ${DBS}/mcshared.sql
      else
        die ${LINENO} "test" "Unable to read shared database"
    fi
    if [ -r ${DBS}/mediacommons.sql ]
      then
        echo "Import Fieldguide database"
        mysql fieldguide_d6_content < ${DBS}/mediacommons.sql
        echo "Import MediaCommons database"
        mysql mediacommons_d6_content < ${DBS}/mediacommons.sql
      else
        die ${LINENO} "test" "Unable to read Mediacommons content database"
    fi
    if [ -r ${DBS}/alt-ac.sql ]
      then
        echo "Import ALT-AC database"
        mysql altac_d6_content < ${DBS}/alt-ac.sql
      else
        die ${LINENO} "test" "Unable to read ALT-AC database"
    fi
    if [ -r ${DBS}/imr.sql ]
      then
        echo "Import In Media Res database"
        mysql imr_d6_content < ${DBS}/imr.sql
      else
        die ${LINENO} "test" "Unable to read In Media Res database"
    fi
    if [ -r ${DBS}/intransition.sql ]
      then
        echo "Import [in]Transition database"
        mysql intransition_d6_content <${DBS}/intransition.sql
      else
        die ${LINENO} "test" "Unable to read [in]Transition database"
    fi
    if [ -r ${DBS}/intransition.sql ]
      then
        echo "Import The New Everyday database"
        mysql tne_d6_content < ${DBS}/tne.sql
      else
        die ${LINENO} "test" "Unable to read [in]Transition database"
    fi
fi

# Build Umbrella
[ ${ROOT}/configs/mediacommons.conf ] || die ${LINENO} "test" "No configuration file provided."

# Load configuration file
. ${ROOT}/configs/mediacommons.conf

[ $MAKE_FILE ] || MAKE_FILE=${ROOT}/mediacommons.make

echo "Build script will use ${MAKE_FILE} to build the project"

${ROOT}/bin/build.sh -c ${ROOT}/configs/mediacommons.conf -m ${MAKE_FILE} -k -e ${ENVIRONMENT};

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
    echo "Set-up and clean-up others";
    ${ROOT}/bin/utilities/postprocess.sh -c ${ROOT}/configs/mediacommons.conf;
    echo "Set-up Apache Solr";
    ${ROOT}/bin/utilities/apachesolr.sh -c ${ROOT}/configs/mediacommons.conf;
  else
   die ${LINENO} "test" "build" "Fail: Unable to build";
fi;

exit  0
