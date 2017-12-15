#!/bin/bash

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  if [[ -f ${TEMP_DIR}/apachesolr.pid ]]; then
    rm -f ${TEMP_DIR}/apachesolr.pid
  fi
  exit 1
}

while getopts ":c:h" opt; do
 case $opt in
  c)
   [ -f $OPTARG ] || die "Configuration file does not exist."
   CONF_FILE=$OPTARG
   ;;
  h)
   echo " "
   echo " Usage: ./apachesolr.sh -c project.conf"
   echo " "
   echo " Options:"
   echo "   -c <file>    Specify the configuration file to use (e.g., -c project.conf)."
   echo "   -h           Show brief help"
   echo " "
   exit 0
   ;;
  esac
done

[ $CONF_FILE ] || die ${LINENO} "error" "No configuration file provided"

# load configuration file
. $CONF_FILE

# Apache Solr module requieres $base_url
echo "\$base_url = '${BASE_URL}';" >> ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/settings.php

# Set Apache Solr environment (URL) and overwrite the one in database.
echo "\$conf['apachesolr_environments']['solr']['url'] = '${MEDIACOMMONS_APACHESOLR_URL}';" >> ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/settings.php

# while working in my VM

# HOSTIP=`cat /home/base/host.ip`
# echo "\$basename='${BUILD_BASE_NAME}';" >> ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/settings.php
# echo "\$hostip='${HOSTIP}';" >> ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/settings.php
# cat ${BUILD_APP_ROOT}/bin/utilities/ip.txt >> ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/settings.php

exit 0
