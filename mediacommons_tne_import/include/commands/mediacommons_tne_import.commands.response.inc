<?php

function mediacommons_tne_import_migrate_all_response() {
  
  /**
   * Load all clusters
   */
  mediacommons_tne_import_migrate_response(
    mediacommons_tne_import_d6_response_list()
  );
}

function mediacommons_tne_import_migrate_response($data = array()) {

  foreach ($data as $response) {
    
    $d7Nid = mediacommons_base_import_node_nid($response->nid);

    $d7Uid = mediacommons_base_import_user_exists($response->uid);

    $d7Pid = mediacommons_base_import_node_nid($response->pid);

    if (!$d7Nid || !$d7Uid) { continue; }

    if (!$d7Pid) { $d7Pid = 0; }
    
    $subject = mediacommons_base_migrate_truncate($response->title, 60);

    $new_comment = (object) array(
      'nid' => $d7Nid,
      'cid' => 0,
      'pid' => $d7Pid,
      'uid' => $d7Uid,
      'mail' => $response->mail,
      'is_anonymous' => 0,
      'homepage' =>  $response->homepage,
      'status' => ($response->status == 0) ? 1 : 0,
      'subject' => $subject,
      'language' => LANGUAGE_NONE,
      'thread' => $response->thread,
      'hostname' => $response->hostname,
      'date' => date("Y-m-j H:i:s", $response->timestamp),
      'comment_body' => array(
        LANGUAGE_NONE => array(
          0 => array (
            'value' => $response->body,
            'format' => 'full_html',
          ),
        ),
      ),
    );
    
    comment_submit($new_comment);
    comment_save($new_comment);
      
    db_insert('mediacommons_base_import_comments_map')
      ->fields(array(
        'ocid' => (int)$response->cid,
        'cid' => (int)$new_comment->cid,
      )
    )->execute();
  }

}