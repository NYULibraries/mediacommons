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

# Copy sites files
if [ -d "$BACKUP_FILES_DIR" ]; then
  # rsync -vrh --exclude '.htaccess' ${BACKUP_FILES_DIR}/ ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/files
  # rsync -vrh --exclude '.htaccess' ${BACKUP_FILES_DIR}/ ${FILES_DIR}
fi

# make sure there is a link to the default folder using our Drupal 6 convention
ln -s ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/mediacommons.futureofthebook.org.${BUILD_BASE_NAME}

# rm -rf ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/files

# ln -s ${FILES_DIR} ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/files

# somehow there are links to the file folder inside the file folder 
# ln -s ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/files ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default/files/files

exit 0
