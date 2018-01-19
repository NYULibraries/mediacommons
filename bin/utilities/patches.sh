#!/bin/bash

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  if [[ -f ${TEMP_DIR}/patches.pid ]]; then
    rm -f ${TEMP_DIR}/patches.pid
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
   echo " Usage: ./patches.sh -c build.conf"
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

cd ${BUILD_APP_ROOT}/builds/mediacommons/sites/all/modules/apachesolr
patch -p1 < ${BUILD_APP_ROOT}/lib/patches/description-wrong-number-for-start.patch

projects=(${PROJECTS})

for project in ${projects[*]}
  do
    cd ${BUILD_APP_ROOT}/builds/${project}/sites/all/modules/apachesolr
    patch -p1 < ${BUILD_APP_ROOT}/lib/patches/description-wrong-number-for-start.patch
done

exit 0
