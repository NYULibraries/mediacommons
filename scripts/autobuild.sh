#!/bin/bash

# Get the latest make file and do any other task before running jobs
./scripts/update.sh

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
