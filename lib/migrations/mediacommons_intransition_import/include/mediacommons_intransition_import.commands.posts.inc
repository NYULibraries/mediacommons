<?php

/**
 * Migrate IMR post.
 */
function migrate_posts ( ) {
  foreach ( posts () as $key => $node ) migrate_post ( $node ) ;
}

/**
 * Migrate IMR posts.
 */
function migrate_post ( $old_node ) {

  extract_iframe_sources($old_node->body);

  /** prepare node */
  $node = array () ;

  $node['title'] = $old_node->title ;

  $node['ouid'] = $old_node->uid ;

  $node['onid'] = $old_node->nid ;

  $node['language'] = LANGUAGE_NONE ;

  $node['date'] = date( 'Y-m-j H:i:s', $old_node->created ) ;

  $node['status'] = $old_node->status ;

  $node['promote'] = $old_node->promote ;

  $node['moderate'] = $old_node->moderate ;

  $node['sticky'] = $old_node->sticky ;

  $node['field_pubstat'] = $old_node->status ;

  $node['field_body'] = $old_node->body ;

  foreach (explode (', ', $old_node->terms ) as $tid )  $node['field_tags'][] = $tid ;

  /** representative image */
  if ( isset ( $old_node->field_tease_image_fid ) ) $node['field_attached_images_ri'] = ( object ) array ( 'fid' => $old_node->field_tease_image_fid ) ;

  /**
   * Add embed content
   * Extend field must be explicitly added to mediacommons_imr_import_extend_fields()
   */
  if ( isset ( $old_node->field_video_embed_link_value ) ) {

  	extract_iframe_sources ( $old_node->field_video_embed_link_value ) ;

  	$clean_embed_field_input = clean_embed_field_input ( $old_node->field_video_embed_link_value ) ;

  	if ( empty ( $clean_embed_field_input ) ) $clean_embed_field_input = $old_node->field_video_embed_link_value ;

    $node['field_embed'] = array (
      $node['language'] => array (
        array (
          'value' => $clean_embed_field_input ,
          'format' => 'limited_html' ,
        ),
      ),
    ) ;

  }

  create_spoke ( $node ) ;

}
