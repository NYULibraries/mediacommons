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

    # Call from expect script, which automates the interactions with rsync.
    ./${script_name} -d ~/mediacommons_databases -f ~/mediacommons_files -e
EOF
}

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

echo $DATABASE_DUMPS
echo $MC_FILES
echo $DEV_SERVER_USERNAME
echo $EXPECT_MODE

exit