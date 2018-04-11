#!/bin/bash

echo ${0}

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
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
   echo " Usage: ./cleanup.sh -c example.conf"
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

[ -d ${BUILD_DIR}/${BUILD_BASE_NAME} ] || die ${LINENO} "error" "Not found ${BUILD_DIR}/${BUILD_BASE_NAME}"

[ ! `grep -q 'DRUPAL_ROOT' ${BUILD_DIR}/${BUILD_BASE_NAME}/index.php` ] || die ${LINENO} "error" "${BUILD_DIR}/${BUILD_BASE_NAME} not a Drupal installation folder"

if [[ -f ${BUILD_DIR}/${BUILD_BASE_NAME}/.htaccess ]]; then
  mv ${BUILD_DIR}/${BUILD_BASE_NAME}/.htaccess ${BUILD_DIR}/${BUILD_BASE_NAME}/.htaccess.off
  # but no one should read it (unless super user)
  chmod 000 ${BUILD_DIR}/${BUILD_BASE_NAME}/.htaccess.off
fi

if [[ -f ${BUILD_DIR}/${BUILD_BASE_NAME}/robots.txt ]]; then
  mv ${BUILD_DIR}/${BUILD_BASE_NAME}/robots.txt ${BUILD_DIR}/${BUILD_BASE_NAME}/robots.txt.bk
fi

# remove any file that can easily "reveal" (not prevent) our Drupal set-up
rm ${BUILD_DIR}/${BUILD_BASE_NAME}/*.txt

# do the robot
if [[ -f ${BUILD_DIR}/${BUILD_BASE_NAME}/robots.txt.bk ]]; then
  mv ${BUILD_DIR}/${BUILD_BASE_NAME}/robots.txt.bk ${BUILD_DIR}/${BUILD_BASE_NAME}/robots.txt
fi

# we run on Apache; no need for IIS configuration file
if [[ -f ${BUILD_DIR}/${BUILD_BASE_NAME}/web.config ]]; then
  rm ${BUILD_DIR}/${BUILD_BASE_NAME}/web.config
fi

# keep install.php file around
if [[ -f ${BUILD_DIR}/${BUILD_BASE_NAME}/install.php ]]; then
  mv ${BUILD_DIR}/${BUILD_BASE_NAME}/install.php ${BUILD_DIR}/${BUILD_BASE_NAME}/install.php.off
  # but no one should read it (unless super user)
  chmod 000 ${BUILD_DIR}/${BUILD_BASE_NAME}/install.php.off
fi

exit 0
