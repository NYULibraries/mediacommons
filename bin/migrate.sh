#!/bin/bash

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  rm -f ${DIR}/../temp/${BUILD_BASE_NAME}.migrate.pid;
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

if [ -z ${DRUSH+x} ]; then die ${LINENO} "test" "Fail: Drush is not set"; fi ;

# Here I need to test if the migration is running and kill this process
# or remove the pid file and keep going
if [[ -f ${TEMP_DIR}/${BUILD_BASE_NAME}.migrate.pid ]]; then
  rm -f ${TEMP_DIR}/${BUILD_BASE_NAME}.migrate.pid;
fi

echo $$ > ${TEMP_DIR}/${BUILD_BASE_NAME}.migrate.pid

PROD_CONTENT_DB_BASENAME=`basename $PROD_CONTENT_DB_URL`

PROD_SHARED_DB_BASENAME=`basename $PROD_SHARED_DB_URL`

D6_DATABASE=`echo $BUILD_BASE_NAME | sed 's/[^a-zA-Z]//g'`_d6_content

D6_SHARED=`echo $BUILD_BASE_NAME | sed 's/[^a-zA-Z]//g'`_d6_shared

# not sure about this
MEDIACOMMONS_SHARED=mediacommons

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
    # JIRA: MC-156
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
    # Append database set-up to settings.php file
    echo $DBSTRING >> $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php
    # While migrating we share databases to speed-up the migration process
    cp  $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php $BUILD_DIR/$BUILD_BASE_NAME/sites/default/unshare.settings.php
    # note the dot after ${MEDIACOMMONS_SHARED}, this is needed to allow Drupal to
    # get access to the share database
    echo "define('MEDIACOMMONS_SHARED', '${MEDIACOMMONS_SHARED}.');" >> $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php
    cat ${DIR}/shared_fields.txt >> $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php
    # we have what it looks like a Drupal site
    # test if we can connect to the database and login as user 1
    # `drush -d -v core-status` should return:
    #   - Successfully connected to the Drupal database
    #   - Successfully logged into Drupal
    ${DRUSH} -d -v core-status --uri=$BASE_URL --root=$BUILD_DIR/$BUILD_BASE_NAME --user=1
    if [ $? -eq 0 ];
      then
        ${DRUSH} $DEBUG scr $MIGRATION_SCRIPT --uri=$BASE_URL --root=$BUILD_DIR/$BUILD_BASE_NAME --user=1 --environment=${ENVIRONMENT} --strict=0 --task="${MIGRATION_TASK}"
        # Stop sharing databases
        mv $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php.shared.off
        # Leave behind a copy of the ready-to-share settings.php file to be used
        # if the developer want to share databases
        mv $BUILD_DIR/$BUILD_BASE_NAME/sites/default/unshare.settings.php $BUILD_DIR/$BUILD_BASE_NAME/sites/default/settings.php
        # tables we share
        TABLES="mediacommons_base_import_vocabulary_map mediacommons_base_import_term_map taxonomy_vocabulary taxonomy_term_data taxonomy_term_hierarchy mediacommons_base_import_user_map mediacommons_base_import_role_map field_revision_field_url field_revision_field_twitter field_revision_field_title field_revision_field_subtitle field_revision_field_state field_revision_field_skype field_revision_field_profile_name field_revision_field_plan field_revision_field_phone field_revision_field_organization field_revision_field_last_name field_revision_field_first_name field_revision_field_email field_revision_field_bio field_revision_field_aim field_data_field_url field_data_field_twitter field_data_field_title field_data_field_subtitle field_data_field_state field_data_field_skype field_data_field_profile_name field_data_field_plan field_data_field_phone field_data_field_organization field_data_field_last_name field_data_field_first_name field_data_field_email field_data_field_bio field_data_field_aim cache_gravatar block_role blocked_ips shortcut_set_users realname users users_roles sessions role role_permission authmap"
        # dump the tables
        mysqldump ${MEDIACOMMONS_SHARED} ${TABLES} > ${TEMP_DIR}/shared.sql
        # import data
        mysql ${DRUPAL_DB_NAME} < ${TEMP_DIR}/shared.sql
      else
        die ${LINENO} "test" "Drupal: Unable to connect. URI: ${BASE_URL} ROOT: ${BUILD_DIR}/${BUILD_BASE_NAME}"
    fi
  else
    die ${LINENO} "test" "Unable to ${MIGRATION_TASK} make sure ${BUILD_DIR}/${BUILD_BASE_NAME} it's the root directory of Drupal 7 installation."
  fi
fi

if [[ -f ${TEMP_DIR}/${BUILD_BASE_NAME}.migrate.pid ]]; then
  rm -f ${TEMP_DIR}/${BUILD_BASE_NAME}.migrate.pid;
fi

exit 0
