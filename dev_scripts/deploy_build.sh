#!/bin/bash

die () {
  echo $1
  exit 1
}

[ "$#" -eq 1 ] || die "Please provide make file"

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

LIBRARY="$(dirname "$DIR")"/lib

. $DIR/../project.conf

[ -f $1 ] || die "File $1 does not exist"
[ -d $LIBRARY ] || die "Library directory $LIBRARY does not exist"
[ -d $BUILD_DIR ] || die "Build directory $BUILD_DIR does not exist" 

echo Preparing new site using $1
drush make --prepare-install $1 $BUILD_DIR/$BUILD_NAME
  
echo Install new site
cd $BUILD_DIR/$BUILD_NAME
drush site-install $DRUPAL_INSTALL_PROFILE_NAME --site-name=$DRUPAL_SITE_NAME --account-name=$DRUPAL_ACCOUNT_NAME --account-mail=$DRUPAL_ACCOUNT_MAIL --site-mail=$DRUPAL_SITE_MAIL --db-url=$DRUPAL_SITE_DB_TYPE://$DRUPAL_SITE_DB_USER:$DRUPAL_SITE_DB_PASS@$DRUPAL_SITE_DB_ADDRESS/$DRUPAL_DB_NAME
  
echo Linking build name $BUILD_NAME

site_dirs=(modules themes)

# find modules and themes and symlink them to the repo code
for site_dir in "${site_dirs[@]}"
  do
    for dir in $LIBRARY/${site_dir}/*
      do 
        base=${dir##*/}
        rm -rf $BUILD_DIR/$BUILD_NAME/sites/all/${site_dir}/${base}
        ln -s $LIBRARY/${site_dir}/${base} $BUILD_DIR/$BUILD_NAME/sites/all/${site_dir}/${base}
    done
done

# find profile and symlink them to the repo code
for dir in $LIBRARY/profiles/*
  do 
    base=${dir##*/}
    rm -rf $BUILD_DIR/$BUILD_NAME/profiles/${base}
    ln -s $LIBRARY/profiles/${base} $BUILD_DIR/$BUILD_NAME/profiles/${base}
done

echo Ok