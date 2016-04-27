MediaCommons build and migration
============

<a href="https://github.com/drush-ops/drush">Drush</a> is require to run the script, you also need to have <a href="http://compass-style.org/">Compass</a> to compile the default theme SASS into CSS.

Configure and build
============

Configure the build script by copying the default template default.project.conf as project.conf and fill out the blanks

	cp default.project.conf configs/project.conf

Build the drupal distribution, e.g.

	./bin/build.sh -c configs/project.conf -m mediacommons.make -k -s -e development
