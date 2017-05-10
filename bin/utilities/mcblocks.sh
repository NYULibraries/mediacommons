#!/bin/bash

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  if [[ -f ${TEMP_DIR}/mcblocks.pid ]]; then
    rm -f ${TEMP_DIR}/mcblocks.pid
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
   echo " Usage: ./mcblocks.sh -c build.conf"
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

# Check if process it's running
if [[ -f ${TEMP_DIR}/mcblocks.pid ]]; then
  read pid <${TEMP_DIR}/mcblocks.pid
  if ! kill ${pid} > /dev/null 2>&1; then
    rm -f ${TEMP_DIR}/mcblocks.pid
  else
    die ${LINENO} "error" "Process ${pid} still running."
  fi
fi

echo $$ > ${TEMP_DIR}/mcblocks.pid

if [ -z ${DRUSH+x} ]; then die ${LINENO} "test" "Fail: Drush is not set"; fi ;

# Mark all the documents in the site
${DRUSH} -d -y mc-init-fieldguide --root=${BUILD_APP_ROOT}/builds/mediacommons

projects=(${PROJECTS})

for project in ${projects[*]}
  do
    ${DRUSH} -d -y mc-init-fieldguide --root=${BUILD_APP_ROOT}/builds/${project}
done

rm -f ${TEMP_DIR}/mcblocks.pid

exit 0

