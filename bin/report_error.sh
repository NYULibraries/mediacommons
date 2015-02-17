#!/bin/bash

die () {
  echo $1
  exit 1
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

TODAY=`date +%Y-%m-%d`

CONF_FILE=$DIR/../build.conf

CONF_PERM=`ls -l $CONF_FILE | awk '{print $1}'`

. $CONF_FILE

[ -d $BUILD_DIR ] || die "Build directory ${BUILD_DIR} does not exist."

cd $BUILD_DIR

# Report any problem in build
# find link dir
for BUILD_NAME in `ls -1 | xargs -l readlink`
  do
    if [ -d $BUILD_DIR/$BUILD_NAME ]
      then  
        MATCH=`grep -c 'DRUPAL_ROOT' $BUILD_DIR/$BUILD_NAME/index.php` 
        if [ $MATCH -gt 0 ]
          then
            SITE_ONLINE=`drush core-status --root=$BUILD_DIR/$BUILD_NAME --user=1`
            if [[ $SITE_ONLINE =~ "Connected" && $SITE_ONLINE =~ "Successful" ]]
              then
                echo "[${TODAY}] Successful and Connected - ${BUILD_NAME}"
              else
                drush status --root=$BUILD_DIR/$BUILD_NAME --user=1 | mail -s "[${TODAY}] Fail - ${BUILD_NAME}" $EMAIL
            fi
        else
          drush status --root=$BUILD_DIR/$BUILD_NAME --user=1 | mail -s "[${TODAY}] Fail - ${BUILD_NAME}" $EMAIL
        fi
    fi
done

exit 0
