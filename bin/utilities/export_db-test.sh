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

if [ -z ${DRUSH+x} ]; then die ${LINENO} "test" "Fail: Drush is not set"; fi ;

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

    echo "$DRUSH $DEBUG cc all --uri=${BASE_URL} --root=${BUILD_DIR}/${BUILD_BASE_NAME} --user=1 --strict=0"
    echo "$DRUSH $DEBUG sql-dump --uri=${BASE_URL} --root=${BUILD_DIR}/${BUILD_BASE_NAME} --user=1 --environment=${ENVIRONMENT} --strict=0 > ${SQL_DUMP_DESTINATION}/${SQL_DUMP_FILENAME}"

  else
    die ${LINENO} "test" "Unable to export DB make sure ${BUILD_DIR}/${BUILD_BASE_NAME} it's the root directory of Drupal 7 installation."
  fi
fi

rm ${TEMP_DIR}/${BUILD_BASE_NAME}.export.pid;

exit 0
