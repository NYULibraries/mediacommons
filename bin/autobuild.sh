#!/bin/bash

# Don't use me! ... well, do it if you know what you are doing.
#
# In theory I should work; but in practice I'm almost certain that I will
# fail if you run me in a machine that does not looks like the one
# I'm intended to run it. So, don't use me if you are not sure, I can 
# easily get you into trouble.

SOURCE="${BASH_SOURCE[0]}"

# resolve $SOURCE until the file is no longer a symlink
while [ -h "$SOURCE" ]; do 
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  # if $SOURCE was a relative symlink, we need to resolve it relative to the path where
  # the symlink file was located
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done

DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

cd $DIR/../

# Get the latest make file and do any other task before running jobs
./scripts/update.sh

# Do some house cleaning before running job
./scripts/maintenances.sh

# Takes care of grabbing the most recent production databases and import them 
./scripts/prepare.sh

# Build MediaCommons umbrella
./scripts/build.sh -c configs/mediacommons.conf mediacommons.make -s

# Build Field Guide
./scripts/build.sh -c configs/fieldguide.conf mediacommons.make -s

# Build The New Everyday
./scripts/build.sh -c configs/tne.conf mediacommons.make -s

# Build Alt-Academy
./scripts/build.sh -c configs/alt-ac.conf mediacommons.make -s

# Build [in]Transition
./scripts/build.sh -c configs/intransition.conf mediacommons.make -s

# Build In Media Res
./scripts/build.sh -c configs/imr.conf mediacommons.make -s

# Migrate the content of all the sites
# ./scripts/migrate.sh -c configs/alt-ac.conf

# Run a basic test and report if error
# ./scripts/report_error.sh
