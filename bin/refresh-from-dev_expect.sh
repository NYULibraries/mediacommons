#!/bin/bash

# Root of mediacommons repo
MEDIACOMMONS=$(cd "$(dirname "$0")" ; cd ..; pwd -P )
REFRESH_SCRIPT=${MEDIACOMMONS}/bin/$(basename $0 _expect.sh).sh

source $MEDIACOMMONS/bin/$(basename $REFRESH_SCRIPT .sh)_common.sh

echo -n "Password for web server: "
read -s password
echo

# Number of sites + files/ + database dumps = 6 + 1 + 1 = 8
RSYNC_COUNT=8

/usr/local/bin/expect<<EOF
set timeout -1

spawn ${REFRESH_SCRIPT} ${DATABASE_DUMPS} ${MC_FILES} ${DEV_SERVER_USERNAME}

for {set i 1} {\$i <= ${RSYNC_COUNT}} {incr i 1} {
    expect "arjanik@devmc.dlib.nyu.edu's password:"

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

