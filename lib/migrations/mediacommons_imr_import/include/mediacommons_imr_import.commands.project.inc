<?php

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
