#!/bin/bash

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  if [[ -f ${TEMP_DIR}/solr.pid ]]; then
    rm -f ${TEMP_DIR}/solr.pid
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
   echo " Usage: ./solr.sh -c build.conf"
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

# Delete Apache Solr index
echo "Reset Apache Solr index"
curl "${MEDIACOMMONS_APACHESOLR_URL}/update?stream.body=<delete><query>*:*</query></delete>&commit=true&wt=json"

# Mark all the documents in the site
${DRUSH} -d -y solr-mark-all --root=${BUILD_APP_ROOT}/builds/mediacommons

# Run index off all documents
${DRUSH} -d -y solr-index --root=${BUILD_APP_ROOT}/builds/mediacommons

projects=(${PROJECTS})

for project in ${projects[*]}
  do
  echo ${project}

  # Mark all the documents in the site
  ${DRUSH} -d -y solr-mark-all --root=${BUILD_APP_ROOT}/builds/${project}

  # Run index off all documents
  ${DRUSH} -d -y solr-index --root=${BUILD_APP_ROOT}/builds/${project}

done

for project in ${projects[*]}
  do
  echo ${project}

  # Mark all the documents in the site
  ${DRUSH} -d -y solr-metadata --root=${BUILD_APP_ROOT}/builds/${project}

done

exit 0
