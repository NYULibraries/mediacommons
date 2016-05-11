#!/bin/bash

die () {
  echo $1
  exit 1
}

BUILD_ROOT=$1

MODULES=$2

[ ! -z  $BUILD_ROOT ] || die "Please provide build derectory path"

[ -d $BUILD_ROOT ] || die "Build directory $BUILD_ROOT does not exist"

[ ! `grep -q 'DRUPAL_ROOT' $BUILD_ROOT/index.php` ] || die "$BUILD_ROOT does not look like a Drupal installation folder."

# run command
# drush set-theme-setting-class-name ${DRUPAL_SPECIAL_BODY_CLASS} --root=${BUILD_ROOT}

for i in $(echo $MODULES | sed "s/,/ /g")
  do
    drush en ${i} --root=${BUILD_ROOT} -y
done

exit 0
