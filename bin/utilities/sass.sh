#!/bin/bash

echo ${0}

while getopts ":c:h" opt; do
 case $opt in
  c)
   [ -f $OPTARG ] || die "Configuration file does not exist."
   CONF_FILE=$OPTARG
   ;;
  h)
   echo " "
   echo " Usage: ./check_build.sh -c example.conf"
   echo " "
   echo " Options:"
   echo "   -c <file>    Specify the configuration file to use (e.g., -c example.conf)."
   echo "   -h           Show brief help"
   echo " "
   exit 0
   ;;
  esac
done

[ $CONF_FILE ] || die ${LINENO} "error" "No configuration file provided"

# load configuration file
. $CONF_FILE

THEMES=${BUILD_DIR}/${BUILD_BASE_NAME}/sites/all/themes

for theme in `find $THEMES -maxdepth 1 -type d`
  do
    if [ -f $theme/config.rb ]
      then
        cd $theme
          bundle install --path vendor/bundle
          compass compile --force .
          rm -rf vendor
        cd -
    fi
done

exit 0
