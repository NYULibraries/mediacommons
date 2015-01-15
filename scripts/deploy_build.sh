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

CONF_FILE=$DIR/../project.conf   

MAKE_FILE=$1

while test $# -gt 0; do

  case "$1" in

    -h|--help)
      echo " "
      echo "usage: sh deploy_build.sh [options] example.make"
      echo " "
      echo "options:"
      echo "-h, --help                          show brief help"
      echo "-c, --configuration=example.conf    specify the configuration file to use"
      exit 0
    ;;

    -c|--configuration)
      CONF_FILE=$2
      MAKE_FILE=$3
      shift
    ;;

    *)
      break
    ;;

  esac
done

[ -f $CONF_FILE ] || die "Configuration file does not exist"

[ -f $MAKE_FILE ] || die "Please provide make file"

[ ! -z  $MAKE_FILE ] || die "Please provide make file"

CONF_PERM=`ls -l $CONF_FILE | awk '{print $1}'`

# make this a bit more robust
if [ "$CONF_PERM" != "-rwx------@" ] 
  then 
    echo "Please change configuration file permission to be read only by owner" 
fi

. $CONF_FILE

[ -d $BUILD_DIR ] || mkdir $BUILD_DIR

[ -d $BUILD_DIR ] || die "Build directory $BUILD_DIR does not exist"

if [ -z "$DRUPAL_ACCOUNT_PASS" -a "$DRUPAL_ACCOUNT_PASS"==" " ]; then
  DRUPAL_ACCOUNT_PASS=`drush php-eval 'print MD5(microtime());'`
fi

echo Preparing new site using $MAKE_FILE

drush -d -v make --prepare-install -y $MAKE_FILE $BUILD_DIR/$BUILD_NAME

[ -d $BUILD_DIR/$BUILD_NAME ] || die "Unable to install new site. Build does not exist" 

# Reuse code that has been linked in the lib folder
sh $DIR/link_build.sh $BUILD_DIR/$BUILD_NAME

echo Install new site

cd $BUILD_DIR/$BUILD_NAME

drush -d -v -y site-install $DRUPAL_INSTALL_PROFILE_NAME --site-name="$DRUPAL_SITE_NAME" --account-pass="$DRUPAL_ACCOUNT_PASS" --account-name=$DRUPAL_ACCOUNT_NAME --account-mail=$DRUPAL_ACCOUNT_MAIL --site-mail=$DRUPAL_SITE_MAIL --db-url=$DRUPAL_SITE_DB_TYPE://$DRUPAL_SITE_DB_USER:$DRUPAL_SITE_DB_PASS@$DRUPAL_SITE_DB_ADDRESS/$DRUPAL_DB_NAME

# remove text files and rename install.php to install.php.off
sh $DIR/cleanup.sh $BUILD_DIR/$BUILD_NAME

chmod 777 $BUILD_DIR/$BUILD_NAME/sites/default

chmod 777 $BUILD_DIR/$BUILD_NAME/sites/default/settings.php

if [ -d $BUILD_DIR/$BUILD_NAME/sites/all/themes/mediacommons ]
  then 
  
   # for now; dev is broken; can't compile
   echo Unable to compile CSS at this moment

   # cd $BUILD_DIR/$BUILD_NAME/sites/all/themes/mediacommons
    
   # bundle install --path vendor/bundle    
    
   # compass compile --force .

fi

# link to the lastes build
# need to add a check if is file/dir or link

cd $BUILD_DIR

rm $BUILD_DIR/$BUILD_BASE_NAME

ln -s $BUILD_NAME $BUILD_BASE_NAME

cd -

# Test if site is up and running

STATUS=`curl -s -o /dev/null -w "%{http_code}" $BASE_URL/`

if [ "$STATUS" != "404" ] 
  then 
    echo Build done
    echo Build path $BUILD_DIR/$BUILD_NAME
    echo Base URL $BASE_URL Status report $STATUS

    drush status --root=$BUILD_DIR/$BUILD_NAME --uri=$BASE_URL/ --user=1
    drush features-list --root=$BUILD_DIR/$BUILD_NAME --uri=$BASE_URL/ --user=1

    if [ "$STATUS" == "200" ] 
      then 
        echo Log-in to build with URL: 
        drush uli --root=$BUILD_DIR/$BUILD_NAME --uri=$BASE_URL/
    fi
    
fi

sh $DIR/setup_migration_dbs.sh $BUILD_DIR/$BUILD_NAME $BUILD_BASE_NAME
