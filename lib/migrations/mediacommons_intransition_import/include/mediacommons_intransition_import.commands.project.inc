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

/**
 * Callback to set any non-standard MC distribution field
 */
function mediacommons_intransition_import_extend_fields () {
  return array ('field_embed');
}

function mediacommons_intransition_import_MC151 () {

  module_load_include('inc', 'menu', 'menu.admin');

  $menu_name = 'main-menu';

  $org_path = drupal_lookup_path("source", 'contribute');

  $path = array('source' => $org_path, 'alias' => 'how-it-works');

  path_save($path);

  $tree = menu_tree_all_data($menu_name);

  $weights = array();

  // capture the $weights so that we can place the link in the last position
  foreach ($tree as $leaf) $weights[] = $leaf['link']['weight'];

  $form_state = array(
    'values' => array(
      'menu_name'  => 'main-menu',
      'weight'     => max($weights) + 1,
      'link_title' => 'Contribute',
      'link_path'  => $org_path,
      'module'     => 'mediacommons_site_navigation',
      'mlid'       => 0,
    ),
  );

  // Validate and transform the item, so it conforms with Drupal standards.
  menu_edit_item_validate(array(), $form_state);
  
  // Save the item to database.
  menu_link_save($form_state['values']);
  
}

function mediacommons_intransition_import_custom_blocks () {

  $templates = drupal_get_path('module', 'mediacommons_blocks') . '/templates';

  $content_connect = file_get_contents("$templates/int-connect-block.html");

  $options = array(
    'type' => 'fulltext',
    'mbid' => 'mc_intransition_connect',    
    'label' => 'Connect',
    'content' => $content_connect,
    'options' => array(
      'status' => 1,
      'weight' => -2,
      'region' => 'homepage_sidebar',
      'visibility' => BLOCK_VISIBILITY_LISTED,
      'pages' => '<front>',
    ),
  );
  
  mediacommons_blocks_save_block($options);

  $content_announce = file_get_contents("$templates/int-announce-block.html");

  $options = array(
    'type' => 'fulltext',
    'mbid' => 'mc_intransition_announce',    
    'label' => 'Announce',
    'content' => $content_announce,
    'options' => array(
      'status' => 1,
      'weight' => -3,
      'region' => 'homepage_sidebar',
      'visibility' => BLOCK_VISIBILITY_LISTED,
      'pages' => '<front>',
    ),
  );
  
  mediacommons_blocks_save_block($options);
  
  drupal_flush_all_caches();

}

function setup_project_environment () {

  variable_set('mediacommons_project', 'intransition');

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
  
    // https://jira.nyu.edu/jira/browse/MC-319
    
    $region = 'homepage_sidebar';

    $theme = 'mediacommons';

    $block = 'mediacommons_twitter';
    
    variable_set('mediacommons_twitter_label', 'Twitter Widget');

    //$twitter_url = '';
    
    //variable_set('mediacommons_twitter_url', $twitter_url);

    $twitter_user = 'inTransitionCMS';  

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
