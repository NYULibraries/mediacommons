<?php

function mediacommons_base_import_comment ($nid, $oldNid) {

  $nodes_records = $users_records = $comments_record = array();

  $users_query = 'SELECT * FROM {mediacommons_base_import_user_map}';

  $users = db_query($users_query);

  $query = 'SELECT * FROM {comments} WHERE nid = :nid';

  $comments = Database::getConnection('default', 'drupal6')->query($query, array(':nid' => $oldNid), array());

  foreach ($users as $user) {
    $users_records[$user->ouid] = $user->uid;
  }

  foreach ($comments as $comment) {
    $new_comment = (object) array(
      'nid' => $nid,
      'cid' => 0,
      'pid' => 0, // need to fix this so that the parent of the comment get assigned
      'uid' => $users_records[$comment->uid],
      'mail' => $comment->mail,
      'is_anonymous' => 0,
      'homepage' =>  $comment->homepage,
      // http://api.drupal.org/api/drupal/modules%21comment%21comment.module/constant/COMMENT_PUBLISHED/6
      // http://api.drupal.org/api/drupal/modules%21comment%21comment.module/constant/COMMENT_PUBLISHED/7
      'status' => ($comment->status == 0) ? 1 : 0,
      'subject' => $comment->subject,
      'language' => LANGUAGE_NONE,
      'thread' => $comment->thread,
      'hostname' => $comment->hostname,
      'date' => date("Y-m-j H:i:s", $comment->timestamp),
      'comment_body' => array(
        LANGUAGE_NONE => array(
          0 => array (
            'value' => $comment->comment,
            'format' => 'filtered_html',
          ),
        ),
      ),
    );

    comment_submit($new_comment);
    comment_save($new_comment);

    $comments_record[] = array(
      'old' => $comment,
      'new' => $new_comment
    );

    db_insert('mediacommons_base_import_comments_map')
      ->fields(array(
        'ocid' => (int)$comment->cid,
        'cid' => (int)$new_comment->cid,
      )
    )->execute();

  }

}
