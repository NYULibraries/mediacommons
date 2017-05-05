<?php

function setup_project_environment () {

  variable_set('mediacommons_project', 'fieldguide');

  drupal_flush_all_caches();

}

function enable_mediacommons_views () {
  
  module_enable(array('mediacommons_views'));
  
  drupal_flush_all_caches();

}
