#!/bin/bash

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  exit 1;
}

while getopts ":c:h" opt; do
 case $opt in
  c)
   [ -f $OPTARG ] || die ${LINENO} "error" "Configuration file does not exist."
   CONF_FILE=$OPTARG
   ;;
  h)
   echo " "
   echo " Usage: ./enable_modules.sh -c example.conf"
   echo " "
   echo " Options:"
   echo "   -c <file>    Specify the configuration file to use (e.g., -c example.conf)."
   echo "   -h           Show brief help"
   echo " "
   exit 0
   ;;
  esac
done

[ $CONF_FILE ] || die ${LINENO} "error" "No configuration file provided"

# load configuration file
. $CONF_FILE

if [ -z ${DRUSH+x} ]; then die ${LINENO} "test" "Fail: Drush is not set"; fi ;

[ -d ${BUILD_DIR}/${BUILD_BASE_NAME} ] || die "Build directory ${BUILD_DIR}/${BUILD_BASE_NAME} does not exist"

[ ! `grep -q 'DRUPAL_ROOT' ${BUILD_DIR}/${BUILD_BASE_NAME}/index.php` ] || die "${BUILD_DIR}/${BUILD_BASE_NAME} does not look like a Drupal installation folder."

for i in $(echo $MODULES | sed "s/,/ /g")
  do
    ${DRUSH} en ${i} --root=${BUILD_DIR}/${BUILD_BASE_NAME} -y
done

exit 0
