<?php

function create_hub ( array $old_node ) {

  /**
   * URL to be used as the production URL
   */
  $production_url = mediacommons_base_import_get_production_url() ;

  /**
   * Files folder real path
   */
  $dupal_file_path = drupal_realpath('public://') ;

  /**
   * This node default configuration
   */
  $new_node = _mediacommons_base_import_setup_node('hub') ;

  /**
   * Prepare node
   */
  $old_node = (object) $old_node ;

  /**
   * Do not migrate if we don't have a valid author.
   * This "only fail" if someone try to migrate the content before migrating users, any
   * other reason, you can assume something is wrong with the script or the datasource.
   */
  if ( ! $author_uid = user_exists_by_ouid ( $old_node->ouid ) ) return ;

  /**
   * Find out if node is already in the system
   * If node exists exit
   */
  if ( node_exists ( $old_node->onid ) ) return ;

  /** conditions meet node migration start */

  $path_alias = mediacommons_base_import_find_url_alias('node/' . $old_node->onid);

  $new_node->title = $old_node->title ;

  $new_node->uid = $author_uid ;

  $new_node->language = LANGUAGE_NONE ;

  $new_node->date = $old_node->date ;

  $new_node->status = $old_node->status ;

  $new_node->promote = $old_node->promote ;

  $new_node->moderate = $old_node->moderate ;

  $new_node->sticky = $old_node->sticky ;

  extract_iframe_sources ( $old_node->field_body ) ;

  /** add description */
  $new_node->field_body[$new_node->language][0] = array (
    'value' => $old_node->field_body,
    'format' => 'filtered_html',
  ) ;

  /** Contributors */
  if ( isset ( $old_node->field_contributors ) ) {
    $new_node->field_contributors = array();
    foreach ( $old_node->field_contributors as $key => $contributors ) {
      if ( $uid = user_exists_by_ouid ( $contributors ) ) {
        $new_node->field_contributors[$old_node->language][$key] = array(
          'uid' => $uid,
          '_weight' => $key,
        );
      }
    }
  }

  /** spokes */
  if ( isset ( $old_node->field_spokes ) ) {
    $new_node->field_spokes = array();
    foreach ($old_node->field_spokes as $key => $hub) {
      if ( $nid = nid_by_onid ( $hub ) ) $new_node->field_spokes[$old_node->language][$key]['nid'] = $nid ;
    }
  }

  /** dates */
  if ( isset ( $old_node->field_period_1 ) ) {
    $new_node->field_period[$old_node->language][0]['value'] = $old_node->field_period_1;
  }

  if ( isset ( $old_node->field_period_2 ) ) {
    $new_node->field_period[$old_node->language][0]['value2'] = $old_node->field_period_2;
  }

  /** attached representative image */
  if ( isset ( $old_node->field_representative_image ) ) {
  	if ( isset ( $old_node->field_representative_image->fid ) ) {
  	  $fid = mediacommons_base_import_get_file_fid_by_ofid ( $old_node->field_representative_image->fid ) ;
  	  $field_representative_image_object = file_load((int) $fid);
      if ( $field_representative_image_object ) {
      	$new_node->field_representative_image[$old_node->language][] = (array) file_load ( $fid ) ;
      }
    }
  }

  /** pubstat */
  if (isset($old_node->field_pubstat)) {
    $new_node->field_pubstat[$old_node->language][0] = array( 'value' => $old_node->field_pubstat ) ;
  }

  /** Tags */
  if (isset($old_node->field_tags)) {
    foreach ($old_node->field_tags as $tag_id) {
      $tag = mediacommons_base_import_find_term_id($tag_id);
      if ($tag) {
        $new_node->field_taxonomy[$new_node->language][] = array('tid' => $tag);
      }
    }
  }

  /** add extended fields (fields that are not part of mediacommons_base distribution) */

  /** function to be call */
  $call = $settings['script_path']['filename'] . '_extend_fields';

  /** If function exists call it */
  if ( function_exists ( $call ) ) {
    $fields = $call();
    foreach ($fields as $field) {
      if (isset($old_node->$field)) {
        $new_node->$field = $old_node->$field;
      }
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

    db_insert('mediacommons_base_import_node_map')->fields (
      array (
        'onid' => $old_node->onid,
        'nid' => $new_node->nid,
      )
    )->execute() ;

    drush_log( t('Hub @title" saved.', array('@title' => $old_node->title)) , 'ok');

  }

}

function mediacommons_base_import_create_hub ( array $old_node ) {
  create_hub ( $old_node  ) ;
}
