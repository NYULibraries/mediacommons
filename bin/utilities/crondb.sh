#!/bin/bash

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  if [[ -f ${TEMP_DIR}/cron.pid ]]; then
    rm -f ${TEMP_DIR}/crondb.pid
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
   echo " Usage: ./crondb.sh -c build.conf"
   echo " "
   echo " Options:"
   echo "   -c <file>    Specify the configuration file to use (e.g., -c project.conf)."
   echo "   -h           Show brief help"
   echo " "
   exit 0
   ;;
  esac
done

echo $$ > ${TEMP_DIR}/crondb.pid

[ $CONF_FILE ] || die ${LINENO} "error" "No configuration file provided"

# load configuration file
. $CONF_FILE

${DRUSH} cron --root=${BUILD_APP_ROOT}/builds/mediacommons
${DRUSH} cc all --root=${BUILD_APP_ROOT}/builds/mediacommons
${DRUSH} cc all --root=${BUILD_APP_ROOT}/builds/mediacommons

projects=(${PROJECTS})

for project in ${projects[*]}
  do
    ${DRUSH} cron --root=${BUILD_APP_ROOT}/builds/${project}
    ${DRUSH} cc all --root=${BUILD_APP_ROOT}/builds/${project}
    ${BUILD_APP_ROOT}/bin/utilities/export_db.sh -c ${project}.conf
done

rm -f ${TEMP_DIR}/crondb.pid

exit 0
