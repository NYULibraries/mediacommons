#!/bin/bash

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  exit 1;
}

SOURCE="${BASH_SOURCE[0]}"

# resolve $SOURCE until the file is no longer a symlink
while [ -h "$SOURCE" ]; do 
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  # if $SOURCE was a relative symlink, we need to resolve it relative to the path where
  # the symlink file was located
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done

DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

DEBUG=""

while getopts ":c:hdb" opt; do
  case $opt in
    c)
      [ -f $OPTARG ] || die "Configuration file does not exist." 
      CONF_FILE=$OPTARG
      ;;
    d)
      DEBUG="-d -v"
      ;;
    h)
      echo " "
      echo " Usage: ./multisite.sh -c example.conf"
      echo " "
      echo " Options:"
      echo "   -h                Show brief help"    
      echo "   -p <directory>    Specify the directory of sites configurations (e.g., -c /path)."
      echo "   -c <file>         Specify the configuration file for the umbrella site (e.g., -c example.conf)."
      echo " "
      exit 0
      ;;
  esac
done


CONFIGURATION_UMBRELLA=mediacommons.conf

# load configuration file
. /Users/ortiz/tools/projects/mediacommons/configs/${CONFIGURATION_UMBRELLA}

UMBRELLA_BUILD_DIR=${BUILD_DIR}
UMBRELLA_BUILD_BASE_NAME=${BUILD_BASE_NAME}
UMBRELLA_BUILD=${BUILD_DIR}/${BUILD_BASE_NAME}

UMBRELLA_COOKIE_DOMAIN=${COOKIE_DOMAIN}

[ -w $UMBRELLA_BUILD/sites ] || die ${LINENO} "test" "Unable to write to ${UMBRELLA_BUILD}/sites directory." ;

# mv ${UMBRELLA_BUILD}/sites/default ${UMBRELLA_BUILD}/sites/${COOKIE_DOMAIN}.${UMBRELLA_BUILD_BASE_NAME}

CONFIGURATION_DIRECTORY=/Users/ortiz/tools/projects/mediacommons/configs

for configuration in $(find ${CONFIGURATION_DIRECTORY} -maxdepth 1 -type f \( -iname "*.conf" ! -iname "default.*.conf" ! -iname "build.conf" ! -iname "${CONFIGURATION_UMBRELLA}" \)) ; 
  do
    # load configuration file
    . ${configuration}
    [ -w $BUILD_DIR/$BUILD_BASE_NAME ] || die ${LINENO} "test" "Unable to write to ${BUILD_DIR}/${BUILD_BASE_NAME}/sites directory." ;
    if [[ -f $BUILD_DIR/$BUILD_BASE_NAME/index.php ]]; then
      # check if this directory looks like Drupal 7
      MATCH=`grep -c 'DRUPAL_ROOT' $BUILD_DIR/$BUILD_BASE_NAME/index.php` 
      if [ $MATCH -gt 0 ]; then
        [ -w $BUILD_DIR/$BUILD_BASE_NAME/sites/default ] || die ${LINENO} "test" "Unable to write ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default" ;
        # move site directory inside umbrella
        mv ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/default ${UMBRELLA_BUILD}/sites/${COOKIE_DOMAIN}.${BUILD_BASE_NAME}
        # remove link or delete directory 
        if [ -L ${BUILD_DIR}/${BUILD_BASE_NAME} ] ; then
          # link
          rm ${BUILD_DIR}/${BUILD_BASE_NAME}
        else
          # directory
          rm -rf ${BUILD_DIR}/${BUILD_BASE_NAME}
        fi
        # link to umbrella 
        cd ${UMBRELLA_BUILD_DIR}
          ln -s ${UMBRELLA_BUILD_BASE_NAME} ${BUILD_BASE_NAME}
        cd -
      fi
  fi
done

exit 1
