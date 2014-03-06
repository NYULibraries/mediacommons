MediaCommons README
============

# Building the Drupal Distribution

Install drush

	pear install drush/drush

Quick start
============

Clone the repo: git@github.com:NYULibraries/mediacommons.git

Configure and build
============

Configure the build script by copying the default template default.project.conf as project.conf and fill out the blanks

	cp default.project.conf project.conf

Build the drupal distribution, e.g.

	sh scripts/deploy_build.sh mediacommons.make



