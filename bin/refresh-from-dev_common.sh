# This string tells the expect script wrapper that refresh run has completed.
SCRIPT_RUN_COMPLETE='Mediacommons refresh run completed.'
# Used by select_sites() to report what sites have been selected, and by `expect`
# script to know when to return from `interact`
EXPECT_SIGNAL_SELECT_SITES_COMPLETED="EXPECT: NUMBER OF SITES SELECTED = "

BASTION_HOST=b.dlib.nyu.edu

DEV_SERVER=devmc2.dlib.nyu.edu
DEV_SERVER_MC=/www/sites/mediacommons
DEV_SERVER_BUILDS=${DEV_SERVER_MC}/builds
DEV_SERVER_CONFIGS=${DEV_SERVER_MC}/configs
DEV_SERVER_DATABASE_DUMPS=${DEV_SERVER_MC}/lib/dumps
DEV_SERVER_EXPORT_DB_SCRIPT=${DEV_SERVER_MC}/bin/utilities/export_db.sh
DEV_SERVER_FILES=${DEV_SERVER_MC}/lib/files

# Directory where storing local copies of devmc database dumps
DATABASE_DUMPS=

# Directory where storing local copies of devmc files/
MC_FILES=

# Username on bastion host and dev server
NETWORK_HOST_USERNAME=

# Running using `expect` script?
EXPECT_MODE=false

# Mediacommons main site and channel sites
declare -a ALL_SITES=( alt-ac fieldguide imr intransition mediacommons tne )

# Sites that user would like to refresh.  Default to all of them.  If user runs
# the `expect` refresh script, there will be an option to select which sites
# to refresh.
declare -a selected_sites=("${ALL_SITES[@]}")
# Sites that have not yet been selected to display in the selection menu.
declare -a remaining_sites_menu_options=("${ALL_SITES[@]}")

# We need a SHA checksum program for password creation.
# Mac OS X and Centos 6 and 7 all appear to have one installed by default.
SHASUM=$(which sha256sum)

if [ -z $SHASUM ]
then
    SHASUM=$(which shasum)
fi

if [ -z $SHASUM ]
then
    echo >&2 'You must have either `sha256sum` or `shasum` in your PATH.'
    exit 1
fi

function validate_args() {

    if [ ! -d "${DATABASE_DUMPS}" ]
    then
        echo >&2 "DATABASE_DUMPS '${DATABASE_DUMPS}' is not a directory."
        usage
        exit 1
    fi

    if [ ! -d "${MC_FILES}" ]
    then
        echo >&2 "MC_FILES '${MC_FILES}' is not a directory."
        usage
        exit 1
    fi

    if [ -z "${NETWORK_HOST_USERNAME}" ]
    then
        echo >&2 "You must provide NETWORK_HOST_USERNAME."
        usage
        exit 1
    fi
}

function get_menu_options_list() {
    local options=$1

    echo "Done ${options} All"
}

function select_sites() {
    # Clear selected sites list
    selected_sites=()
    # This initialization should have already been done before, but do it again
    # anyway just in case.
    remaining_sites_menu_options=("${ALL_SITES[@]}")

    options_list=$(get_menu_options_list "${remaining_sites_menu_options[*]}")
    until [ "${choice}" == "All" ] || [ "${choice}" == "Done" ]; do
        select choice in $options_list; do
            if [ $choice == "All" ]; then
                selected_sites=("${ALL_SITES[@]}")
                break
            elif [ $choice == "Done" ]; then
                # Exit if user didn't select any sites.
                if [ ${#selected_sites[@]} -eq 0 ]; then
                    echo >&2 'You have not selected any sites to refresh.'gd
                    unset choice
                fi

                break
            else
                selected_sites+=($choice)

                # Adjust menu
                array_index_of_site=$((REPLY-2))
                unset remaining_sites_menu_options[${array_index_of_site}]
                # Re-index array
                remaining_sites_menu_options=( "${remaining_sites_menu_options[@]}" )
                options_list=$(get_menu_options_list "${remaining_sites_menu_options[*]}")

                break
            fi
        done
    done

    echo "Selected sites to refresh: ${selected_sites[*]}"

    if [ "${EXPECT_MODE}" == "true" ]; then
        num_selected_sites=${#selected_sites[@]}

        echo "${EXPECT_SIGNAL_SELECT_SITES_COMPLETED}${num_selected_sites}"
    fi
}
