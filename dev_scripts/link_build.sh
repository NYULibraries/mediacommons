#!/bin/bash

die () {
  echo $1
  exit 1
}

SOURCE="${BASH_SOURCE[0]}"

while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done

DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

LIBRARY=$DIR/../

. $DIR/../project.conf

[ -d $LIBRARY ] || die "Library directory $LIBRARY does not exist"
[ -d $BUILD_DIR ] || die "Build directory $BUILD_DIR does not exist" 

site_dirs=(modules themes)

# find modules and themes and symlink them to the repo code
for site_dir in "${site_dirs[@]}" do
  for dir in $LIBRARY/${site_dir}/* do 
    base=${dir##*/}
    rm -rf $BUILD_DIR/sites/all/${site_dir}/${base}
    ln -s $(pwd)/$LIBRARY/${site_dir}/${base} $BUILD_DIR/sites/all/${site_dir}/${base}
  done
done

# find profile and symlink them to the repo code
for dir in $LIBRARY/profiles/* do 
  base=${dir##*/}
  rm -rf $BUILD_DIR/profiles/${base}
  ln -s $(pwd)/$LIBRARY/profiles/${base} $BUILD_DIR/profiles/${base}
done

echo "ok"