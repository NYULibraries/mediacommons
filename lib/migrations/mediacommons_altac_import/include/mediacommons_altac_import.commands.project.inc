<?php

function setup_project_environment () {

  variable_set('mediacommons_project', 'altac');

  variable_set('mediacommons_umbrella_url', 'http://devmc2.dlib.nyu.edu/mediacommons');
  
  drupal_flush_all_caches();

}
