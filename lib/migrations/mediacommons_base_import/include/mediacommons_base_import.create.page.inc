<?php

/**
 * Migrate page nodes.
 */
function migrate_pages ( ) {
	foreach ( pages () as $key => $old_node ) migrate_page ( $old_node ) ;
}

/**
 * Migrate pages.
 */
function migrate_page ( $old_node ) {

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

	extract_iframe_sources ( $old_node->body ) ;

	$node['field_body'] = $old_node->body ;

	create_page ( $node ) ;

}


function create_page ( $old_node ) {

  global $settings;

  /**
   * URL to be used as the production URL
   */
  $production_url = mediacommons_base_import_get_production_url () ;

  /**
   * Files folder real path
   */
  $dupal_file_path = drupal_realpath('public://');

  /**
   * This node default configuration
   */
  $new_node = _mediacommons_base_import_setup_node( 'page' ) ;

  /**
   * Prepare node
   */
  $old_node = (object) $old_node ;

  /**
   * Find out if node is already in the system
   * If node exists exit
   */
  $node_exists = nid_by_onid ( $old_node->onid ) ;

  /**
   * Find out if user is already in the system
   */
  $author_uid = user_exists_by_ouid ( $old_node->ouid ) ;

  if ( $node_exists || ! $author_uid ) return FALSE ;

  $path_alias = url_alias_by_onid ( $old_node->onid ) ;

  $new_node->title = $old_node->title ;

  $new_node->uid = $author_uid ;

  $new_node->language = $old_node->language ;

  $new_node->date = $old_node->date ;

  $new_node->status = $old_node->status ;

  $new_node->promote = $old_node->promote ;

  $new_node->moderate = $old_node->moderate ;

  $new_node->sticky = $old_node->sticky ;

  if ( isset ( $old_node->field_body ) ) {
    $new_node->body[$new_node->language][0] = array ( 'value' => $old_node->field_body, 'format' => 'filtered_html' ) ;
  }

  /**
   * Prepare node for saving
   */
  if ( $new_node = node_submit ( $new_node ) ) {

    node_save ( $new_node ) ;

    $path = array (
      'source' => 'node/' . $new_node->nid,  // The internal system path.
      'alias' => $path_alias->dst, // The URL alias.
    ) ;

    path_save ( $path ) ;

    db_insert('mediacommons_base_import_node_map')->fields ( array ( 'onid' => $old_node->onid, 'nid' => $new_node->nid ) )->execute() ;

    drush_log ( t('"@title" successfully migrated. ', array ( '@title' => $old_node->title ) ) , 'ok') ;

  }

}
