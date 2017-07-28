<?php

function setup_project_environment() {

  variable_set('mediacommons_project', 'mediacommons');

}

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

function enable_twitter_block() {

  drush_log('Task: Enable Twitter block', 'ok');
  
  $enabled = module_enable(array('mediacommons_twitter'));
  
  if ($enabled) {
  
    // https://jira.nyu.edu/jira/browse/MC-319
    
    $region = 'umb_homepage_sidebar';

    $theme = 'mediacommons';

    $block = 'mediacommons_twitter';
    
    $twitter_user = 'mediacommons';  
      
    variable_set('mediacommons_twitter_label', 'Twitter Widget');
    
    // variable_set('mediacommons_twitter_url', 'https://twitter.com/lhenze/lists/mediacommons');
    
    variable_set('mediacommons_twitter_user', $twitter_user);

    $args = array(
      'weight' => '-1', 
      'status' => '1',
      'region' => $region,
      'theme' => $theme,
    );

    drupal_theme_initialize();
    
    // Fetch and sort blocks. We need to do this to init our block
    $blocks = mediacommons_prepare_blocks($theme);
    
    $transaction = db_transaction();
    
    try {
      db_update('block')
         ->fields($args)
        ->condition('module', $block)
        ->condition('delta', $block)
        ->condition('theme', 'mediacommons')
        ->execute();
    }
    
    catch (Exception $e) {
      $transaction->rollback();
      print_r($e);
      throw $e;
    }
  }  
  
}
