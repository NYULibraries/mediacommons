<?php

function migrate_posts ( ) {
  foreach ( posts () as $old_node ) migrate_post ( $old_node ) ;
}

function migrate_post ( $old_node  ) {

  $node = array();

  $node['title'] = $old_node->title;

  $node['ouid'] = $old_node->uid;

  $node['onid'] = $old_node->nid;

  $node['language'] = LANGUAGE_NONE ;

  $node['date'] = date('Y-m-j H:i:s', $old_node->created);

  $node['status'] = $old_node->status;

  $node['field_pubstat'] = $old_node->status ;  

  $node['promote'] = $old_node->promote;

  $node['moderate'] = $old_node->moderate;

  $node['sticky'] = $old_node->sticky;

  if ( isset ( $old_node->body ) ) {
    
    extract_iframe_sources ( $old_node->body ) ;  
    
    $node['field_body'] = $old_node->body ;

  }

  foreach ( explode ( ', ', $old_node->terms ) as $tid ) $node['field_tags'][] = $tid ;

  if ( isset ( $old_node->field_tease_image_fid ) ) {
    $node['field_attached_images_ri'] = (object) array( 'fid' => $old_node->field_tease_image_fid, );
  }

  if ( isset ( $old_node->field_video_embed_link_value ) ) {
  
    extract_iframe_sources ( $old_node->field_video_embed_link_value ) ;  
  
    $node['field_embed'] = array ( LANGUAGE_NONE => array ( array ( 'value' => $old_node->field_video_embed_link_value, 'format' => 'limited_html' ) ) ) ;
  }
  
  create_spoke ( $node ) ;
  
}
