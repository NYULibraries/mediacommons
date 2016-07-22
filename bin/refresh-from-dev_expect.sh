#!/bin/bash

# Root of mediacommons repo
MEDIACOMMONS=$(cd "$(dirname "$0")" ; cd ..; pwd -P )

REFRESH_SCRIPT=${MEDIACOMMONS}/bin/$(basename $0 _expect.sh).sh

source $MEDIACOMMONS/bin/$(basename $REFRESH_SCRIPT .sh)_common.sh

function usage() {
    script_name=$(basename $0)

    cat <<EOF

usage: ${script_name} -d DATABASE_DUMPS -f MC_FILES [-u DEV_SERVER_USERNAME]
    options:
        -d DATABASE_DUMPS       Full path to local directory where database dumps
                                    will be stored.
        -f MC_FILES             Full path to local directory where Drupal files/
                                    directories will be stored.
        -u DEV_SERVER_USERNAME  Optional username to use when logging into the dev
                                    server.  Defaults to \$(whoami).

examples:

    # Specify user name on dev server
    ./${script_name} -d ~/mediacommons_databases -f ~/mediacommons_files -u somebody

    # Use user name on local machine as login name on dev server
    ./${script_name} -d ~/mediacommons_databases -f ~/mediacommons_files
EOF
}

while getopts d:ef:u: opt
do
    case $opt in
        d) DATABASE_DUMPS=$OPTARG ;;
        f) MC_FILES=$OPTARG ;;
        u) DEV_SERVER_USERNAME=$OPTARG ;;
        *) echo >&2 "Options not set correctly."; usage; exit 1 ;;
    esac
done

if [ -z $DEV_SERVER_USERNAME ]; then
    DEV_SERVER_USERNAME=$(whoami)
fi

validate_args

echo -n "Password for web server: "
read -s password
echo

# Number of sites + files/ + database dumps = 6 + 1 + 1 = 8
RSYNC_COUNT=8

/usr/local/bin/expect<<EOF
set timeout -1

spawn ${REFRESH_SCRIPT} -e -d ${DATABASE_DUMPS} -f ${MC_FILES} -u ${DEV_SERVER_USERNAME}

for {set i 1} {\$i <= ${RSYNC_COUNT}} {incr i 1} {
    expect "${DEV_SERVER_USERNAME}@${DEV_SERVER}'s password:"

    send "$password\r";

    expect {
        -re "total size is .*  speedup is " { puts "\nrsync #\$i completed successfully." }

        "Permission denied, please try again." {
             puts "\nYou will need to run this script again and re-type your password."
             exit 1
        }
    }
}

expect "${SCRIPT_RUN_COMPLETE}"
EOF

