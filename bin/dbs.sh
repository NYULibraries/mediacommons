#!/bin/bash

# if anything in this script fail; FAIL big! WE MUST STOP any other
# process related to autobuild.sh.

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  rm -f ${TEMP_DIR}/dbs.pid
  exit 1;
}

while getopts ":c:s" opt; do
  case $opt in
    c)
      [ -f $OPTARG ] || die "Configuration file does not exist."
      CONF_FILE=$OPTARG
      ;;
  esac
done

[ $CONF_FILE ] || die ${LINENO} "fail" "No configuration file provided."

# load configuration file
. $CONF_FILE

if [[ -f ${TEMP_DIR}/dbs.pid ]];
then
  die ${LINENO} "error" "Fail: Process already running."
fi

echo $$ > ${TEMP_DIR}/dbs.pid

rm ${DBS}/*.sql

scp -r ${USER}@${DEVHOST}:${DEVDBSPATH} ${DBS}

if [ $? -eq 0 ];
  then
    echo "DONE"
  else
    echo ${LINENO} "error" "Fail: Unable to download databases"
fi;

if [[ -f ${TEMP_DIR}/dbs.pid ]];
  then
    rm -f ${TEMP_DIR}/dbs.pid
fi

exit 0
