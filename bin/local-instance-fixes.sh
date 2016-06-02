#!/bin/bash

BASEDIR=$(cd "$(dirname "$0")" ; cd ..; pwd -P )
DRUSH=$BASEDIR/bin/drush

NEW_HOSTNAME=$1

function usage() {
    script_name=$(basename $0)

    cat <<EOF

usage: ${script_name} NEW_HOSTNAME

examples:

    ./${script_name} localhost
    
    # Alias for localhost in /etc/hosts
    ./${script_name} mediacommons
   
    # If using this script to fix up a stage instance
    ./${script_name} www-stage.media-commons.org

EOF
}

function echo_and_execute() {
    local cmd="$1"

    echo $cmd
    eval $cmd
}

function update_menu_link() {
    local sql=$1

    echo_and_execute "drush sql-query \"${sql}\""
}

function update_menu_links() {
    local site_name=$1

    echo "Updating menu links for ${site_name}"

    site_dir=$BASEDIR/builds/$site_name

    echo_and_execute "cd ${site_dir}"

    if [ $? -eq 0 ]
    then
        update_menu_link "UPDATE menu_links SET link_path = 'http://${NEW_HOSTNAME}/mediacommnons' WHERE menu_name = 'menu-mcglobalnav' AND link_title = 'Front Page'"
        update_menu_link "UPDATE menu_links SET link_path = 'http://${NEW_HOSTNAME}/fieldguide' WHERE menu_name = 'menu-mcglobalnav' AND link_title = 'Field Guide'"
        update_menu_link "UPDATE menu_links SET link_path = 'http://${NEW_HOSTNAME}/alt-ac' WHERE menu_name = 'menu-mcglobalnav' AND link_title = '#Alt-Academy'"
        update_menu_link "UPDATE menu_links SET link_path = 'http://${NEW_HOSTNAME}/imr' WHERE menu_name = 'menu-mcglobalnav' AND link_title = 'In Media Res'"
        update_menu_link "UPDATE menu_links SET link_path = 'http://${NEW_HOSTNAME}/intransition' WHERE menu_name = 'menu-mcglobalnav' AND link_title = '[in]Transition'"
        update_menu_link "UPDATE menu_links SET link_path = 'http://${NEW_HOSTNAME}/tne' WHERE menu_name = 'menu-mcglobalnav' AND link_title = 'The New Everyday'"

        echo_and_execute "${DRUSH} cc all"
    else
        echo >&2 "Skipping ${site_dir}."
    fi

    echo
}

function fix_globalnav_menus() {
    for site in {alt-ac,mediacommons,fieldguide,imr,intransition,tne}
    do
        update_menu_links $site
    done
}

function fix_tne_node_135() {
    echo_and_execute "cd $BASEDIR/builds/tne"
    
    echo_and_execute "${DRUSH} sql-query --file=$BASEDIR/bin/fix-bad-html-in-tne-node-135.sql"
}

if [ -z $NEW_HOSTNAME ]
then
    echo -e >&2 "\nMissing required NEW_HOSTNAME"
    usage
    exit 1
fi

cd $BASEDIR

# mediacommons_globalnav module is hardcoding hostname in link URLs.  Need to
# change them to point to local hostname.
fix_globalnav_menus

# Fixes https://jira.nyu.edu/browse/MC-183
fix_tne_node_135

