#!/bin/bash

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  exit 1;
}

OVERWRITE_DRUPAL_SPECIAL_BODY_CLASS=""

while getopts ":c:d:h" opt; do
 case $opt in
  c)
   [ -f $OPTARG ] || die ${LINENO} "error" "Configuration file does not exist."
   CONF_FILE=$OPTARG
   ;;
  d)
    OVERWRITE_DRUPAL_SPECIAL_BODY_CLASS=$OPTARG
    ;;
  h)
   echo " "
   echo " Usage: ./theme_variable.sh -c example.conf -d mediacommons"
   echo " "
   echo " Options:"
   echo "   -c <file>    Specify the configuration file to use (e.g., -c example.conf)."
   echo "   -d <sting>   Special body class used by MediaCommons theme"
   echo "   -h           Show brief help"
   echo " "
   exit 0
   ;;
  esac
done

[ $CONF_FILE ] || die ${LINENO} "error" "No configuration file provided"

# load configuration file
. $CONF_FILE

[ $DRUPAL_SPECIAL_BODY_CLASS ] || die ${LINENO} "error" "No special class"

if [ $OVERWRITE_DRUPAL_SPECIAL_BODY_CLASS ]; then
  DRUPAL_SPECIAL_BODY_CLASS=${OVERWRITE_DRUPAL_SPECIAL_BODY_CLASS}
fi;

[ ! `grep -q 'DRUPAL_ROOT' ${BUILD_DIR}/${BUILD_BASE_NAME}/index.php` ] || die "${BUILD_DIR}/${BUILD_BASE_NAME} does not look like a Drupal installation folder."

# make sure our drush command is register
drush cache-clear drush --root=${BUILD_DIR}/${BUILD_BASE_NAME}

# run command
drush set-theme-setting-class-name ${DRUPAL_SPECIAL_BODY_CLASS} --root=${BUILD_DIR}/${BUILD_BASE_NAME}

exit 0
