#!/bin/bash

# This file has a dependency cron.sh
# Note: cron.sh is not part of this repo, in its own private repo for security reasons

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  exit 1;
}

# resolve $SOURCE until the file is no longer a symlink
while [ -h "$SOURCE" ]; do
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  # if $SOURCE was a relative symlink, we need to resolve it
  # relative to the path where the symlink file was located
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done

DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

while getopts ":c:h" opt; do
 case $opt in
  c)
   [ -f $OPTARG ] || die "Configuration file does not exist."
   CONF_FILE=$OPTARG
   ;;
  h)
   echo " "
   echo " Usage: ./preprocess.sh -c example.conf"
   echo " "
   echo " Options:"
   echo "   -c <file>    Specify the configuration file to use (e.g., -c example.conf)."
   echo "   -h           Show brief help"
   echo " "
   exit 0
   ;;
  esac
done

[ $CONF_FILE ] || die ${LINENO} 1 "No configuration file provided"

# Step # 1
# load configuration file
. $CONF_FILE

# make sure there is a link to the default folder using our Drupal 6 convention
ln -s ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/mediacommons.futureofthebook.org.${BUILD_BASE_NAME}

# Directory exist?
if [ ! -d ${DRUPAL_FILES_DIR} ];
  then
    # If directory does not exist check if we can create it
    # Try to create the files directory
    mkdir -p ${DRUPAL_FILES_DIR}
    if [ $? -eq 0 ];
      then
        echo "Success: Creating directory ${DRUPAL_FILES_DIR}. Link it.";
        chmod -R 2777 ${DRUPAL_FILES_DIR}
        rm -rf ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/files
        ln -s ${DRUPAL_FILES_DIR} ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/files
      else
        die ${LINENO} "build" "Fail: Creating directory ${DRUPAL_FILES_DIR}";
    fi;
else
  chmod -R 2777 ${DRUPAL_FILES_DIR}
  rm -rf ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/files
  ln -s ${DRUPAL_FILES_DIR} ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/files
fi

if [ ! -d ${DRUPAL_FILES_DIR}/pictures ];
  then
    mkdir -p ${DRUPAL_FILES_DIR}/pictures
    chmod -R 2777 ${DRUPAL_FILES_DIR}/pictures
fi

# copy users images directory
rsync -vrh --exclude '.htaccess' ${BUILD_APP_ROOT}/lib/files/mediacommons/pictures/* ${DRUPAL_FILES_DIR}/pictures

# copy sites files
if [ -d ${DRUPAL_6_FILES_DIR} ]; then
  rsync -vrh --exclude '.htaccess' ${DRUPAL_6_FILES_DIR}/* ${DRUPAL_FILES_DIR}
fi

drush vset file_temporary_path "${TEMP_DIR}" --root=${BUILD_DIR}/${BUILD_BASE_NAME} --user=1

exit 0
