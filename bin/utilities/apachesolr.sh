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

echo "\$conf['apachesolr_environments']['solr']['url'] = '${MEDIACOMMONS_APACHESOLR_URL}';" >> ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/settings.php

${DRUSH} en -y mediacommons_solr --root=${BUILD_DIR}/${BUILD_BASE_NAME} --uri=${BASE_URL}

if [ "${BUILD_BASE_NAME}" = "mediacommons" ]; then
  # ${DRUSH} -d -y solr-delete-index --root=${BUILD_DIR}/${BUILD_BASE_NAME} --uri=${BASE_URL}
  curl "${MEDIACOMMONS_APACHESOLR_URL}/update?stream.body=<delete><query>*:*</query></delete>&commit=true"
  sleep 2m
fi

${DRUSH} -d -y solr-mark-all --root=${BUILD_DIR}/${BUILD_BASE_NAME} --uri=${BASE_URL}

${DRUSH} -d -y solr-index --root=${BUILD_DIR}/${BUILD_BASE_NAME} --uri=${BASE_URL}

exit 0
