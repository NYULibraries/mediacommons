# This string tells the expect script wrapper that refresh run has completed.
SCRIPT_RUN_COMPLETE='Mediacommons refresh run completed.'

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

    options_list="All ${ALL_SITES[@]} Done"
    until [ "${choice}" == "All" ] || [ "${choice}" == "Done" ]; do
        select choice in $options_list; do
            if [ $choice == "All" ]; then
                selected_sites=("${ALL_SITES[@]}")
                break
            elif [ $choice == "Done" ]; then
                break
            else
                selected_sites+=($choice)
                break
            fi
        done
    done

    echo "Selected sites to refresh: ${selected_sites[*]}"

validate_args
