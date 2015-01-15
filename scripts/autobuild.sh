#!/bin/bash

# Don't use me! ... well do it if you know what you are doing.
#
# In theory I should work; but in practice I'm almost certain that I will
# fail if you run me in a machine that does not looks like the one
# I'm intended to run it. So, don't use me if you are not sure, I can 
# easily get you into trouble.

# Get the latest make file and do any other task before running jobs
./scripts/update.sh

# Do some house cleaning before running job
./scripts/maintenances.sh

# Takes care of grabbing the most recent production databases and import them 
./scripts/prepare.sh

# Build MediaCommons umbrella
./scripts/deploy_build.sh -c mediacommons.conf mediacommons.make

# Build The New Everyday
./scripts/deploy_build.sh -c tne.conf mediacommons.make

# Build Alt-Academy
./scripts/deploy_build.sh -c alt-ac.conf mediacommons.make

# Build [in]Transition
./scripts/deploy_build.sh -c intransition.conf mediacommons.make

# Build In Media Res
./scripts/deploy_build.sh -c imr.conf mediacommons.make

# Migrate the content of all the sites
./scripts/migrate_d6_to_d7.sh

# Run a basic test and report if error
./scripts/report_error.sh
