<?php

function migrate_responses () {
  foreach (responses () as $key => $entity) migrate_response($entity);
}

function migrate_response ( $entity ) {

  $nid = nid_by_onid ( $entity->nid ) ;

  $pid = nid_by_onid ( $entity->pid ) ;

  $uid = user_exists_by_ouid ( $entity->uid ) ;

  if ( ! $nid || ! $uid ) return ;

  if ( ! $pid) $pid = 0 ;

    $new_comment = (object) array(
      'nid' => $nid,
      'cid' => 0,
      'pid' => $pid,
      'uid' => $uid,
      'mail' => $entit->mail,
      'is_anonymous' => 0,
      'homepage' =>  $entit->homepage,
      'status' => ( $entit->status == 0 ) ? 1 : 0,
      'subject' => mediacommons_base_migrate_truncate ( $entity->title , 60 ),
      'language' => LANGUAGE_NONE,
      'thread' => $entity->thread,
      'hostname' => $entity->hostname,
      'date' => date( "Y-m-j H:i:s" , $entity->timestamp ),
      'comment_body' => array(
        LANGUAGE_NONE => array(
          0 => array (
            'value' => $entity->body,
            'format' => 'full_html',
          ),
        ),
      ),
    );

    comment_submit ( $new_comment ) ;

    comment_save ( $new_comment ) ;

    db_insert('mediacommons_base_import_comments_map')->fields( array ( 'ocid' => (int)$entity->cid, 'cid' => (int)$new_comment->cid ) )->execute() ;

}
