#!/bin/bash

# Don't use me! ... well, do it if you know what you are doing.
#
# In theory I should work; but in practice I'm almost certain that I will
# fail if you run me in a machine that does not looks like the one
# I'm intended to run it. So, don't use me if you are not sure, I can
# easily get you into trouble.

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  rm -f ${TEMP_DIR}/autobuild.pid
  exit 1;
}

ENVIRONMENT="development"

MAKE_FILE=`pwd`"/mediacommons.make"

while getopts ":m:c:e:h" opt; do
 case $opt in
  c)
    [ -f $OPTARG ] || die "Configuration file does not exist."
    CONF_FILE=$OPTARG
    ;;
  u)
    UPDATE=true
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
   echo " "
   exit 0
   ;;
  esac
done

[ $CONF_FILE ] || die ${LINENO} "test" "No configuration file provided."

# load configuration file
. $CONF_FILE

# Here I need to test if the autobuild is running and kill this process
# or remove the pid file and keep going
if [[ -f ${TEMP_DIR}/autobuild.pid ]]; then
  rm -f ${TEMP_DIR}/autobuild.pid;
fi

echo $$ > ${TEMP_DIR}/autobuild.pid

# Get the latest make file and do any other task before running jobs
${BUILD_APP_ROOT}/bin/update.sh

# Do some house cleaning before running job
# ${BUILD_APP_ROOT}/bin/maintenances.sh -c ${BUILD_APP_ROOT}/configs/build.conf;

# Build and migrate Umbrella before anything else
${BUILD_APP_ROOT}/bin/umbrella.sh -c ${BUILD_APP_ROOT}/configs/build.conf -m ${MAKE_FILE};

projects=(${PROJECTS})

for project in ${projects[*]}
  do
    # -l run in legacy mode
    # -e environment we are building
    # -k use cookies to share databases
    ${BUILD_APP_ROOT}/bin/build.sh -c ${BUILD_APP_ROOT}/configs/${project}.conf -m ${MAKE_FILE} -l -e ${ENVIRONMENT} -k;
    if [ $? -eq 0 ];
      then
        echo "Successful: Build ${project}";
        # Run preprocess task
        echo "Run preprocess task";
        ${BUILD_APP_ROOT}/bin/utilities/preprocess.sh -c ${BUILD_APP_ROOT}/configs/${project}.conf;
        # Migrate the content
        echo "Migrate the content";
        ${BUILD_APP_ROOT}/bin/migrate.sh -c ${BUILD_APP_ROOT}/configs/${project}.conf;
        # Step: Export database
        echo "Export database";
        ${BUILD_APP_ROOT}/bin/utilities/export_db.sh -c ${BUILD_APP_ROOT}/configs/${project}.conf;
        echo "Set-up and clean-up others";
        ${BUILD_APP_ROOT}/bin/utilities/postprocess.sh -c ${BUILD_APP_ROOT}/configs/${project}.conf;
      else
        echo ${LINENO} "build" "Fail: Build ${project}";
    fi;
done;

if [[ -f ${TEMP_DIR}/autobuild.pid ]]; then
  rm -f ${TEMP_DIR}/autobuild.pid;
fi

exit 0;
