#!/bin/bash

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  exit 1;
}

while getopts ":c:hdb" opt; do
  case $opt in
    c)
      [ -f $OPTARG ] || die ${LINENO} "error" "Configuration file does not exist"
      CONF_FILE=$OPTARG
      ;;
    h)
      echo " "
      echo " Usage: ./cookies.sh -c example.conf"
      echo " "
      echo " Options:"
      echo "   -h           Show brief help"
      echo "   -c <file>    Specify the configuration file to use (e.g., -c example.conf)."
      echo " "
      exit 0
      ;;
  esac
done

[ $CONF_FILE ] || die ${LINENO} "build" "Fail: No configuration file provided."

# load configuration file
. $CONF_FILE

if [[ -f ${BUILD_DIR}/${BUILD_BASE_NAME}/index.php ]]; then
  # check if this directory looks like Drupal 7
  MATCH=`grep -c 'DRUPAL_ROOT' ${BUILD_DIR}/${BUILD_BASE_NAME}/index.php`
  if [ $MATCH -gt 0 ]; then
    # share cookie
    echo "\$cookie_domain = '${COOKIE_DOMAIN}';" >> ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/settings.php
  fi
fi

exit 0
