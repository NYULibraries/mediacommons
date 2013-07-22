<?php

function mediacommons_base_import_generate_all_users() {
  
  drush_print(
    t("Importing users: This will take time, go get yourself a coffee.")
  );
  
  foreach (mediacommons_base_import_d6_users_list() as $user) {
    mediacommons_base_import_generate_user($user);
  }
}

function mediacommons_base_import_generate_roles() {
  $roles_d7 = $roles_d6_list = array();
  
  $roles_d6_list = mediacommons_base_import_d6_roles();
  
  foreach(mediacommons_base_import_d7_roles() as $role) {
    $roles_d7[$role->name] = $role->rid;
  }

  foreach ($roles_d6_list as $record) {
  
    $rid = db_query("SELECT rid FROM {mediacommons_base_import_role_map} WHERE orid = :orid", array(':orid' => $record->rid));
    
    if (!$rid->rowCount()) {
       drush_print_r('Role exist in Drupal 7');        
      /** Role exist in Drupal 7 */
      if (array_key_exists($record->name, $roles_d7)) {
        drush_print('Role ' . $record->name . ' already exist');
        db_insert('mediacommons_base_import_role_map')
          ->fields(array(
            'orid' => $record->rid,
            'rid' => $roles_d7[$record->name]
          )
        )->execute();
      }
  
      /** Role does not exist in Drupal 7 */
      else {
        
        drush_print_r('Creating role ' . $record->name);
        $role = new stdClass();
        $role->name = $record->name;
        $new = user_role_save($role);
        $r = user_role_load_by_name($role->name);
        db_insert('mediacommons_base_import_role_map')
          ->fields(array(
            'orid' => $record->rid,
            'rid' => $r->rid,
          )
        )->execute();
      }
    }
    else {
      drush_print('Role "' . $record->name . '" already imported');
    }
  }
}

function mediacommons_base_import_generate_vocabulary() {

  $d7 = mediacommons_base_import_d7_vocabulary_list();
  $d6 = mediacommons_base_import_d6_vocabulary_list();
  $d7_names = array();

  $query = 'SELECT vid FROM {mediacommons_base_import_vocabulary_map} WHERE vuid = :vuid';
  
  foreach ($d7 as $record) { 
    $d7_names[strtolower($record->name)] = $record->vid;
  }
  
  foreach ($d6 as $record) {
    $vid = db_query($query, array(':vuid' => $record->vid));
    if (!$vid->rowCount()) {
      /** Vocabulary exist in Drupal 7 */
      if (array_key_exists(strtolower($record->name), $d7_names)) {
        drush_print('Vocabulary ' . strtolower($record->name) . ' already exist');
        db_insert('mediacommons_base_import_vocabulary_map')
          ->fields(array(
            'vuid' => $record->vid,
            'vid' => $d7_names[strtolower($record->name)],
          )
        )->execute();
      }
      /** Vocabulary does not exist in Drupal 7 */
      else {
        drush_print('Creating ' . $record->name);
        mediacommons_base_import_create_taxonomy_vocabulary($record);
      }
    }
  }
}

function mediacommons_base_import_generate_terms() {
  
  /** Just in case */
  mediacommons_base_import_generate_vocabulary();
  
  $vocabulary_map = db_query("SELECT * FROM {mediacommons_base_import_vocabulary_map}");
  $terms_d6 = mediacommons_base_import_d6_term_list();
  
  $vocabularies = array();
  
  foreach ($vocabulary_map as $vocabulary) {
    $vocabularies[$vocabulary->vuid] = $vocabulary->vid;
  }
  
  foreach ($terms_d6 as $term) {
    if (isset($vocabularies[$term->vid])) {
      $tid = db_query("SELECT tid FROM {mediacommons_base_import_term_map} WHERE toid = :toid", array('toid' => $term->tid));
      if (!$tid->rowCount()) {
        mediacommons_base_import_create_taxonomy_term($term);
      }
    }
  }
}

/**
 * Given an old node, create a spoke.
 */
function mediacommons_base_import_create_spoke(array $old_node) {
	
  global $settings;
  
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
  $new_node = _mediacommons_base_import_setup_node('spoke');
  
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

  if ($node_exist) {
    drush_print(
      t('Spoke @title already exist.', array('@title' => $old_node->title))
    );
    return;
  }
      
  if (!$author_uid) {
    drush_print(
      t("Can't import spoke @title author does not exist in system. Please make sure the author is created before creating content under his username.", array('@title' => $old_node->title))
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

  $new_node->language = $old_node->language;

  $new_node->date = $old_node->date;

  $new_node->status = $old_node->status;

  $new_node->promote = $old_node->promote;

  $new_node->moderate = $old_node->moderate;

  $new_node->sticky = $old_node->sticky;
  
  if (isset($old_node->field_body)) {
  	// $new_node->field_body[$old_node->language][0] = array( not working for TNE; there is only 5 ES nodes; what should we do?
  	
    $new_node->field_body[LANGUAGE_NONE][0] = array(
      'value' => $old_node->field_body,
      'format' => 'full_html', // should this be filtered_html?
      // 'format' => 'filtered_html',
    );
  }

  /** 
   * Attached representative image
   * @TODO: Make modifications to IMR
   */    
  if (isset($old_node->field_attached_images_ri)) {
  	
    $ri = mediacommons_base_import_find_image($old_node->field_attached_images_ri->fid);
    
    $url_image_path = url($production_url . $ri->filepath, array('absolute' => TRUE));

    $source_image_path = $dupal_file_path . '/' . $ri->filename;
    
    $put_success = file_put_contents($source_image_path, mediacommons_base_import_get_content($url_image_path));
    
    if ($put_success) {    
      $info = image_get_info($source_image_path);
      if (is_array($info)) {
        $file = (object) array(
          'uid' => (int)$new_node->uid, // change this to this user ID?
          'uri' => $source_image_path,
          'filemime' => file_get_mimetype($source_image_path),
          'status' => 1,
          'ri' => 1,
          'cropbox_x' => 0,
          'cropbox_y' => 0,
          'cropbox_height' => 0,
          'cropbox_width' => 0,
        );

        $file = file_copy($file, 'public://');        
        $new_node->field_attached_images[$old_node->language][] = (array)$file;      
      }      
    }
  }
  
  /**
   * Attached all other images
   * @TODO: Make modifications to IMR (maybe not)
   */
  if (isset($old_node->field_attached_images)) {

    foreach ($old_node->field_attached_images as $image) {
    	
      $url_image_path = url($production_url . $image->filepath, array('absolute' => TRUE));

      $source_image_path = $dupal_file_path . '/' . basename($url_image_path);
      
      if (!file_exists($source_image_path)) {
        $put_success = file_put_contents($source_image_path, file_get_contents($url_image_path));
      }
      // file already exist in file directory
      else {
        $put_success = TRUE;
      }
      
      if ($put_success && !isset($image->skip)) {
      	
        $file = (object) array(
          'uid' => (int)$new_node->uid,
          'uri' => $source_image_path,
          'filemime' => file_get_mimetype($source_image_path),
          'status' => 1,
          'ri' => 1,
          'cropbox_x' => 0,
          'cropbox_y' => 0,
          'cropbox_height' => 0,
          'cropbox_width' => 0,
        );
        
        $file = file_copy($file, 'public://');        
        $new_node->field_attached_images[$old_node->language][] = (array)$file;        
      }
    }
  }
  
  /** Tags */
  if (isset($old_node->field_tags)) {
    foreach ($old_node->field_tags as $tag_id) {
      if ($tag = mediacommons_base_import_find_term_id($tag_id)) {
        $new_node->field_tags[$old_node->language][] = array('tid' => $tag);
      }
    }
  }
  
  // for now let the cluster do the work
  if (isset($old_node->field_part_of)) {}
  
  /**
   * Contributors 2 steps
   * 1. Add the author of the spoke as a contributor
   * 2. If we have contributors, add them to the field
   */
  $new_node->field_contributors = array(
    $new_node->language => array(),
  );

  /** 
   * Step 1. The author is always the first contributor (at least for the migration) 
   */
  $new_node->field_contributors[$new_node->language][0] = array(
    'uid' => $new_node->uid,
    '_weight' => 0,
  );

  /** 
   * Step 2. Work on this when when we have this case 
   */  
  
  $new_node->field_pubstat[$old_node->language][0] = array(
    'value' => $old_node->field_pubstat,
  );
  
  /**
   * Add extended fields (fields that are not part of mediacommons_base distribution)
   */
  
  // Function to be call
  $call = $settings['script_path']['filename'] . '_extend_fields';
  
  // If function exists call it
  if (function_exists($call)) {
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
  if ($new_node = node_submit($new_node)) {
        
    node_save($new_node);
        
    $path = array(
      'source' => 'node/' . $new_node->nid,  // The internal system path.
      'alias' => $path_alias->dst, // The URL alias.
    );
        
    path_save(&$path);
        
    mediacommons_base_import_comment($new_node->nid, $old_node->onid);
        
    db_insert('mediacommons_base_import_node_map')->fields(
      array(
        'onid' => $old_node->onid,
        'nid' => $new_node->nid,
      )
    )->execute();
        
    drush_print(
      t('Spoke saved.')
    );
      
  }
  
}

/**
 * Given an *old* node (Drupal 6), create a hub.
 */
function mediacommons_base_import_create_hub(array $old_node) {

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
      t('Hub @title already exist.', array('@title' => $old_node->title))
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

  // $new_node->language = $old_node->language;
  
  $new_node->language = LANGUAGE_NONE; // for now

  $new_node->date = $old_node->date;

  $new_node->status = $old_node->status;

  $new_node->promote = $old_node->promote;

  $new_node->moderate = $old_node->moderate;

  $new_node->sticky = $old_node->sticky;
  
  /** 
   * Add description 
   */
  $new_node->field_body[$new_node->language][0] = array(
    'value' => $old_node->field_body,
    'format' => 'full_html',
  );
        
  /** 
   * Media type 
   * Hubs no longer have media type
   */
  //if (isset($old_node->field_type)) {
  //  $new_node->field_type[$old_node->language][0]['value'] = $old_node->field_type;
  //}
      
  /**
   * Tags 
   * Hubs no longer have tags
   */
  //if (isset($old_node->field_tags)) {
    //$new_node->field_tags = array();
    //foreach ($old_node->field_tags as $tag_id) {
      //if ($tag = mediacommons_base_import_find_term_id($tag_id)) {
        //$new_node->field_tags[$old_node->language][] = array('tid' => $tag);
      //}
    //}
  //}
  
  /**
   * Contributors
   */
  if (isset($old_node->field_contributors)) {
    $new_node->field_contributors = array();
    foreach ( $old_node->field_contributors as $key => $contributors ) {
      $uid = mediacommons_base_import_user_exists($contributors);
      if ($uid) {
        $new_node->field_contributors[$old_node->language][$key] = array(
          'uid' => $uid,
          '_weight' => $key, 
        );        
      }
    }
  }
  
  /**
   * Spokes
   */   
  if (isset($old_node->field_spokes)) {
    $new_node->field_spokes = array();
    foreach ($old_node->field_spokes as $key => $hub) {
      $nid = mediacommons_base_import_node_nid($hub);
      if ($nid) {
        $new_node->field_spokes[$old_node->language][$key]['nid'] = $nid;
      }
    }
  }   

  /**
   * Dates
   */
  if (isset($old_node->field_period_1)) {
    $new_node->field_period[$old_node->language][0]['value'] = $old_node->field_period_1; 
  }
   
  if (isset($old_node->field_period_2)) {
    $new_node->field_period[$old_node->language][0]['value2'] = $old_node->field_period_2; 
  }
  
  /** 
   * Attached representative image
   */
  if (isset($old_node->field_representative_image)) {
  	
  	if (isset($old_node->field_representative_image->fid)) {
  
      $ri = mediacommons_base_import_find_image($old_node->field_representative_image->fid);

      // The path of the image we will try to download
      $url_image_path = url($production_url . $ri->filepath, array('absolute' => TRUE));

      // The path to save the image if 'put' is successful 
      $source_image_path = $dupal_file_path . '/' . $ri->filename;

    }
    
    elseif (isset($old_node->field_representative_image->source)) {

    	
      // The path of the image we will try to download
      $url_image_path = $old_node->field_representative_image->source;

      // The path to save the image if 'put' is successful 
      $source_image_path = $dupal_file_path . '/' . basename($old_node->field_representative_image->source);
      
    }

    $put_success = file_put_contents($source_image_path, mediacommons_base_import_curl($url_image_path));
    
    if ($put_success) {

      $file = (object) array(
        'uid' => (int)$new_node->uid,
        'uri' => $source_image_path,
        'filemime' => file_get_mimetype($source_image_path),
        'status' => 1,
        'cropbox_x' => 0,
        'cropbox_y' => 0,
        'cropbox_height' => 0,
        'cropbox_width' => 0,
      );

      $file = file_copy($file, 'public://');

      $new_node->field_representative_image[$old_node->language][] = (array)$file;

    }    
  }
  
  /** 
   * Pubstat
   */
  if (isset($old_node->field_pubstat)) {  
    $new_node->field_pubstat[$old_node->language][0] = array(
      'value' => $old_node->field_pubstat,
    );
  }

  /**
   * Add extended fields (fields that are not part of mediacommons_base distribution)
   */
  
  // Function to be call
  $call = $settings['script_path']['filename'] . '_extend_fields';
  
  // If function exists call it
  if (function_exists($call)) {
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
  if ($new_node = node_submit($new_node)) {
        
    node_save($new_node);
        
    $path = array(
      'source' => 'node/' . $new_node->nid,  // The internal system path.
      'alias' => $path_alias->dst, // The URL alias.
    );

    path_save(&$path);
        
    mediacommons_base_import_comment($new_node->nid, $old_node->onid);
        
    db_insert('mediacommons_base_import_node_map')->fields(
      array(
        'onid' => $old_node->onid,
        'nid' => $new_node->nid,
      )
    )->execute();
        
    drush_print(
      t('Spoke saved.')
    );
      
  }
  
}

/**
 * Create a hubs.
 */
function mediacommons_base_import_generate_hubs() {
  
  global $settings;
  
  $production_url = $settings['settings']['production']['url'];
  
  $dupal_file_path = drupal_realpath('public://');

  $data = mediacommons_base_import_d6_theme_week_list();
  
  /** If we have $data proceed */
  if ($data->rowCount()) {

    foreach ($data as $key => $value) {
    
      $old_author_uid = (int) $value->uid;
      
      /** Find out if user is already in the system */
      $author_exist = db_query('SELECT uid FROM {mediacommons_base_import_user_map} u WHERE ouid = :ouid', array(':ouid' => $old_author_uid));
      
      $authorCount = (int)$author_exist->rowCount();
      
      /** Find out if node is already in the system */
      $node_exist = db_query('SELECT nid FROM {mediacommons_base_import_node_map} u WHERE onid = :onid', array(':onid' => $value->nid));
      
      $rowCount = (int)$node_exist->rowCount();
      
      if (
          $rowCount == 0 &&
          $authorCount > 0 &&
          $old_author_uid > 0
      ) {
        
        drush_print('Importing ' . $value->title . "\n");        
        
        /** This node defualt configuration */
        $node = _mediacommons_base_import_setup_node('hub');
        
        /** Node title */
        $node->title = $value->title;

        /** Node status */
        $node->status = $value->status;
        
        /** Node created and changed */
        $node->date = date("Y-m-j H:i:s", $value->created);

        /** Node comment */
        $node->comment = $value->comment;

        /** Node author record */
        $author_record = $author_exist->fetchObject();
        
        /** Node author new id */
        $node->uid = (int)$author_record->uid;
        
        $contributor_role_id = mediacommons_base_import_get_role_by_name('contributor');
        
        /** Node author */
        $node->uid = (int)$author_record->uid;
        
        /** Load user object */
        $account = user_load(array('uid' => $node->uid));
        
        /** Save the user object with the new roles. */
        if ($account !== FALSE && !isset($account->roles[$contributor_role_id])) {
          $roles = $account->roles + array($contributor_role_id => 'contibutor');
          user_save($account, array('roles' => $roles));
        }
        
        /** Add description */
        $node->field_description[$node->language][0] = array(
          'value' => $value->body,
          'format' => 'full_html',
        );
        
        /** Media type */
        $node->field_type[$node->language][0]['value'] = 1;
      
        /** Hub representative image */
        if (!empty($value->field_theme_week_image_fid)) {
          $dupal_file_path = drupal_realpath('public://');
          $data = mediacommons_base_import_find_image($value->field_theme_week_image_fid);
          $url_image_path = url($production_url . $data->filepath);
          $source_image_path = $dupal_file_path . '/' . $data->filename;
          file_put_contents($source_image_path, file_get_contents($url_image_path));
          $file = (object) array(
            'uid' => 1,
            'uri' => $source_image_path,
            'filemime' => file_get_mimetype($source_image_path),
            'status' => 1,
          );
          $file = file_copy($file, 'public://');
          $node->field_representative_image[$node->language][0] = (array)$file;
        }

        $contributors = array();
        $posts = array();        
        
        if (!empty($value->field_featured_post_1_nid)) {
          $posts[] = $value->field_featured_post_1_nid;
        }
        
        if (!empty($value->field_featured_post_2_nid)) {
          $posts[] = $value->field_featured_post_2_nid;
        }

        if (!empty($value->field_featured_post_3_nid)) {
          $posts[] = $value->field_featured_post_3_nid;
        }

        if (!empty($value->field_featured_post_4_nid)) {
          $posts[] = $value->field_featured_post_4_nid;
        }

        if (!empty($value->field_featured_page_or_feed_nid)) {
          $posts[] = $value->field_featured_page_or_feed_nid;
        }
    
        if (!empty($value->field_featured_post_5_nid)) {
          $posts[] = $value->field_featured_post_5_nid;
        }
        
        
        foreach ($posts as $key => $post) {
          $post_exist = db_query("SELECT nid FROM {mediacommons_base_import_node_map} WHERE onid = :onid", array(':onid' => $post));
          if ($post_exist->rowCount()) {
            $post_record = $post_exist->fetchObject();
            $c = db_query("SELECT uid FROM {node} WHERE nid = :nid", array(':nid' => $post_record->nid));
            $contributor_record = $c->fetchObject();
            $contributors[] = $contributor_record->uid;
            $node->field_spokes[LANGUAGE_NONE][$key]['nid'] = (int)$post_record->nid;
          }
        }

        foreach ($contributors as $key => $contributor) {
          $node->field_contributors[LANGUAGE_NONE][$key] = array(
            'uid' => $contributor,
            '_weight' => $key, 
          );        
        }        
        
        unset($contributors);
      
        /** dates */
        //$node->field_field_period[$node->language][0]['value'] = $value->field_period_value;

        //$node->field_field_period[$node->language][0]['value2'] = $value->field_period_value2;
      
        /** taxonomy */
        foreach (explode(', ', $value->terms) as $tid) {
          $tid_result = db_query("SELECT tid FROM {mediacommons_base_import_term_map} WHERE toid = :toid", array(':toid' => $tid));
          $tiCount = (int)$tid_result->rowCount();
          if ($tiCount > 0) {
            $term_object = $tid_result->fetchObject();
            $node->field_cluster_tags[$node->language][] = array('tid' => $term_object->tid);
          }
        }
        
        /** Prepare node for saving */
        if ($node = node_submit($node)) {
          node_save($node);
          db_insert('mediacommons_base_import_node_map')
            ->fields(array(
              'onid' => $value->nid,
              'nid' => $node->nid,
            )
          )->execute();
          drush_print('Node saved. ' . url('node/' . $node->nid, array('absolute'=>TRUE)) . "\n");
        }
      }
      else {
        drush_print('Node already exist');
      }
    }
  }  
}


function mediacommons_base_import_comment($nid, $oldNid) {

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

/**
 * Create a taxonomy term and return the tid.
 */
function mediacommons_base_import_create_taxonomy_term($term) {
  
  $new = new stdClass();
  $vocabulary = mediacommons_base_import_find_vocabulary($term->vid);
  $new->vid = $vocabulary->vid;
  $new->name = $term->name;
  $new->description = $term->description;
  $new->weight = $term->weight;
  $new->parent = (int)$term->parent;
  if (taxonomy_term_save($new)) {
    db_insert('mediacommons_base_import_term_map')
      ->fields(array(
        'toid' => $term->tid,
        'tid' => $new->tid,
      )
    )->execute();
    drush_print('Term ' . $new->name . ' saved!');
    return $new;
  }
}

/**
 * Create a taxonomy vocabulary.
 */
function mediacommons_base_import_create_taxonomy_vocabulary($vocabulary) {
  
  $new = new stdClass();
  $machine_name = strtolower(preg_replace('/\s+/', '', $vocabulary->name));
  $new->name = $vocabulary->name;
  $new->description = $vocabulary->description;
  $new->help = $vocabulary->help;
  $new->relations = $vocabulary->relations;
  $new->machine_name = $machine_name;
  $new->hierarchy = $vocabulary->hierarchy;
  $new->multiple = $vocabulary->multiple;
  $new->required = $vocabulary->required;
  $new->tags = $vocabulary->tags;
  $new->module = $vocabulary->module;
  $new->weight = $vocabulary->weight;
  if ($v = taxonomy_vocabulary_save($new)) {
    $m = taxonomy_vocabulary_machine_name_load($new->machine_name);
    db_insert('mediacommons_base_import_vocabulary_map')
      ->fields(array(
        'vuid' => $vocabulary->vid,
        'vid' => $m->vid,
      )
     )->execute();
     drush_print('Vocabulary saved. ' . $vocabulary->name . "\n");
     return $v;
  }
}

/**
 * Create user.
 */
function mediacommons_base_import_generate_user($user) {

  drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_FULL);
  
  $query = 'SELECT * FROM {mediacommons_base_import_user_map} WHERE ouid = :ouid';

  $result = db_query($query, array(':ouid' => $user->uid));

  $exclude_by_username = db_query("SELECT * FROM {users} WHERE name = :name", array(':name' => $user->name));
  
  if ($result->rowCount() == 0 && $exclude_by_username->rowCount() == 0) {
    
    drush_print('Importing user: ' . $user->name);

    $new_user = array(
      'name' => $user->name,
      'pass' => user_password(),
      'mail' => $user->mail,
      'init' => $user->init,
      'status' => $user->status,
      'access' => $user->access,
      'theme' => $user->theme,
      'signature' => $user->signature,
      'signature_format' => $user->signature_format,
      'created' => $user->created,
      'login' => $user->login,
      'language' => $user->language,
      'picture' => $user->picture,
      'init' => $user->init,
      'timezone_name' => $user->timezone_name,
    );
    
    if (isset($user->roles)) {
      foreach(explode(',', $user->roles) as $value) {
        $new_user['roles'][$value] = TRUE;
      }
    }

    $account = user_save(null, $new_user);
    
    if ($account) {
      db_insert('mediacommons_base_import_user_map')
        ->fields(array(
          'ouid' => $user->uid,
          'uid' => $account->uid,
        )
      )->execute();    

      db_update('users')->fields(array('mail' => 'dlts.pa@nyu.edu'))
        ->condition('mail', $user->mail, '=')
        ->execute();
      $account->ouid = $user->uid;
    }
    return $account;
  }
  else {
    if ($exclude_by_username->rowCount()) {
      drush_print('User "'. $user->name .'" already exist in database');
      $m = $exclude_by_username->fetchObject();
      $account = user_load($m->uid);
      $account->ouid = $user->uid;
      return $account;
    }
    else {
      return FALSE;
    }
  }
}