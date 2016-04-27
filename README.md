MediaCommons build and migration
============

<a href="https://github.com/drush-ops/drush">Drush</a> is require to run the script, you also need to have <a href="http://compass-style.org/">Compass</a> to compile the default theme SASS into CSS.

Configure and build
============

Configure the build script by copying the default template default.project.conf as project.conf and fill out the blanks

	cp default.project.conf configs/project.conf

Build the drupal distribution, e.g.

	./bin/build.sh -c configs/project.conf -m mediacommons.make -k -s -e development


Usage options for build.sh
============

Usage: ./build.sh -m example.make -c example.conf

Options:

-h           Show brief help
-e           Set the environment variable (default to local) if not set.
-k           Allow site to share cookies accross domain  
-s           Find SASS based themes and compile
-c <file>    Specify the configuration file to use (e.g., -c example.conf).
-m <file>    Specify the make file to use (e.g., -m example.make).
-t           Tell all relevant actions (don't actually change the system).
 
