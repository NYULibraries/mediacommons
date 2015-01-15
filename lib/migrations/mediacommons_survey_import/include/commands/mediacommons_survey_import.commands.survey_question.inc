<?php

/**
 * Given an *old* node (Drupal 6), create a frontpage node.
 */
function mediacommons_survey_import_create_survey_question(array $old_node) {

  /**
   * URL to be used as the production URL
   */
  $production_url = mediacommons_base_import_get_production_url();
  
  /**
   * Files folder real path
   */
  $dupal_file_path = drupal_realpath('public://');

  /**
   * This node default configuration 
   */
  $new_node = _mediacommons_base_import_setup_node('hub');
  
  /**
   * Prepare node
   */      
  $old_node = (object) $old_node;
  
  /** 
   * Find out if node is already in the system 
   */
  $node_exist = mediacommons_base_import_node_exists($old_node->onid);
  
  /**
   * Find out if user is already in the system 
   */
  $author_uid = mediacommons_base_import_user_exists($old_node->ouid);
  
  /**
   * Conditions to be meet before creating content
   */

  /**
   * If hub exist, exit
   */
  if ($node_exist) {
    drush_print(
      t('Front page @title already exist.', array('@title' => $old_node->title))
    );
    return;
  }

  /**
   * Do not migrate if we don't have a valid author.
   * This "only fail" if someone try to migrate the content before migrating users, any 
   * other reason, you can assume something is wrong with the script or the datasource.
   */
  if (!$author_uid) {
    drush_print(
      t(
        'Can\'t import spoke "@title" (nid: @nid, uid: @uid) author does not exist in system.', 
        array('@title' => $old_node->title, '@nid' => $old_node->onid, '@uid' => $old_node->ouid)
      )
    );
    return;
  }
  
  /**
   * Conditions meet node migration start
   */
  drush_print(
    t('Migrating "@title".', array('@title' => $old_node->title))
  );

  $path_alias = mediacommons_base_import_find_url_alias('node/' . $old_node->onid);
  
  $new_node->title = $old_node->title;
  
  $new_node->uid = $author_uid;

  $new_node->language = LANGUAGE_NONE;

  $new_node->date = $old_node->date;

  $new_node->status = $old_node->status;

  $new_node->promote = $old_node->promote;

  $new_node->moderate = $old_node->moderate;

  $new_node->sticky = $old_node->sticky;
  
  /** Prepare node for saving */
  if ($new_node = node_submit($new_node)) {
        
    node_save($new_node);
        
    $path = array(
      'source' => 'node/' . $new_node->nid,  // The internal system path.
      'alias' => $path_alias->dst, // The URL alias.
    );

    path_save($path);
        
    mediacommons_base_import_comment($new_node->nid, $old_node->onid);
        
    db_insert('mediacommons_base_import_node_map')->fields(
      array(
        'onid' => $old_node->onid,
        'nid' => $new_node->nid,
      )
    )->execute();
        
    drush_print(
      t('Survey Question node saved.')
    );
      
  }
  
}

function mediacommons_survey_import_migrate_all_survey_question() {

  $data = mediacommons_survey_import_d6_survey_question_list();

  mediacommons_survey_import_migrate_survey_question( $data );

}

function mediacommons_survey_import_migrate_survey_question($data = array()) {

  /**
   * Files folder real path
   */
  $dupal_public_file_path = variable_get('file_public_path', conf_path() . '/files');   

  foreach ($data as $key => $hub) {
      
    /** Prepare node */
    
  	$node = array();

    $node['title'] =  $hub->title;

    $node['ouid'] = $hub->uid;

    $node['onid'] = $hub->nid;

    $node['language'] = (!empty($hub->language)) ? $hub->language : LANGUAGE_NONE;

    $node['date'] = date('Y-m-j H:i:s', $hub->created);

    $node['status'] = $hub->status;

    $node['promote'] = $hub->promote;

    $node['moderate'] = $hub->moderate;

    $node['sticky'] = $hub->sticky;
    
    mediacommons_survey_import_create_survey_question($node);

  }
}