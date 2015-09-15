#!/bin/bash

# load configuration file
. ./configs/alt-ac.conf

cd ${BUILD_DIR}/${BUILD_BASE_NAME}/sites
ln -s default mediacommons.futureofthebook.org.alt-ac

cd ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/files

ln -s ${BUILD_DIR}/${BUILD_BASE_NAME}/sites/files images

cd -

exit 0
