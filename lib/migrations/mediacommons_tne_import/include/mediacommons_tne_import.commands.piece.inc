<?php

function migrate_contributed_pieces () {
  
  foreach ( contributed_pieces() as $spoke ) migrate_contributed_piece ( $spoke ) ;

}

function migrate_contributed_piece ( $spoke ) {
  
  if ( ! empty ( $spoke->language ) && $spoke->language == 'es' ) return ;
  
  if ( $spoke->nid == 14584 ) return ; // skip problematic node  
  
  $node = array();
  
  $node['title'] = $spoke->title;

  $node['ouid'] = $spoke->uid;

  $node['onid'] = $spoke->nid;
  
  $node['field_pubstat'] = $spoke->status;  

  $node['language'] = LANGUAGE_NONE;
  
  $node['date'] = date('Y-m-j H:i:s', $spoke->created);

  $node['status'] = $spoke->status;

  $node['promote'] = $spoke->promote;

  $node['moderate'] = $spoke->moderate;

  $node['sticky'] = $spoke->sticky;

  if ( isset ( $spoke->field_document_textarea_body_value ) ) {

    $node['field_body'] = $spoke->field_document_textarea_body_value;
    
    extract_iframe_sources ( $spoke->field_document_textarea_body_value ) ;

  }

  /** Tags */
  foreach ( explode (',', $spoke->terms) as $tid ) $node['field_tags'][] = (int) trim ( $tid ) ;  
  
  /** Additional authors */
  if ( $spoke->additonal_authors ) {
  
    $node['field_contributors'] = array () ;
    
    /** Contributors */
    $spoke_contributors = explode (',', $spoke->additonal_authors ) ;
    
    /** Contributor order */
    foreach ( $spoke_contributors as $key => $contributor ) {
      $node['field_contributors'][] = array ( 'uid' => $contributor, '_weight' => $key ) ;
    }
    
  }
  
  /** Representative image */
  if ( isset ( $spoke->thumbnail_fid ) ) {
    $node['field_attached_images_ri'] = (object) array( 'fid' => $spoke->thumbnail_fid, );
  }

  /**
   * Attached images
   * @var unknown_type
   */
  $node['field_attached_images'] = array( ) ;

  foreach ( attached_images ( $spoke->nid ) as $key => $image ) {
    
    $node['field_attached_images'][] = $image;
    
    if ($image->filename != '_original') $image->skip = TRUE ;
    
  }
  
  create_spoke ( $node ) ;

}