<?php

function mediacommons_imr_import_generate_all_users() {
  drush_print('Importing users: This will take some time, go get yourself a coffee');
  drush_print("\n");
  $users_d6_list = mediacommons_imr_import_d6_users_list();
  foreach ($users_d6_list as $user) {
    mediacommons_imr_import_generate_user($user);
  }
}

function mediacommons_imr_import_generate_random_users($new_users_per_rol = 3) {

  drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_FULL);

  $sharedemail_enable = FALSE;
  $sharedemail_path = drupal_get_path('module', 'sharedemail');

  if (empty($sharedemail_path)) {
    drush_print('I need sharedemai module in order to use the same email for all the accounts');
    return;
  }

  drush_set_option('password', 'dlts2010');

  /** make sure we can used the same email address */
  if ( !module_exists('sharedemail') ) {
    drush_print('Will try to enable sharedemail module. Remember to disable/uninstall and remove module before going to production.');
    module_enable(array('sharedemail'));
  }

  foreach (user_roles(TRUE) as $rol) {
    $i = $new_users_per_rol;
    while ($i--) {
      $user_name = str_replace(' ', '_', $rol) . '_' . uniqid();
      $result = db_query("SELECT name FROM {users} WHERE name = :name", array(':name' => $user_name));
      if (!$result->rowCount()) {
        drush_user_create($user_name);
        drush_user_add_role($rol, $user_name);
        db_update('users')->fields(array('mail' => variable_get('site_mail', 'dlts.pa@nyu.edu')))
        ->condition('name', $user_name, '=')
        ->execute();
      }
    }
  }
}

function mediacommons_imr_import_generate_roles() {
  $roles_d7 = array();
  $roles_d6_list = mediacommons_imr_import_d6_roles();
  
  foreach(mediacommons_imr_import_d7_roles() as $role) {
    $roles_d7[$role->name] = $role->rid;
  }

  foreach ($roles_d6_list as $record) {
  
    $rid = db_query("SELECT rid FROM {mediacommons_imr_import_role_map} WHERE orid = :orid", array(':orid' => $record->rid));
    
    if (!$rid->rowCount()) {
       drush_print_r('Role exist in Drupal 7');        
      /** Role exist in Drupal 7 */
      if (array_key_exists($record->name, $roles_d7)) {
        drush_print('Role ' . $record->name . ' already exist');
        db_insert('mediacommons_imr_import_role_map')
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
        db_insert('mediacommons_imr_import_role_map')
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

function mediacommons_imr_import_generate_vocabulary() {
  $d7 = mediacommons_imr_import_d7_vocabulary_list();
  $d6 = mediacommons_imr_import_d6_vocabulary_list();
  $d7_names = array();

  foreach ($d7 as $record) {
    $d7_names[strtolower($record->name)] = $record->vid;
  }

  foreach ($d6 as $record) {
    $vid = db_query("SELECT vid FROM {mediacommons_imr_import_vocabulary_map} WHERE vuid = :vuid", array(':vuid' => $record->vid));
    if (!$vid->rowCount()) {
      /** Vocabulary exist in Drupal 7 */
      if (array_key_exists(strtolower($record->name), $d7_names)) {
        drush_print('Vocabulary ' . strtolower($record->name) . ' already exist');
        db_insert('mediacommons_imr_import_vocabulary_map')
          ->fields(array(
            'vuid' => $record->vid,
            'vid' => $d7_names[strtolower($record->name)],
          )
        )->execute();
      }
      /** Vocabulary does not exist in Drupal 7 */
      else {
        drush_print('Creating ' . $record->name);
        mediacommons_imr_import_create_taxonomy_vocabulary($record);
      }
    }
  }
}

function mediacommons_imr_import_generate_terms() {
  
  /** Just in case */
  mediacommons_imr_import_generate_vocabulary();
  
  $vocabulary_map = db_query("SELECT * FROM {mediacommons_imr_import_vocabulary_map}");
  $terms_d6 = mediacommons_imr_import_d6_term_list();
  
  $vocabularies = array();
  
  foreach ($vocabulary_map as $vocabulary) {
    $vocabularies[$vocabulary->vuid] = $vocabulary->vid;
  }
  
  foreach ($terms_d6 as $term) {
    if (isset($vocabularies[$term->vid])) {
      $tid = db_query("SELECT tid FROM {mediacommons_imr_import_term_map} WHERE toid = :toid", array('toid' => $term->tid));
      if (!$tid->rowCount()) {
        mediacommons_imr_import_create_taxonomy_term($term);
      }
    }
  }
  
}

/**
 * Create a spokes.
 */
function mediacommons_imr_import_generate_spokes() {

  /** Load data, this will take time; go get yourself a coffee */
  $data = mediacommons_imr_import_d6_post_list();
  
  $settigs = mediacommons_imr_import_settings();
  
  $production_url = $settigs['settings']['production']['url'];
  /** If we have $data proceed */
  if (isset($data)) {

    foreach ($data as $key => $spoke) {
      
      // drush_print_r($spoke);

      /** Find out if node is already in the system */
      $node_exist = db_query('SELECT nid FROM {mediacommons_imr_import_node_map} u WHERE onid = :onid', array(':onid' => $spoke->nid));
      
      $rowCount = (int)$node_exist->rowCount();
      
      $uid = (int)$spoke->uid;
      
      /** Find out if user is already in the system */
      $author_exist = db_query('SELECT uid FROM {mediacommons_imr_import_user_map} u WHERE ouid = :ouid', array(':ouid' => $uid));
      
      $authorCount = (int)$author_exist->rowCount();
      
      if (
          $rowCount == 0 && 
          $authorCount > 0 &&
          $uid > 0
      ) {
        
        drush_print('Creating ' . $spoke->title . "\n");
        
        /** This node defualt configuration */
        $node = _mediacommons_imr_import_setup_node('spoke');
          
        $node->title = $spoke->title;

        /** Node author record */
        $author_record = $author_exist->fetchObject();

        /** Node author new id */
        $node->uid = (int)$author_record->uid;      
          
        $node->created = (int)$spoke->created;

        $node->changed = (int)$spoke->changed;

        $node->comment = (int)$spoke->comment;

        $node->status = (int)$spoke->status;

        $node->promote = (int)$spoke->promote;

        $node->moderate = (int)$spoke->moderate;

        $node->sticky = (int)$spoke->sticky;
        
        /** Add description */
        $node->field_body[$node->language][0] = array(
          'value' => $spoke->body,
          'format' => 'filtered_html',
        );
        
        dpm($node->status);
        
        /** Add Publication Status */        
        $node->field_pubstat[$node->language][0] = array(
          'value' => $node->status,
        );
        
        /** Prepare node for saving */
        $node = node_submit($node);

        if ($node) {
          node_save($node);
          
          db_insert('mediacommons_imr_import_node_map')
            ->fields(array(
              'onid' => $value['nid'],
              'nid' => $node->nid,
            )
          )->execute();
          drush_print("Spoke saved. " . url('node/' . $node->nid, array('absolute'=>TRUE)) . "\n");
        }
        
      }
      else {
        drush_print('Skiping spoke. uid: ' . $uid . ' rowCount: ' . $rowCount);
      }
    }
  }
}

/**
 * Create a taxonomy term and return the tid.
 */
function mediacommons_imr_import_create_taxonomy_term($term) {
  $new = new stdClass();
  $vocabulary = mediacommons_imr_import_find_vocabulary($term->vid);
  $new->vid = $vocabulary->vid;
  $new->name = $term->name;
  $new->description = $term->description;
  $new->weight = $term->weight;
  $new->parent = (int)$term->parent;
  if (taxonomy_term_save($new)) {
    db_insert('mediacommons_imr_import_term_map')
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
function mediacommons_imr_import_create_taxonomy_vocabulary($vocabulary) {
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
    db_insert('mediacommons_imr_import_vocabulary_map')
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
 * Create a hubs.
 */
function mediacommons_imr_import_generate_hubs() {
  
  $settigs = mediacommons_imr_import_settings();
  
  $production_url = $settigs['settings']['production']['url'];

  $data = mediacommons_imr_import_d6_hub_list();
  
  /** Get a list of all the D6 users */
  $d6_users = mediacommons_imr_import_d6_users_list();
  
  /** If we have $data proceed */
  if ($data->rowCount()) {

    foreach ($data as $key => $value) {
      
      $old_author_uid = (int) $value->uid;
      
      /** Find out if user is already in the system */
      $author_exist = db_query('SELECT uid FROM {mediacommons_imr_import_user_map} u WHERE ouid = :ouid', array(':ouid' => $old_author_uid));
      
      $authorCount = (int)$author_exist->rowCount();
      
      /** Find out if node is already in the system */
      $node_exist = db_query('SELECT nid FROM {mediacommons_imr_import_node_map} u WHERE onid = :onid', array(':onid' => $value->nid));
      
      $rowCount = (int)$node_exist->rowCount();
      
      if (
          $rowCount == 0 &&
          $authorCount > 0 &&
          $old_author_uid > 0
      ) {
        
        drush_print('Creating ' . $value->title . "\n");        
        
        /** This node defualt configuration */
        $node = _mediacommons_imr_import_setup_node('hub');
        
        /** Node title */
        $node->title = $value->title;

        /** Node status */
        $node->status = $value->status;

        /** Node comment */
        $node->comment = $value->comment;

        /** Node author record */
        $author_record = $author_exist->fetchObject();
        
        /** Node author new id */
        $node->uid = (int)$author_record->uid;
        
        $contributor_role_id = mediacommons_imr_import_get_role_by_name('contributor');
        
        /** Node author */
        $node->uid = (int)$author_record->uid;
        
        /** Load user object */
        $account = user_load(array('uid' => $node->uid));
        
        /** Save the user object with the new roles. */
        if ($account !== FALSE && !isset($account->roles[$contributor_role_id])) {
          $roles = $account->roles + array($contributor_role_id => 'contibutor');
          user_save($account, array('roles' => $roles));
        }
        
        /** Contributors */
        $hub_contributors = explode(', ', $value->contributors);

        /** Contributor order */
        $contributor_order = explode(', ', $value->contributor_order);      

        /** Find out if user is already in the system */
        foreach ($hub_contributors as $key => $contributor) {

          $result = db_query('SELECT uid FROM {mediacommons_imr_import_user_map} u WHERE ouid = :ouid', array(':ouid' => $contributor));

          if (!$result->rowCount() && isset($d6_users[$contributor])) {
            $record = mediacommons_imr_import_generate_user($d6_users[$contributor]);
            if ($record) {
              db_insert('mediacommons_imr_import_user_map')
                ->fields(array(
                  'ouid' => $record->ouid,
                  'uid' => $record->uid,
                )
              )->execute();
            }
          }
          else {
            $record = $result->fetchObject();
          }
          $node->field_contributors[$node->language][$key] = array(
            'uid' => (int)$record->uid,
            '_weight' => (int)$contributor_order[$key], 
          );
        }
      
        /** Add description */
        $node->field_description[$node->language][0] = array(
          'value' => $value->field_description_value,
          'format' => 'filtered_html',
        );
        
        /** Video */
        if (isset($value->provider)) {
          if ($value->provider == 'youtube') {
           $video_url = 'http://www.youtube.com/watch?v=' . $value->video_link;  
          }
          $node->field_video_embed_link[$node->language][0]['video_url'] = $video_url;
        }
        
        /** Media type */
        $node->field_type[$node->language][0]['value'] = $value->field_cluster_type_value;
      
        /** Hub representative image */
        if (!empty($value->field_image_fid)) {
          $dupal_file_path = drupal_realpath('public://');
          $data = mediacommons_imr_import_find_image($value->field_image_fid);
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
      
        /** dates */
        $node->field_field_period[$node->language][0]['value'] = $value->field_period_value;

        $node->field_field_period[$node->language][0]['value2'] = $value->field_period_value2;
      
        /** taxonomy */
        foreach (explode(', ', $value->terms) as $tid) {
          $tid_result = db_query("SELECT tid FROM {mediacommons_imr_import_term_map} WHERE toid = :toid", array(':toid' => $tid));
          $tiCount = (int)$tid_result->rowCount();
          if ($tiCount > 0) {
            $term_object = $tid_result->fetchObject();
            $node->field_cluster_tags[$node->language][] = array('tid' => $term_object->tid);
          }
        }
        
        /** Prepare node for saving */
        if ($node = node_submit($node)) {
          node_save($node);
          db_insert('mediacommons_imr_import_node_map')
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

/**
 * Create user.
 */
function mediacommons_imr_import_generate_user($user) {

  /** IMR admin role is the same as Administrator role in Drupal 7? Same privilege? */
  /** If the user is black, we want to  migrate it? */

  drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_FULL);

  $result = db_query("SELECT * FROM {mediacommons_imr_import_user_map} WHERE ouid = :ouid", array(':ouid' => $user->uid));

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
      db_insert('mediacommons_imr_import_user_map')
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

/**
 * Generate random content
 */
function mediacommons_imr_import_generate_random_content($new_hubs = 4) {

  drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_FULL);

  // http://digg.com/rss/top.rss
  // http://digg.com/rss/popular.rss

  $feed = 'http://digg.com/rss/popular.rss';
  $feed = __DIR__ . "/top.rss";

  /** find users */
  $user_roles = db_query("SELECT DISTINCT uid FROM {users_roles} WHERE uid <> 1 AND (rid <> 1 OR rid <> 2)", array());

  /** fetch users */
  $user = $user_roles->fetchAll();

  $xml = simplexml_load_file($feed);

  $hubs_count = count($xml->channel->item);

  $data = array(
      'hubs' => array()
  );

  $items = $xml->channel->item;

  $a = 1;

  if ($hubs_count >= $new_hubs ) {
    foreach($items as $child) {
      $title = (string) $child->title[0];
      $data['hubs'][$a] = new stdClass();
      $data['hubs'][$a]->title = !empty($title) ? $title : 'Working title';
      $data['hubs'][$a]->type = 'hub';
      $data['hubs'][$a]->language = LANGUAGE_NONE;
      $data['hubs'][$a]->uid = $user[array_rand($user)]->uid;
      $data['hubs'][$a]->status = 1; //(1 or 0): published or not
      $data['hubs'][$a]->promote = 1; //(1 or 0): promoted or not
      $data['hubs'][$a]->comment = 1; //2 = comments on, 1 = comments off
      $data['hubs'][$a]->field_contributors[LANGUAGE_NONE][0]['uid'] = $user[array_rand($user)]->uid;
      $data['hubs'][$a]->field_contributors[LANGUAGE_NONE][1]['uid'] = $user[array_rand($user)]->uid;
      $data['hubs'][$a]->field_co_editor[LANGUAGE_NONE][0]['uid'] = $user[array_rand($user)]->uid;
      $data['hubs'][$a] = node_submit($data['hubs'][$a]); // Prepare node for saving
      node_save($data['hubs'][$a]);
      if ($a == $new_hubs) {
        break;
      }
      $a++;
    }
  }
  foreach ($data['hubs'] as $hub) {
    $spokes = 4;
    while($spokes--) {
      $content = $items[rand($new_hubs, $hubs_count)];
      $title = (string) $content->title[0];
      $node = new stdClass();
      $node->title = !empty($title) ? $title : 'Working title';
      $node->type = 'spoke';
      $node->language = $hub->language;
      $node->uid = $user[array_rand($user)]->uid;
      $node->status = 1; //(1 or 0): published or not
      $node->promote = 1; //(1 or 0): promoted or not
      $node->comment = 1; //2 = comments on, 1 = comments off
      $node->field_part_of_hub[$node->language][]['nid'] = $hub->nid;
      $node = node_submit($node); // Prepare node for saving
      node_save($node);
    }
  }
}