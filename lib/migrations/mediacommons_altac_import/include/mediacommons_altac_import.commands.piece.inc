<?php

function migrate_contributed_pieces ( ) {
  foreach ( contributed_pieces () as $key => $entity ) migrate_contributed_piece ( $entity ) ;
}

function migrate_contributed_piece ( $entity ) {

  /** Prepare node */
  $node = array () ;

  $node['title'] = $entity->title ;

  $node['ouid'] = $entity->uid ;

  $node['onid'] = $entity->nid ;

  $node['language'] = LANGUAGE_NONE ;

  $node['date'] = date( 'Y-m-j H:i:s' , $entity->created ) ;

  $node['status'] = $entity->status ;

  $node['field_pubstat'] = $entity->status ;

  $node['promote'] = $entity->promote ;

  $node['moderate'] = $entity->moderate ;

  $node['sticky'] = $entity->sticky ;

  $node['field_contributors'] = array() ;

  /** Field body */
  if ( isset ( $entity->field_document_textarea_body_value ) ) {

  	extract_iframe_sources ( $entity->field_document_textarea_body_value ) ;

  	$node['field_body'] = $entity->field_document_textarea_body_value ;

  }

  /**
   * Tagline
   * @TODO: We don't have a migration path for this field. Ask Mark  about what we want to do with this
   */
  if ( isset ( $entity->field_tagline ) ) $node['field_tagline'] = $entity->field_tagline ;

  /** Tags */
  foreach ( explode ( ',' , $entity->terms ) as $tid ) $node['field_tags'][] = $tid ;

  /** Additional authors */
  if ( $entity->additonal_authors ) $node['field_contributors'] = explode ( ',' , $entity->additonal_authors ) ;

  /** Attached images */
  $node['field_attached_images'] = array() ;

  /** Representative image */
  if ( isset ( $entity->thumbnail_fid ) ) $node['field_attached_images_ri'] = (object) array ( 'fid' => $entity->thumbnail_fid ) ;

  foreach ( attached_images_by_onid ( $entity->nid ) as $image ) $node['field_attached_images'][] = $image->fid ;

  if ( ! empty ( $node['field_body'] ) ) find_iframe_sources ( $node['field_body'] ) ;

  create_spoke ( $node ) ;

}
