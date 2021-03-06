#!/bin/bash

PID_FILE=${TEMP_DIR}/${BUILD_BASE_NAME}.import.pid

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  rm ${PID_FILE}
  exit 1;
}

while getopts ":c:h" opt; do
  case $opt in
    c)
      [ -f $OPTARG ] || die "Configuration file does not exist."
      CONF_FILE=$OPTARG
      ;;
    h)
      echo " "
      echo " Usage: ./import_db.sh -c example.conf"
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

if [ -z ${DRUSH+x} ]; then die ${LINENO} "test" "Fail: Drush is not set"; fi ;

[ -w $TEMP_DIR ] || die ${LINENO} "test" "Unable to write to ${TEMP_DIR}";

echo $$ > ${PID_FILE}

if [[ -f $BUILD_DIR/$BUILD_BASE_NAME/index.php ]]; then
  # Get database from source
  curl -u ${CURL_USER}:${CURL_PASS} ${SQL_IMPORT_URL} > ${TEMP_DIR}/$BUILD_BASE_NAME.d7.sql
  # Danger zone
  # Drop database
  ${DRUSH} sql-drop -y --uri=${BASE_URL} --root=${BUILD_DIR}/${BUILD_BASE_NAME} --user=1
  # Be afraid and do only if you know what you are doing
  `${DRUSH} sql-connect -y --uri=${BASE_URL} --root=${BUILD_DIR}/${BUILD_BASE_NAME} --user=1` < ${TEMP_DIR}/$BUILD_BASE_NAME.d7.sql
fi

exit 0
