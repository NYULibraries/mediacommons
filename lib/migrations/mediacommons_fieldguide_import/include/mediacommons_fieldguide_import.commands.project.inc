<?php

function config_apachesolr() {

  module_load_include('inc', 'apachesolr', 'apachesolr.index');

  $blacklisted = array(
    'front_page_post'
  );

  $bundles = array_keys(node_type_get_types());

  $bundles_to_index = array_diff($bundles, $blacklisted);

  $env_id = 'solr';

  $entity_types = array('node', 'comment');
  
  foreach ($entity_types as $entity_type) {
    $transaction = db_transaction();
    try {
      db_delete('apachesolr_index_bundles')
        ->condition('env_id', $env_id)
        ->condition('entity_type', $entity_type)
        ->execute();
      $insert = db_insert('apachesolr_index_bundles')
        ->fields(array('env_id', 'entity_type', 'bundle'));
      foreach ($bundles_to_index as $bundle) {
        if ($entity_type == 'comment') $bundle = 'comment_node_' . $bundle;  
        $insert->values(array(
          'env_id' => $env_id,
          'entity_type' => $entity_type,
          'bundle' => $bundle,
        ));
      
      }
      $insert->execute();
    }
    catch (Exception $e) {
      $transaction->rollback();
      throw $e;
    }
  }
  cache_clear_all();
}

function setup_project_environment () {

  variable_set('mediacommons_project', 'fieldguide');

  drupal_flush_all_caches();

}

function enable_mediacommons_views () {
  
  module_enable(array('mediacommons_views'));
  
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
