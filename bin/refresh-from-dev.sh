#!/bin/bash

# Root of mediacommons repo
MEDIACOMMONS=$(cd "$(dirname "$0")" ; cd ..; pwd -P )

source $MEDIACOMMONS/bin/$(basename $0 .sh)_common.sh

DRUSH=$MEDIACOMMONS/bin/drush

function usage() {
    script_name=$(basename $0)

    cat <<EOF

usage: ${script_name} -d DATABASE_DUMPS [-e] -f MC_FILES [-u DEV_SERVER_USERNAME]
    options:
        -d DATABASE_DUMPS       Full path to local directory where database dumps
                                    will be stored.
        -e                      "expect" mode.  Indicates this script is being run from
                                    the expect wrapper script.
        -f MC_FILES             Full path to local directory where Drupal files/
                                    directories will be stored.
        -u DEV_SERVER_USERNAME  Optional username to use when logging into the dev
                                    server.  Defaults to \$(whoami).

examples:

    # Specify user name on dev server
    ./${script_name} -d ~/mediacommons_databases -f ~/mediacommons_files -u somebody

    # Use user name on local machine as login name on dev server
    ./${script_name} -d ~/mediacommons_databases -f ~/mediacommons_files

    # Call from expect script only, for automating the interactions with rsync.
    # Should never run this directly on the command line.
    ./${script_name} -d ~/mediacommons_databases -f ~/mediacommons_files -e

EOF
}

function copy_drupal_code() {
    cd $MEDIACOMMONS

    for site in "${selected_sites[@]}"; do
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
    
    for site in "${selected_sites[@]}"
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
    for site in "${selected_sites[@]}"
    do
        rsync -azvh --delete ${DEV_SERVER_USERNAME}@${DEV_SERVER}:${DEV_SERVER_FILES}/${site} ${MC_FILES}/${site}
    done
}

function copy_database_dumps() {
    # To keep things simple, copy all the database dumps regardless of which sites
    # were selected for refresh.  Faster, simpler.
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

    for site in "${selected_sites[@]}"; do
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

    for site in "${selected_sites[@]}"; do
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

while getopts d:ef:u: opt
do
    case $opt in
        d) DATABASE_DUMPS=$OPTARG ;;
        e) EXPECT_MODE=true ;;
        f) MC_FILES=$OPTARG ;;
        u) DEV_SERVER_USERNAME=$OPTARG ;;
        *) echo >&2 "Options not set correctly."; usage; exit 1 ;;
    esac
done

if [ -z $DEV_SERVER_USERNAME ]; then
    DEV_SERVER_USERNAME=$(whoami)
fi

validate_args

select_sites

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