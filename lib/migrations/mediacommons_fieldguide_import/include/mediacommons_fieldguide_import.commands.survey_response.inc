<?php

function clean_images_path ( $text ) {

  return $text ;

}

/**
 * Migrate Survey responses.
 */
function migrate_survey_responses ( ) {
  foreach ( survey_responses () as $old_node ) migrate_survey_response ( $old_node ) ;
}

/**
 * Migrate Survey response.
 */
function migrate_survey_response ( $old_node ) {

  $node = array () ;

  $node['title'] = $old_node->title ;
  
  $node['ouid'] = $old_node->uid ;

  $node['onid'] = $old_node->nid ;

  $node['language'] = LANGUAGE_NONE ;

  $node['date'] = date ( 'Y-m-j H:i:s', $old_node->created ) ;

  $node['status'] = $old_node->status ;

  $node['field_pubstat'] = $old_node->status ;

  $node['promote'] = $old_node->promote ;

  $node['moderate'] = $old_node->moderate ;

  $node['sticky'] = $old_node->sticky ;

  $node['position'] = $old_node->position ;

  $node['field_attached_images'] = array ( ) ;

  if ( isset ( $old_node->field_response_value ) ) {
  
    extract_iframe_sources ( $old_node->field_response_value ) ;
    
  	$node['field_body'] = clean_images_paths ( $old_node->field_response_value ) ;
  	
  }

  if ( isset( $old_node->field_sr_reference_nid ) ) $node['field_part_of'] = array ( $old_node->field_sr_reference_nid ) ;

  /** Attached images */

  if ( isset ( $old_node->images ) ) {
    foreach ( (array) explode ( ',' , $old_node->images ) as $image ) {
      if ( $attached_image = attached_images_by_ofid ( $image ) ) $node['field_attached_images'][] = $attached_image ;
	}
  }
  
  create_spoke ( $node ) ;

}

function response_arrange_order () {
  drush_log ( __FUNCTION__ , 'OK' ) ;
  foreach ( survey_questions () as $entity ) {
    if ( $nid = nid_by_onid ( $entity->nid ) ) {
      $wrapper = entity_metadata_wrapper ( 'node', node_load ( $nid  ) ) ;
      $wrapper->field_spokes->set( array() ) ;
      foreach ( survey_response_by_question_nid ( $entity->nid ) as $key => $ref ) {
        if ( $ref_nid = nid_by_onid ( $ref->nid ) ) $wrapper->field_spokes[$key]->set( (int) $ref_nid ) ;
      }
      $wrapper->save() ;
    }
  }
}
