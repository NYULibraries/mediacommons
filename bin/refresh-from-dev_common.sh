# This string tells the expect script wrapper that refresh run has completed.
SCRIPT_RUN_COMPLETE='Mediacommons refresh run completed.'

DEV_SERVER=devmc.dlib.nyu.edu
DEV_SERVER_MC_BUILDS=/www/sites/drupal/scripts/mediacommons/builds
DEV_SERVER_DATABASE_DUMPS=/www/sites/drupal/scripts/mediacommons/builds/dbs/7
DEV_SERVER_FILES=/content/dev/pa/drupal/mediacommons/7

# Directory where storing local copies of devmc database dumps
DATABASE_DUMPS=$1

# Directory where storing local copies of devmc files/
MC_FILES=$2

# Username on dev server
DEV_SERVER_USERNAME=$3

# Shared database has no associated Drupal site or conf file
SHARED_DATABASE='shared'

function usage() {
    script_name=$(basename $0)

    cat <<EOF

usage: ${script_name} DATABASE_DUMPS MC_FILES DEV_SERVER_USERNAME

examples:

    ./${script_name} ~/mediacommons_databases ~/mediacommons_files somebody
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

validate_args
