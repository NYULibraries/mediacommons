#!/bin/bash

# i don't think we are using this anymore

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

if [ -z ${DRUSH+x} ]; then die ${LINENO} "test" "Fail: Drush is not set"; fi ;

STATUS=`curl -s -o /dev/null -w "%{http_code}" $BASE_URL`

echo "Base URL ${BASE_URL} reported status ${STATUS}"

${DRUSH} status --root=${BUILD_DIR}/${BUILD_BASE_NAME} --uri=${BASE_URL} --user=1

${DRUSH} features-list --root=${BUILD_DIR}/${BUILD_BASE_NAME} --uri=${BASE_URL} --user=1

exit 0
