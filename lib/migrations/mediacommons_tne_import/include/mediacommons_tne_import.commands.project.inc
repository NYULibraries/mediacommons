<?php

function setup_project_environment () {

  variable_set('mediacommons_project', 'tne');

  drupal_flush_all_caches();

}

function enable_twitter_block() {

  drush_log('Task: Enable Twitter block', 'ok');

  module_enable(array('mediacommons_twitter'));

  variable_set('mediacommons_twitter_label', 'Twitter Widget');

  variable_set('mediacommons_twitter_url', 'https://twitter.com/lhenze/lists/mediacommons');

  $block = block_load('mediacommons_twitter', 'mediacommons_twitter');

  db_update('block')
    ->fields(array(
      'weight' => -1,
      'region' => 'homepage_sidebar',
      'theme' => 'mediacommons',
      'status' => 1,
    ))
    ->condition('module', 'mediacommons_twitter')
    ->condition('delta', 'mediacommons_twitter')
    ->condition('theme', 'mediacommons')
    ->execute();

  // Flush cache
  cache_clear_all();

  drupal_flush_all_caches();

}
