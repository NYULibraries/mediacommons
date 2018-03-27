MediaCommons build and migration
============

<a href="https://github.com/drush-ops/drush">Drush</a> is require to run the script, you also need to have <a href="http://compass-style.org/">Compass</a> to compile the default theme SASS into CSS.

Configure and build
============

Configure the build script by copying the default template default.project.conf as project.conf and fill out the blanks

	cp default.project.conf configs/project.conf

Build the drupal distribution, e.g.

	./bin/build.sh -c configs/project.conf -m mediacommons.make -e development


Usage options for build.sh
============

	./bin/build.sh -m example.make -c example.conf -l

Options:
============

Flag | Description
--- | ---
h | Show brief help
e | Set the environment variable (default to local) if not set.
k | Allow site to share cookies accross domain  
s | Find SASS based themes and compile
c | Specify the configuration file to use (e.g., -c example.conf).
m |  Specify the make file to use (e.g., -m example.make).
t | Tell all relevant actions (don't actually change the system).
l | Legacy option


migrate.sh
============

Bash script use for sites migration. NOTE: It's not recomended to run this program in your local environment (see import_db.sh).

Usage options for migrate.sh
============

	./bin/migrate.sh -c example.conf
	
Options:
============

Flag | Description
--- | ---
h | Show brief help
c | Specify the configuration file to use (e.g., -c example.conf).

import_db.sh
============

Import a copy (mysql dump) of a recently build website plus the migrated content. The script works with the presumption that your enviorment is up-to-date (core, modules, themes) and almost a mirror of our development enviorment. NOTE: See configs/default.project.conf

Usage options for import_db.sh
============

	./bin/utilities/import_db.sh -c example.conf
	
Options:
============

Flag | Description
--- | ---
h | Show brief help
c | Specify the configuration file to use (e.g., -c example.conf).


Run tests
============

Install composer: https://getcomposer.org/

Install dependencies:

     composer install
     
Run all tests:

     vendor/phpunit/phpunit/phpunit tests/
     
Run Apache redirect tests only:

     vendor/phpunit/phpunit/phpunit --filter "testRedirect" \
         ApacheConfigurationTest tests/ApacheConfigurationTest.php
 
Run simple test of the internal data provider for the redirect tests:

     vendor/phpunit/phpunit/phpunit --filter "testGenerateTestUrlsSimple" \
         ApacheConfigurationTest tests/ApacheConfigurationTest.php
          
Run comprehensive test of the internal data provider for the redirect tests:

     vendor/phpunit/phpunit/phpunit --filter "testGenerateTestUrls$" \
         ApacheConfigurationTest tests/ApacheConfigurationTest.php
          
Run both simple and comprehensive tests of the internal data provider for the redirect tests:

     vendor/phpunit/phpunit/phpunit --filter "testGenerateTestUrls" \
             ApacheConfigurationTest tests/ApacheConfigurationTest.php

Run all tests in ApacheConfigurationTest:

     vendor/phpunit/phpunit/phpunit tests/ApacheConfigurationTest.php
 