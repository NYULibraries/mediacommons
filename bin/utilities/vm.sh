#!/bin/bash

tell () {
  echo "file: ${0} | line: ${LINENO} | echo: ${1}"
}

echo $$ > /tmp/init.pid

git clone https://github.com/NYULibraries/mediacommons.git

cd mediacommons/lib

mkdir databases

mkdir files

cd modules

git clone https://github.com/NYULibraries/mediacommons_modules.git

cd mediacommons_modules

git checkout develop

cd ../..

cd profiles

git clone https://github.com/NYULibraries/mediacommons_umbrella.git

cd mediacommons_umbrella

git checkout develop

cd ..

git clone https://github.com/NYULibraries/mediacommons_projects.git

cd mediacommons_projects

git checkout develop

cd ../..

cd themes

git clone https://github.com/NYULibraries/mediacommons_admin.git

cd mediacommons_admin

git checkout develop

cd ..

git clone https://github.com/NYULibraries/mediacommons_theme.git

cd mediacommons_theme

git checkout develop

cd ../..

exit 0
