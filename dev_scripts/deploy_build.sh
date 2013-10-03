#!/bin/bash

die () {
  echo $1
  exit 1
}

[ "$#" -eq 1 ] || die "Please provide make file"

SOURCE="${BASH_SOURCE[0]}"

while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done

DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

LIBRARY=$DIR/../

. $DIR/../project.conf

[ -f $1 ] || die "File $1 does not exist"
[ -d $LIBRARY ] || die "Library directory $LIBRARY does not exist"
[ -d $BUILD_DIR ] || die "Build directory $BUILD_DIR does not exist" 

echo Preparing new site using $1
  drush make --prepare-install $1 $BUILD_DIR/$BUILD_NAME
  
echo Install new site
  cd $BUILD_DIR/$BUILD_NAME
  drush site-install $DRUPAL_INSTALL_PROFILE_NAME --site-name=$DRUPAL_SITE_NAME --account-name=$DRUPAL_ACCOUNT_NAME --account-mail=$DRUPAL_ACCOUNT_MAIL --site-mail=$DRUPAL_SITE_MAIL --db-url=$DRUPAL_SITE_DB_TYPE://$DRUPAL_SITE_DB_USER:$DRUPAL_SITE_DB_PASS@$DRUPAL_SITE_DB_ADDRESS/$DRUPAL_DB_NAME
  
echo Build name $BUILD_NAME