#!/bin/bash
# https://askubuntu.com/questions/967623/creating-symbolic-links-within-folders-shared-with-the-windows-host-system

die () {
  echo $1
  exit 1
}

while getopts ":c:hd" opt; do
  case $opt in
    c)
      [ -f $OPTARG ] || die "Configuration file does not exist." 
      CONF_FILE=$OPTARG
      ;;
    h)
      echo " "
      echo " Usage: ./utilities/link_build.sh -c example.conf"
      echo " "
      echo " Options:"
      echo "   -h           Show brief help"
      echo "   -c <file>    Specify the configuration file to use (e.g., -c example.conf)."
      echo " "
      exit 0
      ;;
  esac
done

[ $CONF_FILE ] || die "No configuration file provided."

# load configuration file
. $CONF_FILE

LIBRARY="${BUILD_APP_ROOT}/lib"

[ -d $LIBRARY ] || die "Library directory ${LIBRARY} does not exist"

if [[ -f $BUILD_DIR/$BUILD_BASE_NAME/index.php ]]; then
  # check if this directory looks like Drupal 7
  MATCH=`grep -c 'DRUPAL_ROOT' $BUILD_DIR/$BUILD_BASE_NAME/index.php` 
  if [ $MATCH -gt 0 ]; then
    echo Linking build ${BUILD_DIR}/${BUILD_BASE_NAME}
    site_dirs=(modules themes)
    # find modules/themes and symlink them to the repo code
    for site_dir in "${site_dirs[@]}"
      do
        for dir in $LIBRARY/${site_dir}/*
          do
            base=${dir##*/}
            if [ -d ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/all/${site_dir}/${base} ] && [ -d ${LIBRARY}/${site_dir}/${base} ]
              then
                rm -rf ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/all/${site_dir}/${base}
                ln -s $LIBRARY/${site_dir}/${base} ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/all/${site_dir}/${base}
            fi  
      done
    done
    # find profiles and symlink them to the repo code
    for dir in $LIBRARY/profiles/*
      do 
        base=${dir##*/}
        rm -rf ${BUILD_DIR}/${BUILD_BASE_NAME}/profiles/${base}
        ln -s $LIBRARY/profiles/${base} ${BUILD_DIR}/${BUILD_BASE_NAME}/profiles/${base}    
    done
  fi
fi

exit 0
