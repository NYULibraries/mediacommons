<?php

function setup_project_environment () {

  variable_set('mediacommons_project', 'altac');

  drupal_flush_all_caches();

}


function _prepare_blocks($theme) {
  $blocks = _block_rehash($theme);
  $compare_theme = &drupal_static('_block_compare:theme');
  $compare_theme = $theme;
  usort($blocks, '_block_compare');
  return $blocks;
}

function enable_twitter_block() {
  drush_log('Task: Enable Twitter block', 'ok');
  $enabled = module_enable(array('mediacommons_twitter'));
  if ($enabled) {
    variable_set('mediacommons_twitter_label', 'Twitter Widget');
    variable_set('mediacommons_twitter_url', 'https://twitter.com/lhenze/lists/mediacommons');
    $args = array('weight' => '-1', 'region' => 'homepage_sidebar', 'theme' => 'mediacommons', 'status' => '1');
    $theme = 'mediacommons';
    $block = 'mediacommons_twitter';
    drupal_theme_initialize();
    // Fetch and sort blocks. We need to do this to init our block
    $blocks = _prepare_blocks($theme);
    $transaction = db_transaction();
    try {
      db_update('block')->fields($args)->condition('module', $block)->condition('delta', $block)->condition('theme', 'mediacommons')->execute();
    }
    catch (Exception $e) {
      $transaction->rollback();
      print_r($e);
      throw $e;
    }
    
    // Flush cache
    cache_clear_all();

    drupal_flush_all_caches();

  }
}
