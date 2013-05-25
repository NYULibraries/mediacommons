<?php

/** Yar, maties, to-day we sail with Virgilio" */
function mediacommons_imr_import_delete_alias() {
  db_delete('url_alias')
    ->condition('pid', 'kill em all', '<>')
    ->execute();
}

/** Peer wha'? Shut up! No one cares */
function mediacommons_imr_import_delete_comments() {

  db_delete('mediacommons_imr_import_comments_map')
    ->condition('cid', 'kill em all', '<>')
    ->execute();

  db_delete('comment')
    ->condition('cid', 'kill em all', '<>')
    ->execute();    
    
  db_delete('node_comment_statistics')
    ->condition('cid', 'kill em all', '<>')
    ->execute();        

  db_delete('field_revision_comment_body')
    ->condition('entity_id', 'kill em all', '<>')
    ->execute();        
    
  db_delete('field_data_comment_body')
    ->condition('entity_id', 'kill em all', '<>')
    ->execute();        
    
  db_update('node')
    ->fields(array(
      'comment' => 0,
    )
  )
  ->condition('title', 'kill em all', '<>')
  ->execute();

  db_update('node_revision')
    ->fields(array(
      'comment' => 0,
    )
  )
  ->condition('title', 'kill em all', '<>')
  ->execute();

}

/** We don't need no education */
function mediacommons_imr_import_delete_users($roles = array()) {

  /** find users */
  $query = db_query("SELECT DISTINCT uid FROM {users} WHERE uid <> 0 AND uid <> 1", array());

  /** fetch users */
  $users = $query->fetchAll();

  foreach ($users as $user) {
    user_delete($user->uid);
    $num_deleted = db_delete('mediacommons_imr_import_user_map')
    ->condition('uid', $user->uid)
    ->execute();
  }

}

/** Kill 'em all */
function mediacommons_imr_import_delete_post($ype = 'all') {
  foreach (mediacommons_imr_import_find_nodes('spoke') as $node) {
    node_delete($node->nid);
    db_delete('mediacommons_imr_import_node_map')
      ->condition('nid', $node->nid, '=')
      ->execute();      
  }
}

/** Kill 'em all */
function mediacommons_imr_import_delete_hubs() {
  foreach (mediacommons_imr_import_find_nodes('hub') as $node) {
    node_delete($node->nid);
    db_delete('mediacommons_imr_import_node_map')
      ->condition('nid', $node->nid, '=')
      ->execute();  
  }
}

/** Kill 'em all */
function mediacommons_imr_import_delete_content($ype = 'all') {

  $nodes = array();

  switch ($ype) {

    case 'hub' :
      $nodes += mediacommons_imr_import_find_nodes('hub');
      break;

    case 'spoke' :
      $nodes += mediacommons_imr_import_find_nodes('spoke');
      break;

    default : /** same as all */
      $nodes += array_merge(mediacommons_imr_import_find_nodes('hub'), mediacommons_imr_import_find_nodes('spoke'));
  }

  foreach ($nodes as $node) {
    node_delete($node->nid);
  }

  db_delete('mediacommons_imr_import_node_map')
    ->condition('nid', 'kill em all', '<>')
    ->execute();
  
  db_delete('mediacommons_imr_import_term_map')
    ->condition('tid', 'kill em all', '<>')
    ->execute();  
  
  db_delete('taxonomy_term_data')
    ->condition('tid', 'kill em all', '<>')
    ->execute();

  db_delete('taxonomy_term_hierarchy')
    ->condition('tid', 'kill em all', '<>')
    ->execute();
    
  db_delete('taxonomy_term_hierarchy')
    ->condition('tid', 'kill em all', '<>')
    ->execute();
    
  db_delete('mediacommons_imr_import_vocabulary_map')
    ->condition('vid', 'kill em all', '<>')
    ->execute();

  db_delete('taxonomy_vocabulary')
    ->condition('vid', 'kill em all', '<>')
    ->execute();

}

