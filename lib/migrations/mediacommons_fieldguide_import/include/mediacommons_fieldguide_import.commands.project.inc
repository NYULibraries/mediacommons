<?php

function setup_project_environment () {

  variable_set('mediacommons_project', 'fieldguide');

  drupal_flush_all_caches();

}
