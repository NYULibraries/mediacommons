#!/bin/bash

# load configuration file
. ./configs/tne.conf

cd ${BUILD_DIR}/${BUILD_BASE_NAME}/sites
ln -s default mediacommons.futureofthebook.org.tne

cd ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/files

ln -s ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/files images

cd -

exit 0
