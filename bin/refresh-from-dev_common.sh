# This string tells the expect script wrapper that refresh run has completed.
SCRIPT_RUN_COMPLETE='Mediacommons refresh run completed.'
# Used by select_sites() to report what sites have been selected, and by `expect`
# script to know when to return from `interact`
EXPECT_SIGNAL_SELECT_SITES_COMPLETED="EXPECT: NUMBER OF SITES SELECTED = "

DEV_SERVER=devmc.dlib.nyu.edu
DEV_SERVER_MC_BUILDS=/www/sites/drupal/scripts/mediacommons/builds
DEV_SERVER_DATABASE_DUMPS=/www/sites/drupal/scripts/mediacommons/builds/dbs/7
DEV_SERVER_FILES=/content/dev/pa/drupal/mediacommons/7

# Directory where storing local copies of devmc database dumps
DATABASE_DUMPS=

# Directory where storing local copies of devmc files/
MC_FILES=

# Username on dev server
DEV_SERVER_USERNAME=

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

    if [ -z "${DEV_SERVER_USERNAME}" ]
    then
        echo >&2 "You must provide DEV_SERVER_USERNAME."
        usage
        exit 1
    fi
}

function select_sites() {
    # Clear selected sites list
    selected_sites=()
    # This initialization should have already been done before, but do it again
    # anyway just in case.
    remaining_sites_menu_options=("${ALL_SITES[@]}")

    options_list="Done ${remaining_sites_menu_options[@]} All"
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
                options_list="Done ${remaining_sites_menu_options[@]} All"

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
