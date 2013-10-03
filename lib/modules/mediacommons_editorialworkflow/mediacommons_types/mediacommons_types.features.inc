<?php
/**
 * @file
 * mediacommons_types.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function mediacommons_types_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "corresponding_node_references" && $api == "default_corresponding_node_references_presets") {
    return array("version" => "1");
  }
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_node_info().
 */
function mediacommons_types_node_info() {
  $items = array(
    'hub' => array(
      'name' => t('Hub'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'spoke' => array(
      'name' => t('Spoke'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
