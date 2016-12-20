#!/bin/bash

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  rm ${TEMP_DIR}/${BUILD_BASE_NAME}.export.pid;
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

DRUSH=$DIR/../drush

ENVIRONMENT="local"

while getopts ":c:h" opt; do
  case $opt in
    c)
      [ -f $OPTARG ] || die "Configuration file does not exist."
      CONF_FILE=$OPTARG
      ;;
    h)
      echo " "
      echo " Usage: ./export_db.sh -c example.conf"
      echo " "
      echo " Options:"
      echo "   -c <file>    Specify the configuration file to use (e.g., -c example.conf)."
      echo "   -h           Show brief help"
      echo " "
      exit 0
      ;;
  esac
done

[ $CONF_FILE ] || die ${LINENO} "test" "No configuration file provided."

# load configuration file
. $CONF_FILE

[ -w $TEMP_DIR ] || die ${LINENO} "test" "Unable to write to ${TEMP_DIR}";

echo $$ > ${TEMP_DIR}/${BUILD_BASE_NAME}.export.pid

if [[ -f $BUILD_DIR/$BUILD_BASE_NAME/index.php ]]; then
  [ -w ${SQL_DUMP_DESTINATION} ] || die ${LINENO} "test" "Unable to write inside ${SQL_DUMP_DESTINATION}";
  # check if this directory looks like Drupal 7
  MATCH=`grep -c 'DRUPAL_ROOT' ${BUILD_DIR}/${BUILD_BASE_NAME}/index.php`
  if [ $MATCH -gt 0 ]; then
    # we have what it looks like a Drupal site
    # test if we can connect to the database and login as user 1
    # `drush -d -v core-status` should return:
    #   - Successfully connected to the Drupal database
    #   - Successfully logged into Drupal
    SITE_ONLINE=`$DRUSH -d -v core-status --uri=${BASE_URL} --root=${BUILD_DIR}/${BUILD_BASE_NAME} --user=1`
    if [[ ${SITE_ONLINE} =~ "Successful" ]] ;
      then
        $DRUSH $DEBUG cc all --uri=${BASE_URL} --root=${BUILD_DIR}/${BUILD_BASE_NAME} --user=1 --strict=0
        $DRUSH $DEBUG sql-dump --uri=${BASE_URL} --root=${BUILD_DIR}/${BUILD_BASE_NAME} --user=1 --environment=${ENVIRONMENT} --strict=0 > ${SQL_DUMP_DESTINATION}/${SQL_DUMP_FILENAME}
        if [[ -f ${SQL_DUMP_DESTINATION}/${SQL_DUMP_FILENAME} ]]; then
          echo "Success: See dump database ${SQL_DUMP_DESTINATION}/${SQL_DUMP_FILENAME}"
          # Add the shared tables from "origin"
          if [[ $DRUPAL_DB_NAME != *"mediacommons"* ]]
            then
              # dump the shared tables
              TABLES="mediacommons_base_import_vocabulary_map mediacommons_base_import_term_map taxonomy_vocabulary taxonomy_term_data taxonomy_term_hierarchy mediacommons_base_import_user_map mediacommons_base_import_role_map field_revision_field_url field_revision_field_twitter field_revision_field_title field_revision_field_subtitle field_revision_field_state field_revision_field_skype field_revision_field_profile_name field_revision_field_plan field_revision_field_phone field_revision_field_organization field_revision_field_last_name field_revision_field_first_name field_revision_field_email field_revision_field_bio field_revision_field_aim field_data_field_url field_data_field_twitter field_data_field_title field_data_field_subtitle field_data_field_state field_data_field_skype field_data_field_profile_name field_data_field_plan field_data_field_phone field_data_field_organization field_data_field_last_name field_data_field_first_name field_data_field_email field_data_field_bio field_data_field_aim cache_gravatar block_role blocked_ips shortcut_set_users realname users users_roles sessions role role_permission authmap"
              DUMP=`mysqldump mediacommons ${TABLES}`
              echo "/* Umbrella dump to overwrite to avoid multiple databases dependency while we develop */;" >> ${SQL_DUMP_DESTINATION}/${SQL_DUMP_FILENAME}
              echo ${DUMP} >> ${SQL_DUMP_DESTINATION}/${SQL_DUMP_FILENAME}                            
              echo "/* end: Umbrella dump */;" >> ${SQL_DUMP_DESTINATION}/${SQL_DUMP_FILENAME}                            
          fi          
        else
          die ${LINENO} "test" "Unable to dump database ${SQL_DUMP_DESTINATION}/${SQL_DUMP_FILENAME}"
        fi
      else
        die ${LINENO} "test" "Unable to connect URI: ${BASE_URL} ROOT: ${BUILD_DIR}/${BUILD_BASE_NAME}"
    fi
  else
    die ${LINENO} "test" "Unable to export DB make sure ${BUILD_DIR}/${BUILD_BASE_NAME} it's the root directory of Drupal 7 installation."
  fi
fi

rm ${TEMP_DIR}/${BUILD_BASE_NAME}.export.pid;

exit 0
