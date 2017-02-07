<?php

function setup_project_environment () {

  variable_set('mediacommons_project', 'imr');

  drupal_flush_all_caches();

}
