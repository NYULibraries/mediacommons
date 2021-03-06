#!/bin/bash

# Root of mediacommons repo
MEDIACOMMONS=$(cd "$(dirname "$0")" ; cd ..; pwd -P )

REFRESH_SCRIPT=${MEDIACOMMONS}/bin/$(basename $0 _expect.sh).sh

source $MEDIACOMMONS/bin/$(basename $REFRESH_SCRIPT .sh)_common.sh

if [ -z $(which expect) ]
then
    cat >&2 <<EOF

Expect does not appear to be installed.  You will need to install it or put the
executable in your PATH in order to run this script.

Expect: http://expect.sourceforge.net/

Alternatively, you can run the refresh script without expect support:

    ${REFRESH_SCRIPT} ${DATABASE_DUMPS} ${MC_FILES} ${NETWORK_HOST_USERNAME}
EOF

    exit 1
fi

function usage() {
    script_name=$(basename $0)

    cat <<EOF

usage: ${script_name} -d DATABASE_DUMPS -f MC_FILES [-u NETWORK_HOST_USERNAME]
    options:
        -d DATABASE_DUMPS        Full path to local directory where database dumps
                                     will be stored.
        -f MC_FILES              Full path to local directory where Drupal files/
                                     directories will be stored.
        -u NETWORK_HOST_USERNAME Optional username to use when logging into the
                                     bastion host and dev server.
                                     Defaults to \$(whoami).

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
        u) NETWORK_HOST_USERNAME=$OPTARG ;;
        *) echo >&2 "Options not set correctly."; usage; exit 1 ;;
    esac
done

if [ -z $NETWORK_HOST_USERNAME ]; then
    NETWORK_HOST_USERNAME=$(whoami)
fi

validate_args

echo -n "Password for ${BASTION_HOST} and ${DEV_SERVER}: "
read -s password
echo

# Originally had heredoc:
# expect<<EOF
# ...but when added `interact` command it didn't work right.  Nothing was being
# echoed from the spawned process, and everything just seemed to hang.
expect -c "
set timeout -1

set return_signal \"${EXPECT_SIGNAL_SELECT_SITES_COMPLETED}(\\\\d+)\"

spawn ${REFRESH_SCRIPT} -e -d ${DATABASE_DUMPS} -f ${MC_FILES} -u ${NETWORK_HOST_USERNAME}

interact {
    -o -re \$return_signal {
        set num_selected_sites \$interact_out(1,string)
        return
    }
}

set num_ssh_drush_sql_dump_calls_to_perform \"\$num_selected_sites\"

puts \"\\nNumber of ssh drush sql-dump calls to perform: \$num_ssh_drush_sql_dump_calls_to_perform\"

for {set i 1} {\$i <= \$num_ssh_drush_sql_dump_calls_to_perform} {incr i 1} {
    expect \"${NETWORK_HOST_USERNAME}@${BASTION_HOST}'s password:\"

    send \"$password\r\";

    expect \"${NETWORK_HOST_USERNAME}@${DEV_SERVER}'s password:\"

    send \"$password\r\";

    expect {
        \"Database dump saved to\" { puts \"\ndrush sql-dump #\$i completed successfully.\" }

        \"Permission denied, please try again.\" {
             puts \"\nYou will need to run this script again and re-type your password.\"
             exit 1
        }
    }
}

# number of Drupal directories + number of files/ directories + database dumps directory
#     number of Drupal directories = number of sites
#     number of files/ directories = number of sites
#     database dumps directory     = 1
set num_rsyncs_to_perform \"[expr ( \$num_selected_sites * 2 ) + 1]\"

puts \"\\nNumber of rsyncs to perform: \$num_rsyncs_to_perform\"

for {set i 1} {\$i <= \$num_rsyncs_to_perform} {incr i 1} {
    expect \"${NETWORK_HOST_USERNAME}@${BASTION_HOST}'s password:\"

    send \"$password\r\";

    expect \"${NETWORK_HOST_USERNAME}@${DEV_SERVER}'s password:\"

    send \"$password\r\";

    expect {
        -re \"total size is .*  speedup is \" { puts \"\nrsync #\$i completed successfully.\" }

        \"Permission denied, please try again.\" {
             puts \"\nYou will need to run this script again and re-type your password.\"
             exit 1
        }
    }
}

expect \"${SCRIPT_RUN_COMPLETE}\"
"
