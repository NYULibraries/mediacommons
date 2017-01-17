#!/bin/bash

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  if [[ -f ${TEMP_DIR}/maintenances.pid ]]; then
    rm -f ${TEMP_DIR}/maintenances.pid
  fi
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
  local filename=$(basename ${0})
  local replacement=""
  local regex="[_0-9]"
  local link=$(echo $filename | sed -e "s/$regex/$replacement/g")
  echo $filename
  echo $link
}

export -f remove_if_symlink_does_not_link

export -f containsElement

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

# Check if process it's running
if [[ -f ${TEMP_DIR}/maintenances.pid ]]; then
  read pid <${TEMP_DIR}/maintenances.pid
  if ! kill ${pid} > /dev/null 2>&1; then
    rm -f ${TEMP_DIR}/maintenances.pid
  else
    die ${LINENO} "error" "Process ${pid} still running."
  fi
fi

echo $$ > ${TEMP_DIR}/maintenances.pid

[ -d ${ROOT}/builds ] || die ${LINENO} "test" "Builds directory ${ROOT}/builds does not exist"

projects=(${PROJECTS})

for project in ${projects[*]}
  do
    # Find and remove 1 days old directories
    find ${ROOT}/builds/${project}* -maxdepth 0 -type d -mtime +1 -exec bash -c 'remove_if_symlink_does_not_link' {} \;
    if [ $? -eq 0 ];
      then
        echo "Successful: Build ${project}";
      else
        echo ${LINENO} "build" "Fail: Build ${project}";
    fi;
done;

if [[ -f ${TEMP_DIR}/maintenances.pid ]]; then
  rm -f ${TEMP_DIR}/maintenances.pid;
fi

exit 0
