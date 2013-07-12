<?php

function mediacommons_tne_migrate_response() {

  $nodes_records = $users_records = $comments_record = array();
  
  $responses = mediacommons_tne_import_d6_response_list($oldNid);

  // http://api.drupal.org/api/drupal/modules%21comment%21comment.module/constant/COMMENT_PUBLISHED/6
  // http://api.drupal.org/api/drupal/modules%21comment%21comment.module/constant/COMMENT_PUBLISHED/7
  
  foreach ($responses as $response) {
  	
  	$d7Nid = mediacommons_base_import_node_nid($response->nid);
  	
  	$d7Uid = mediacommons_base_import_user_exists($response->uid);
  	
  	$d7Pid = mediacommons_base_import_node_nid($response->pid);
  	
    if (!$d7Nid || !$d7Uid) {
      continue;
    }
  	
  	if (!$d7Pid) {
  	  $d7Pid = 0;
  	}
  	
  	$subject =  mediacommons_base_migrate_truncate($response->title, 60);
  	
    $new_comment = (object) array(
      'nid' => $d7Nid,
      'cid' => 0,
      'pid' => $d7Pid,
      'uid' => $d7Uid,
      'mail' => $response->mail,
      'is_anonymous' => 0,
      'homepage' =>  $response->homepage,
      'status' => ($comment->status == 0) ? 1 : 0,
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

/**
 *
 */
function mediacommons_tne_import_migrate_contributed_pieces() {

  /**
   * Load data, this will take time; go get yourself a coffee 
   */
  $data = mediacommons_tne_import_d6_contributed_pieces_list();
  
  /**
   * Files folder real path
   */
  $dupal_public_file_path = variable_get('file_public_path', conf_path() . '/files');   

  /** 
   * If we have $data proceed 
   */
  if (isset($data)) {
  	
    foreach ($data as $key => $spoke) {
    	
      $path_alias = mediacommons_base_import_find_url_alias('node/' . $spoke->nid);

      /** 
       * Find out if node is already in the system 
       */
      $node_exist = mediacommons_base_import_node_exists($spoke->nid);

      /**
       * Find out if user is already in the system 
       */
      $author_uid = mediacommons_base_import_user_exists($spoke->uid);

      /**
       * Conditions to be meet before creating content
       */
      if ($node_exist) {
        drush_print(
          t(
            'Spoke "@title" already exist.', 
            array('@title' => $spoke->title)
          )
        );
        continue;
      }

      if (!$author_uid) {
        drush_print(
          t("Can't import spoke @title author does not exist in system. Please make sure the author is created before creating content under his username", array('@title' => $spoke->title))
        );
        continue;
      }
      
      /**
       * Prepare node
       */
      $node = array();

      $node['title'] = $spoke->title;

      $node['ouid'] = $spoke->uid;

      $node['onid'] = $spoke->nid;

      $node['language'] = (!empty($spoke->language)) ? $spoke->language : LANGUAGE_NONE;

      $node['date'] = date('Y-m-j H:i:s', $spoke->created);

      $node['status'] = $spoke->status;

      $node['promote'] = $spoke->promote;

      $node['moderate'] = $spoke->moderate;

      $node['sticky'] = $spoke->sticky;

      if (isset($spoke->field_document_textarea_body_value)) {
        $node['field_body'] = $spoke->field_document_textarea_body_value;
      }

      $node['field_pubstat'] = $spoke->status;
      
      
      if (isset($spoke->field_tagline)) {
        $node['field_tagline'] = $spoke->field_tagline;
      }
      
      foreach (explode(', ', $spoke->terms) as $tid) {
        $node['field_tags'][] = $tid;
      }
      
      /** Additonal authors */
      if ($spoke->additonal_authors) {
      	
      	$node['field_contributors'] = array();
          
        /** Contributors */
        $spoke_contributors = explode(', ', $spoke->additonal_authors);

        /** Contributor order */
        $contributor_order = explode(', ', $spoke->additonal_authors_delta);

        foreach ($spoke_contributors as $key => $contributor) {
          $node['field_contributors'][$key] = array(
            'uid' => $contributor,
            '_weight' => (int)$contributor_order[$key],
          );
        }
      }
      
      /** 
       * Representative image
       */
      if (isset($spoke->thumbnail_fid)) {
        $node['field_attached_images_ri'] = $spoke->thumbnail_fid;
      }
      
      /**
       * Attached images
       * @var unknown_type
       */
      $node['field_attached_images'] = array();
      
      $attached_images = mediacommons_tne_import_d6_attached_images($spoke->nid);
      
      foreach ($attached_images as $key => $image) {
        $node['field_attached_images'][] = $image;          
        $find = str_replace("/", "\/", $image->filepath);          
        $pattern = "/" . $find . "/i";
        preg_match($pattern, $node['field_body'], $matches);
          
        if ($matches) {
          $node['field_body'] = str_replace($matches[0], $dupal_public_file_path . '/' . basename($image->filepath), $node['field_body']);
          unset($matches);  
        }
        
        if ($image->filename != '_original') {
          $image->skip = TRUE;
        }
        
      }
      
      mediacommons_base_import_create_spoke($node);

    }
  }

  drush_print(
    t('Done.')
  );

}

function mediacommons_tne_import_migrate_cluster() {

}

function mediacommons_tne_import_migrate_response() {

}

function mediacommons_tne_import_test() {
	
}