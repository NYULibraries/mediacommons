#!/bin/bash

# Takes care of grabbing the most recent production databases and inport them 
./scripts/prepare.sh

./scripts/deploy_build.sh -c tne.conf mediacommons.make

./scripts/deploy_build.sh -c alt-ac.conf mediacommons.make

./scripts/deploy_build.sh -c intransition.conf mediacommons.make

./scripts/deploy_build.sh -c imr.conf mediacommons.make

./scripts/deploy_build.sh -c mediacommons.conf mediacommons.make

# migrate the content
./scripts/migrate_d6_to_d7.sh