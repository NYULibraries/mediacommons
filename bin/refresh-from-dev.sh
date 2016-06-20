#!/bin/bash

# Root of mediacommons repo
MEDIACOMMONS=$(cd "$(dirname "$0")" ; cd ..; pwd -P )

source $MEDIACOMMONS/bin/$(basename $0 .sh)_common.sh

DRUSH=$MEDIACOMMONS/bin/drush

DEV_SERVER=devmc.dlib.nyu.edu
DEV_SERVER_MC_BUILDS=/www/sites/drupal/scripts/mediacommons/builds
DEV_SERVER_DATABASE_DUMPS=/www/sites/drupal/scripts/mediacommons/builds/dbs/7
DEV_SERVER_FILES=/content/dev/pa/drupal/mediacommons/7

function copy_drupal_code() {
    cd $MEDIACOMMONS

    for site in {alt-ac,fieldguide,imr,intransition,mediacommons,tne}; do
        rm -fr builds/${site}
        rsync -azvh ${DEV_SERVER_USERNAME}@${DEV_SERVER}:${DEV_SERVER_MC_BUILDS}/${site}/ builds/${site}/
 
        # Turning this off because certain files have all perms turned off, which causes rsync
        # to return with non-zero status.
        # Example:
        #     rsync: send_files failed to open "/www/sites/drupal/scripts/mediacommons/builds/alt-ac/sites/default/settings.php.1466136133.off": Permission denied (13)     

        #   if [ $? -ne 0 ]
        #   then
        #       echo >&2 "rsync of ${site} failed."
        #       exit 1
        #   fi
    done
}

function fix_symlinks() {
    cd $MEDIACOMMONS/builds/
    
    for site in {alt-ac,fieldguide,imr,intransition,mediacommons,tne}
    do
        cd ${site}/profiles/
        rm README
        ln -s ../../../lib/profiles/README

        cd ../sites/
        rm mediacommons.futureofthebook.org.${site}
        ln -s default mediacommons.futureofthebook.org.${site}

        cd default/
        rm files
        ln -s $MC_FILES/${site} files

        cd ../../../
    done
}

function change_tne_database_name() {
    cd $MEDIACOMMONS/builds/
    sed -i.bak 's/d7_tne/tne/' tne/sites/default/settings.php 
}

function copy_files() {
    rsync -azvh --delete ${DEV_SERVER_USERNAME}@${DEV_SERVER}:${DEV_SERVER_FILES}/ $MC_FILES/
}

function copy_database_dumps() {
    rsync -azvh ${DEV_SERVER_USERNAME}@${DEV_SERVER}:${DEV_SERVER_DATABASE_DUMPS}/ $DATABASE_DUMPS/

    mv $DATABASE_DUMPS/alt-ac.sql $DATABASE_DUMPS/altac.sql
}

function recreate_database() {
    local database=$1

    mysql -e "DROP DATABASE ${database}"
    mysql -e "CREATE DATABASE ${database} CHARACTER SET utf8 COLLATE utf8_general_ci"
}

function import_database_dump() {
    local database=$1

    mysql $database < $DATABASE_DUMPS/${database}.sql
}

function recreate_user() {
    local database=$1
    local dbuser=$2
    local dbpassword="$3"

    mysql -e "DROP USER '${dbuser}'@'localhost'"
    mysql -e "CREATE USER '${dbuser}'@'localhost' IDENTIFIED BY '${dbpassword}';"
    
    # Used to do grants in this function:
    #
    # mysql -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, INDEX, ALTER, CREATE TEMPORARY TABLES ON ${database}.* TO '${dbuser}'@'localhost' IDENTIFIED BY '${dbpassword}'"
    #
    # ...but for some reason they never took hold, possibly due to a race condition with CREATE USER statement.
    # Grants are done at the end by do_database_grants().
}

function recreate_databases() {
    cd $MEDIACOMMONS/builds/

    for site in {alt-ac,fieldguide,imr,intransition,mediacommons,tne}; do
        cd $site

        database=$( echo $site | sed 's/-//' )
        dbuser=$( ${DRUSH} sql-connect | awk '{print $2}' | sed 's/--user=//' )
        dbpassword="$( ${DRUSH} sql-connect | awk '{print $3}' | sed 's/--password=//' )"

        recreate_database $database

        import_database_dump $database

        recreate_user $database $dbuser $dbpassword

        cd ..
    done
}

function do_database_grants() {
    cd $MEDIACOMMONS/builds/

    for site in {alt-ac,fieldguide,imr,intransition,mediacommons,tne}; do
        cd $site

        database=$( echo $site | sed 's/-//' )
        dbuser=$( ${DRUSH} sql-connect | awk '{print $2}' | sed 's/--user=//' )
        dbpassword="$( ${DRUSH} sql-connect | awk '{print $3}' | sed 's/--password=//' )"

        mysql -e "GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, INDEX, ALTER, CREATE TEMPORARY TABLES ON ${database}.* TO '${dbuser}'@'localhost' IDENTIFIED BY '${dbpassword}'"

        cd ..
    done
}

function fix_tne_wbr_problem() {
    # See https://jira.nyu.edu/browse/MC-183
    
    mysql tne < $MEDIACOMMONS/bin/fix-bad-html-in-tne-node-135.sql
}

set -x

copy_drupal_code

fix_symlinks

change_tne_database_name

copy_files

copy_database_dumps

recreate_databases

do_database_grants

fix_tne_wbr_problem

# This string tells the expect script wrapper that refresh run has completed.
echo $SCRIPT_RUN_COMPLETE

set +x