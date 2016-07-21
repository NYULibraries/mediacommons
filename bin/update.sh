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

[ -f ${MAKE_FILE} ] || die ${LINENO} "test" "Unable to find ${MAKE_FILE}"

# make sure the most recent changes to *.make file are in place
rm -f ${MAKE_FILE}

wget https://raw.githubusercontent.com/NYULibraries/mediacommons/master/mediacommons.make -P ${DIR}/../

chmod 2777 ${DIR}/../mediacommons.make

exit 0
