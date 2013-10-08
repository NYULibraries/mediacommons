MediaCommons README
============

# Building the Drupal Distribution

Install drush 5.9.0. (drush make-local fail with drush 6)

	pear install drush/drush-5.9.0
    
Install make_local

	drush dl make_local
	
Quick start

Clone the repo: git@github.com:NYULibraries/mediacommons.git

Configure the build script by copying the default template default.project.conf as project.conf and fill out the blanks

	cp default.project.conf project.conf

Build the drupal distribution, e.g.

	sh dev_scripts/deploy_build.sh mediacommons.make



