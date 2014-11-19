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

LIBRARY="$(dirname "$DIR")"/lib

. $DIR/../project.conf

[ -d $LIBRARY ] || die "Library directory $LIBRARY does not exist"

[ -d $BUILD_DIR ] || die "Build directory $BUILD_DIR does not exist" 

[ -d $BUILD_DIR/$BUILD_NAME ] || die "Build directory $BUILD_DIR/$BUILD_NAME does not exist" 

cd $BUILD_DIR/$BUILD_NAME

mv robots.txt robots.txt.bk

rm *.txt

mv robots.txt.bk robots.txt

mv install.php install.php.off

cd -