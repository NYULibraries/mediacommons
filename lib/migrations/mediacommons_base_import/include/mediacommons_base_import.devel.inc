<?php

function enable_devel_options() {
  if (!module_exists('devel')) {
    $devel_path = DRUPAL_ROOT . '/' . drupal_get_path('module', 'devel');
    if (file_exists($devel_path)) {
      module_enable(array('devel'));
    }
  }
  variable_set('theme_debug', 1);
}
