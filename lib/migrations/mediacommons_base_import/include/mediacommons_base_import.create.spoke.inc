<?php

function create_spoke($old_node) {

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
  $new_node = _mediacommons_base_import_setup_node('spoke');

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

  $new_node->sticky = $old_node->sticky;

  if (isset($old_node->field_body)) {
    $new_node->field_body[$new_node->language][0] = array ( 'value' => $old_node->field_body, 'format' => 'filtered_html' ) ;
  }

  /** attached representative image */
  if (isset ( $old_node->field_attached_images_ri)) {
    $fid = mediacommons_base_import_get_file_fid_by_ofid ( $old_node->field_attached_images_ri->fid ) ;
    if ($fid) {
      $_file = (array) file_load ($fid);
      $new_node->field_attached_images[$new_node->language][] = $_file;
      $new_node->field_representative_image[$new_node->language][] = $_file;
    }
  }

  /** attached all other images */
  if ( isset ( $old_node->field_attached_images )  && ! empty ( $old_node->field_attached_images ) ) {
    foreach ( $old_node->field_attached_images as $image ) {
    	if ( is_object ( $image ) ) $fid = mediacommons_base_import_get_file_fid_by_ofid ( $image->fid ) ;
      // ideally we just want this
    	else {
    		if ( $fid = fid_by_ofid  ( $image ) ) $new_node->field_attached_images[$new_node->language][] = (array) file_load ( $fid->fid ) ;
    	}
    }
  }

  // hardcoded here until I find a better solutions
  // aof

  // See: https://jira.nyu.edu/browse/MC-156
  // this is the string that match most of the terms in
  // the table mediacommons_base_import_term_map
  // ALT-AC and TNE share the taxonomy so they will
  // match 'drupal6' as the project
  // IMR and [in]transition don't share this taxonomy_get_vocabularies
  // and we need to map the toid using the project so that the
  // taxonomy don't get mixed up
  $code = 'drupal6';

  $project_code = $settings['mediacommons']['project'];

  if ($project_code == 'imr') {
    $code = 'imr6';
  }
  else if ($project_code == 'intransition') {
    $code = 'int6';
  }

  // end hack

  /** Tags */
  if (isset($old_node->field_tags)) {
    foreach ($old_node->field_tags as $tag_id) {
      // See: https://jira.nyu.edu/browse/MC-156
      $tag = mediacommons_base_import_find_term_id($tag_id, $code);
      if ($tag) {
        $new_node->field_taxonomy[$new_node->language][] = array('tid' => $tag);
      }
    }
  }

  if ( isset ( $old_node->field_part_of ) ) {
    foreach ( (array) $old_node->field_part_of as $part_of ) {
      if ( $nid = nid_by_onid ( $part_of ) ) $new_node->field_part_of[$new_node->language][0]['nid'] = $nid ;
    }
  }

  /**
   * Contributors 2 steps
   * 1. Add the author of the spoke as a contributor
   * 2. If we have contributors, add them to the field
   */
  $new_node->field_contributors = array (
    $new_node->language => array(),
  );

  /**
   * Step 1. The author is always the first contributor (at least for the migration)
  */
  $new_node->field_contributors[$new_node->language][0] = array (
    'uid' => $new_node->uid,
    '_weight' => 0,
  ) ;

  if ( isset ( $old_node->field_contributors ) ) {
    foreach ( $old_node->field_contributors as $key => $contributor ) {
      if ( $uid = user_exists_by_ouid ($contributor ) ) {
        $new_node->field_contributors[$old_node->language][] = array (
          'uid' => $uid,
          '_weight' => (int) $key,
        );
      }
    }
  }

  $new_node->field_pubstat[$old_node->language][0] = array ( 'value' => $old_node->field_pubstat ) ;

  /**
   * Add extended fields (fields that are not part of mediacommons_base distribution)
   */

  /** Function to be call */
  $call = $settings['script_path']['filename'] . '_extend_fields';

  /** If function exists call it */
  if ( function_exists ( $call ) ) {
    $fields = $call();
    foreach ( $fields as $field ) {
      if ( isset ( $old_node->$field ) ) $new_node->$field = $old_node->$field ;
    }
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

    mediacommons_base_import_comment ( $new_node->nid, $old_node->onid ) ;

    db_insert('mediacommons_base_import_node_map')->fields ( array ( 'onid' => $old_node->onid, 'nid' => $new_node->nid ) )->execute() ;

    drush_log ( t('"@title" successfully migrated. ', array ( '@title' => $old_node->title ) ) , 'ok') ;

  }

}
