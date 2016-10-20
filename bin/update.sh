#!/bin/bash

echo ${0}

die () {
  echo "file: ${0} | line: ${1} | step: ${2} | message: ${3}";
  rm -f ${TEMP_DIR}/autobuild.pid
  exit 1;
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

MAKE_FILE=${DIR}/../mediacommons.make

MAKE_FILE_DEV=${DIR}/../mediacommons.develop.make

[ -f ${MAKE_FILE} ] || die ${LINENO} "test" "Unable to find ${MAKE_FILE}"

[ -f ${MAKE_FILE_DEV} ] || die ${LINENO} "test" "Unable to find ${MAKE_FILE_DEV}"

rm -f ${MAKE_FILE}

wget https://raw.githubusercontent.com/NYULibraries/mediacommons/master/mediacommons.make -P ${DIR}/../

chmod 2777 ${DIR}/../mediacommons.make

rm -f ${MAKE_FILE_DEV}

wget https://raw.githubusercontent.com/NYULibraries/mediacommons/master/mediacommons.develop.make -P ${DIR}/../

chmod 2777 ${DIR}/../mediacommons.develop.make

exit 0
