#!/bin/bash

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  rm -f ${TEMP_DIR}/autobuild.pid
  exit 1;
}

containsElement () {
  local n=$#
  local value=${!n}
  for ((i=1;i < $#;i++)) {
    if [ "${!i}" == "${value}" ]; then
      echo "y"
      return 0
     fi
  }
  echo "n"
  return 1
}

# remove if a symlink in the same directory does not point to it
remove_if_symlink_does_not_link () {
  declare -a LINKS=();
  BASE=`pwd`
  # before going crazy, make sure the given path looks like a Drupal install directory
  MATCH=`grep -c 'DRUPAL_ROOT' $1/index.php`
  if [ $MATCH -gt 0 ]
    then
      # find link dir
      for BUILD_NAME in `ls -1 | xargs -l readlink`
        do
          LINKS=("${LINKS[@]}" "$BASE/$BUILD_NAME")
        done
        if [ $(containsElement "${LINKS[@]}" "$1") != "y" ]; then
          if  [[ -O $1 ]]; 
            then
              echo "Removing directory ${1}"
              rm -rf ${1}
          else 
            echo "Not owner. Will not try to remove directory ${1}"          
          fi
        fi
  fi
}

export -f remove_if_symlink_does_not_link

export -f containsElement

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

while getopts ":c:e:hm" opt; do
 case $opt in
  c)
    [ -f $OPTARG ] || die "Configuration file does not exist."
    CONF_FILE=$OPTARG
    ;;
  h)
   echo " "
   echo " Usage: ./maintenances.sh"
   echo " "
   echo " Options:"
   echo "   -h           Show brief help"
   echo "   -c           Configuration file"
   echo " "
   exit 0
   ;;
  esac
done

[ $CONF_FILE ] || die ${LINENO} "test" "No configuration file provided."

# load configuration file
. $CONF_FILE
 
[ -d $ROOT/builds ] || die ${LINENO} "test" "Builds directory ${ROOT}/builds does not exist"

cd ${ROOT}/builds

# Find and remove 1 days old directories
find ${ROOT}/builds/* -maxdepth 0 -type d -mtime +1 -exec bash -c 'remove_if_symlink_does_not_link $0' {} \;

cd -

exit 0
