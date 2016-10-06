#!/bin/bash

# This file has a dependency cron.sh
# Note: cron.sh is not part of this repo, in its own private repo for security reasons

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
   echo " Usage: ./preprocess.sh -c example.conf"
   echo " "
   echo " Options:"
   echo "   -c <file>    Specify the configuration file to use (e.g., -c example.conf)."
   echo "   -h           Show brief help"
   echo " "
   exit 0
   ;;
  esac
done

[ $CONF_FILE ] || die ${LINENO} 1 "No configuration file provided"

# Step # 1
# load configuration file
. $CONF_FILE

if [[ -f ${BUILD_DIR}/${BUILD_BASE_NAME}/index.php ]]; then
  # check if this directory looks like Drupal 7
  MATCH=`grep -c 'DRUPAL_ROOT' ${BUILD_DIR}/${BUILD_BASE_NAME}/index.php`
  if [ $MATCH -gt 0 ]; then
    # share cookie
    cat ${BUILD_APP_ROOT}/bin/utilities/no_cookies.txt >> ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/settings.php
  fi
fi

exit 0
