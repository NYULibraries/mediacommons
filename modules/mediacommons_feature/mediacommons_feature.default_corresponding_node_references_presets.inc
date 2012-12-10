<?php
/**
 * @file
 * mediacommons_feature.default_corresponding_node_references_presets.inc
 */

/**
 * Implements hook_default_corresponding_node_references().
 */
function mediacommons_feature_default_corresponding_node_references() {
  $export = array();

  $cnr_obj = new stdClass();
  $cnr_obj->disabled = FALSE; /* Edit this to true to make a default cnr_obj disabled initially */
  $cnr_obj->api_version = 1;
  $cnr_obj->node_types_content_fields = 'cluster*field_contributed_piece*contributed_piece*field_clusters';
  $cnr_obj->enabled = 1;
  $export['cluster*field_contributed_piece*contributed_piece*field_clusters'] = $cnr_obj;

  return $export;
}
