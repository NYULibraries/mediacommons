#!/bin/bash

echo ${0}

while getopts ":c:h" opt; do
 case $opt in
  c)
   [ -f $OPTARG ] || die "Configuration file does not exist."
   CONF_FILE=$OPTARG
   ;;
  h)
   echo " "
   echo " Usage: ./check_build.sh -c example.conf"
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

STATUS=`curl -s -o /dev/null -w "%{http_code}" $BASE_URL`

echo "Base URL ${BASE_URL}I reported status ${STATUS}"

drush status --root=${BUILD_DIR}/${BUILD_BASE_NAME} --uri=${BASE_URL} --user=1

drush features-list --root=${BUILD_DIR}/${BUILD_BASE_NAME} --uri=${BASE_URL} --user=1

exit 0
