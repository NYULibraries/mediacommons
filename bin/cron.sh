#!/bin/bash

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

TODAY=`date +%Y%m%d`

DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

DEBUG=""

while getopts ":c:htd" opt; do
 case $opt in
  c)
   [ -f $OPTARG ] || die "Configuration file does not exist."
   CONF_FILE=$OPTARG
   ;;
  d)
    DEBUG=true
    ;;
  t)
    SIMULATE=true
    ;;
  h)
   echo " "
   echo " Usage: ./cron.sh -c example.conf"
   echo " "
   echo " Options:"
   echo "   -c <file>    Specify the configuration file to use (e.g., -c example.conf)."
   echo "   -h           Show brief help"
   echo "   -t           Tell all relevant actions (don't actually change the system)."
   echo " "
   exit 0
   ;;
  esac
done

[ $CONF_FILE ] || die ${LINENO} 1 "No configuration file provided"

# Step # 1
# load configuration file
. $CONF_FILE

# remove old databases
echo "Remove old databases"
rm ${DATABASES_DIR}/*.sql

# Export production databases
echo "Exporting IMR database"
mysqldump imr_prod > ${DATABASES_DIR}/imr.sql
echo "Exporting Intransition database"
mysqldump intransition_prod > ${DATABASES_DIR}/intransition.sql
echo "Exporting Mediacommons database"
mysqldump mc_prod > ${DATABASES_DIR}/mediacommons.sql
echo "Exporting TNE database"
mysqldump tne_prod > ${DATABASES_DIR}/tne.sql
echo "Exporting Shared database"
mysqldump mcshared_prod > ${DATABASES_DIR}/mcshared.sql
echo "Exporting ALT-AC database"
mysqldump altac_prod > ${DATABASES_DIR}/alt-ac.sql

# Copy sites files
rsync -vrh ${FILES_SOURCE} ${FILES_DESTINATION}
