<?php

function _steps() {
  config_apachesolr();  
}

function config_apachesolr() {
  
  module_load_include('inc', 'apachesolr', 'apachesolr.index');
  
  $enabled = module_enable(array('apachesolr_multisitesearch'));
  
  // Make this Solr search environment multisite capable
  apachesolr_environment_variable_set('solr', 'multisitesearch', TRUE);
  
  $hash = 'imr';
  
  variable_set('apachesolr_site_hash', $hash);
  
  //print_r(apachesolr_environment_variable_get('solr', 'multisitesearch_sites', array())); 
  
  if ($enabled) {
    variable_set('make_multisite', "1");
  }

  $blacklisted = array(
    'front_page_post',
    'message',
    'page',
  );

  $bundles = array_keys(node_type_get_types());

  $bundles_to_index = array_diff($bundles, $blacklisted);
  
  $env_id = 'solr';

  $entity_types = array('node', 'comment');
  
  foreach ($entity_types as $entity_type) {
    $transaction = db_transaction();
    try {      
      db_delete('apachesolr_index_bundles')->condition('env_id', $env_id)->condition('entity_type', $entity_type)->execute();
      $insert = db_insert('apachesolr_index_bundles')->fields(array('env_id', 'entity_type', 'bundle'));
      foreach ($bundles_to_index as $bundle) {
        if ($entity_type == 'comment') {
          $bundle = 'comment_node_' . $bundle;
        }
        $i = $insert->values(array('env_id' => $env_id, 'entity_type' => $entity_type, 'bundle' => $bundle));
      }
      $done = $insert->execute();
    }
    catch (Exception $e) {
      $transaction->rollback();
      throw $e;
    }
  }
  
  $info = facetapi_get_facet_info('apachesolr@solr');
  
  $realm = facetapi_realm_load('block');
  
  $adapter = facetapi_adapter_load('apachesolr@solr');
  
  $blocks = array(
    'bundle', // Content type. Filter by content type.
    'im_field_co_editor', // Co-editor. Filter by field field_co_editor of type user_reference.
    'im_field_contributors', // Contributors. Filter by field field_contributors of type user_reference.
    'im_field_curators_editors', // Curated by. Filter by field field_curators_editors of type user_reference.
    'im_field_taxonomy',  // Tags. Filter by field field_taxonomy of type taxonomy_term_reference.
    'dm_field_pubdate', // Publication date. Filter by field field_pubdate of type datestamp.      
  );
  // Make sure we have the blocks we want to enable  
  foreach ($info as $facet_name => $facet) {
    if (in_array($facet_name, $blocks)) {
      facetapi_save_facet_status($adapter, $realm, $facet, 1, 0, TRUE);      
    }
  }
  
  $theme = 'mediacommons';
  
  $region = 'searchresults_sidebar';
  
  $blocks = mediacommons_prepare_blocks($theme);

  drupal_theme_initialize();
  
  $args = array(
    'weight' => '-1',
    'status' => '1',
    'region' => $region,
    'theme' => $theme,
  );
  
  foreach (facetapi_get_block_info(array('block')) as $block => $data) {
    $transaction = db_transaction();
    try {
      db_update('block')
        ->fields($args)
        ->condition('module', 'facetapi')
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
  
  cache_clear_all();

}

/**
 * Callback to set any non-standard MC distribution field
 */
function mediacommons_imr_import_extend_fields () {
  return array('field_embed');
}

// The relevant page for IMR on production is
// "Current Calls" :
// http://mediacommons.futureofthebook.org/imr/current-calls
function mediacommons_imr_import_MC151() {

 module_load_include('inc', 'menu', 'menu.admin');

 $menu_name = 'main-menu';

 $org_path = drupal_lookup_path("source", 'current-calls');

 $path = array('source' => $org_path, 'alias' => 'how-it-works');

 $path = path_save($path);

 $tree = menu_tree_all_data($menu_name);

 $weights = array();

 // capture the $weights so that we can place the link in the last position
 foreach ($tree as $leaf) $weights[] = $leaf['link']['weight'];

 $form_state = array(
  'values' => array(
      'menu_name'  => 'main-menu',
      'weight'     => max($weights) + 1,
      'link_title' => 'Current Calls',
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

function setup_project_environment () {
  variable_set('mediacommons_project', 'imr');
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

    $twitter_user = 'MC_IMR';  

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
