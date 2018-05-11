#!/bin/bash

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}"
  if [[ -f ${TEMP_DIR}/autobuild.pid ]]; then
    rm -f ${TEMP_DIR}/autobuild.pid
  fi
  exit 1
}

tell () {
  echo "file: ${0} | line: ${1} | step: ${2} | command: ${3}"
}

ENVIRONMENT="development"

SKIP=""

UPDATE=false

MAKE_FILE=`pwd`"/mediacommons.make"

while getopts ":m:c:e:hsu" opt; do
 case $opt in
  c)
    [ -f $OPTARG ] || die "Configuration file does not exist."
    CONF_FILE=$OPTARG
    ;;
  u)
    UPDATE=true
    ;;
  s)
    SKIP="-s"
    ;;
  m)
    [ -f $OPTARG ] || die "Make file does not exist."
    MAKE_FILE=$OPTARG
    ;;
  e)
    ENVIRONMENT=$OPTARG
    ;;
  h)
   echo " "
   echo " Usage: ./autobuild.sh"
   echo " "
   echo " Options:"
   echo "   -h           Show brief help"
   echo "   -m           Run some house cleaning before running job."
   echo "   -s           Do not import Drupal 6 databases"
   echo "   -u           Update Make file from master branch in Github"
   echo " "
   exit 0
   ;;
  esac
done

[ $CONF_FILE ] || die ${LINENO} "test" "No configuration file provided."

# load configuration file
. $CONF_FILE

if [ -z ${DRUSH+x} ]; then die ${LINENO} "test" "Fail: Drush is not set"; fi;

# Check if process it's running
if [[ -f ${TEMP_DIR}/autobuild.pid ]]; then
  read pid <${TEMP_DIR}/autobuild.pid
  if ! kill ${pid} > /dev/null 2>&1; then
    rm -f ${TEMP_DIR}/autobuild.pid
  else
    die ${LINENO} "error" "Process ${pid} still running."
  fi
fi

echo $$ > ${TEMP_DIR}/autobuild.pid

if [ "$UPDATE" = true ];
  then
    # Get the latest make file and do any other task before running jobs
    ${BUILD_APP_ROOT}/bin/update.sh
fi

# Build and migrate Umbrella before anything else
${BUILD_APP_ROOT}/bin/umbrella.sh -c ${BUILD_APP_ROOT}/configs/build.conf -m ${MAKE_FILE} ${SKIP}

projects=(${PROJECTS})

for project in ${projects[*]}
  do
    # -l run in legacy mode
    # -e environment we are building
    # -k use cookies to share databases
    ${BUILD_APP_ROOT}/bin/build.sh -c ${BUILD_APP_ROOT}/configs/${project}.conf -m ${MAKE_FILE} -e ${ENVIRONMENT} -k -bg ${BUILD_APP_ROOT}/configs/build.conf
    if [ $? -eq 0 ];
      then
        echo "Successful: Build ${project}"
        # Run preprocess task
        echo "Run preprocess task"
        ${BUILD_APP_ROOT}/bin/utilities/preprocess.sh -c ${BUILD_APP_ROOT}/configs/${project}.conf
        # Migrate the content
        echo "Migrate the content"
        ${BUILD_APP_ROOT}/bin/migrate.sh -c ${BUILD_APP_ROOT}/configs/${project}.conf
        echo "Set-up and clean-up others"
        ${BUILD_APP_ROOT}/bin/utilities/postprocess.sh -c ${BUILD_APP_ROOT}/configs/${project}.conf
      else
        echo ${LINENO} "build" "Fail: Build ${project}"
    fi
done

${BUILD_APP_ROOT}/bin/utilities/solr.sh -c ${BUILD_APP_ROOT}/configs/build.conf

${BUILD_APP_ROOT}/bin/utilities/cron.sh -c ${BUILD_APP_ROOT}/configs/build.conf

${BUILD_APP_ROOT}/bin/utilities/mcblocks.sh -c ${BUILD_APP_ROOT}/configs/build.conf

${BUILD_APP_ROOT}/bin/utilities/patches.sh -c ${BUILD_APP_ROOT}/configs/build.conf

if [[ -f ${TEMP_DIR}/autobuild.pid ]]; then
  rm -f ${TEMP_DIR}/autobuild.pid
fi

exit 0
