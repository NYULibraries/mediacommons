<?php

/** We don't need no education */
function mediacommons_import_delete_users($roles = array()) {

  /** find users */
  $query = db_query("SELECT DISTINCT uid FROM {users} WHERE uid <> 0 AND uid <> 1", array());

  /** fetch users */
  $users = $query->fetchAll();

  foreach ($users as $user) {
    user_delete($user->uid);
    $num_deleted = db_delete('mediacommons_import_user_map')
    ->condition('uid', $user->uid)
    ->execute();
  }

}


/** Kill 'em all */
function mediacommons_import_delete_content($ype = 'all') {

  $nodes = array();

  switch ($ype) {

    case 'hub' :
      $nodes += mediacommons_import_find_nodes('hub');
      break;

    case 'spoke' :
      $nodes += mediacommons_import_find_nodes('spoke');
      break;

    default : /** same as all */
      $nodes += array_merge(mediacommons_import_find_nodes('hub'), mediacommons_import_find_nodes('spoke'));
  }

  foreach ($nodes as $node) {
    node_delete($node->nid);
  }

  db_delete('mediacommons_import_node_map')
  ->condition('nid', 'kill em all', '<>')
  ->execute();

}

