<?php

// http://drupal.org/node/1481998

function mediacommons_import_generate_spokes() {

  /** Load data, this will take time; go get yourself a coffee */
  $data = mediacommons_import_d6_spoke_list();

  /** Get a list of all the D6 users */
  $d6_users = mediacommons_import_d6_users_list();

  /** If we have $data proceed */
  if (isset($data)) {

    foreach ($data as $key => $spoke) {

      /** Find out if user is already in the system */
      $node_exist = db_query('SELECT nid FROM {mediacommons_import_node_map} u WHERE onid = :onid', array(':onid' => $spoke->nid));

      if (!$node_exist->rowCount()) {
        
        drush_print('Creating ' . $spoke->title . "\n");

        /** This node defualt configuration */
        $node = _mediacommons_import_setup_node('spoke');

        $node->title = $spoke->title;

        /** Find out if user is already in the system */
        $author_result = db_query('SELECT uid FROM {mediacommons_import_user_map} u WHERE ouid = :ouid', array(':ouid' => $spoke->uid));

        if (!$author_result->rowCount() && isset($d6_users[$spoke->uid])) {
          $author_record = mediacommons_import_generate_user($d6_users[$spoke->uid]);
          if ($author_record) {
            db_insert('mediacommons_import_user_map')
            ->fields(array(
            'ouid' => $author_record->ouid,
            'uid' => $author_record->uid,
            )
            )->execute();
          }
        }
        else {
          $author_record = $author_result->fetchObject();
        }

        /** Node author */
        $node->uid = (int)$author_record->uid;

        $node->created = (int)$spoke->created;

        $node->changed = (int)$spoke->changed;

        $node->comment = (int)$spoke->comment;

        $node->status = (int)$spoke->status;

        $node->status = (int)$spoke->status;

        $node->promote = (int)$spoke->promote;

        $node->moderate = (int)$spoke->moderate;

        $node->sticky = (int)$spoke->sticky;

        /** Additonal authors */
        if ($spoke->additonal_authors) {
          
          /** Contributors */
          $spoke_contributors = explode(', ', $spoke->additonal_authors);

          /** Contributor order */
          $contributor_order = explode(', ', $spoke->additonal_authors_delta);

          /** Find out if user is already in the system */
          foreach ($spoke_contributors as $key => $contributor) {
            $result = db_query('SELECT uid FROM {mediacommons_import_user_map} u WHERE ouid = :ouid', array(':ouid' => $contributor));
            if (!$result->rowCount() && isset($d6_users[$contributor])) {
              $record = mediacommons_import_generate_user($d6_users[$contributor]);
              if ($record) {
                db_insert('mediacommons_import_user_map')
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
        }

        /** Description */
        $node->field_body[$node->language][0] = array(
            'value' => $spoke->field_document_textarea_body_value,
            'format' => 'filtered_html',
        );

        /** Publication status */
        if ($spoke->field_pubstat_value == 'Published') {
          $node->field_pubstat[$node->language][0]['value'] = 1;
        }
        else {
          $node->field_pubstat[$node->language][0]['value'] = 0;
        }

        /** Clusters */
        if (
            !empty($spoke->cluster_nid)
        ) {

          $cluster_nid = explode(', ', $spoke->cluster_nid);

          foreach ($cluster_nid as $key => $nid) {
            $hub_result = db_query("SELECT * FROM {mediacommons_import_node_map} WHERE onid = :onid", array(':onid' => $nid));
            if ($hub_result->rowCount()) {
              $hub_record = $hub_result->fetchObject();
              $node->field_part_of_hub[$node->language][$key]['nid'] = (int)$hub_record->nid;
            }
          }
        }

        /** Representative image */

        /*
         if (($value['filename'] != 'NULL')) {
        // Some file on our system
        $file_path = $script_path . $image_source . $value['filename'];

        $file = (object) array(
            'uid' => (int)$value['uid'],
            'uri' => $file_path,
            'filemime' => file_get_mimetype($file_path),
            'status' => 1,
        );
         
        // Save the file to the root of the files directory.
        $file = file_copy($file, 'public://'. $image_destination);
         
        // ALBERTO I'm having trouble loading the file into the attached image table
        // $node->field_attached_images[$node->language][0] = (array)$file; //associate the file object with the image field:
        $node->field_representative_image[$node->language][0] = (array)$file; //associate the file object with the image field:
         
        }
        */

        /** attached images */

        /**
         if (($value['attached_image_filepath'] != 'NULL')) {

         $array = explode(", ", $value['attached_image_filepath']);

         foreach ($array as $key => $value) {
         $filename = pathinfo($value, PATHINFO_BASENAME);
         $file_path = $script_path . $image_source . $filename;
         $file = (object) array(
         'uid' => (int)$data[$c]['uid'],
         'uri' => $file_path,
         'filemime' => file_get_mimetype($file_path),
         'status' => 1,
         );

         // Save the file to the root of the files directory.
         $file = file_copy($file, 'public://'. $image_destination);

         // associate the file object with the image field
         $node->field_attached_images[$node->language][0] = (array)$file;

         }
         }
         */

        /** taxonomy */
        // $array = explode(", ", $value['terms']);

        /** Prepare node for saving */
        if ($node->uid != 0 && $node = node_submit($node)) {
          node_save($node);
          db_insert('mediacommons_import_node_map')
          ->fields(array(
          'onid' => $value['nid'],
          'nid' => $node->nid,
          )
          )->execute();
          drush_print("Spoke saved. " . url('node/' . $node->nid, array('absolute'=>TRUE)) . "\n");
        }

      }
      else {
        drush_print('Spoke already exist');
      }

    }
  }

}

/**
 * Create a taxonomy term and return the tid.
 */
function mediacommons_import_create_taxonomy_term($term) {
  $new = new stdClass();
  $vocabulary = mediacommons_import_find_vocabulary($term->vid);
  $new->vid = $vocabulary->vid;
  $new->name = $term->name;
  $new->description = $term->description;
  $new->weight = $term->weight;
  $new->parent = (int)$term->parent;
  if (taxonomy_term_save($new)) {
    db_insert('mediacommons_import_term_map')
      ->fields(array(
        'toid' => $term->tid,
        'tid' => $new->tid,
      )
    )->execute();
    drush_print('Term ' . $new->name . ' saved!');
    return $new;
  }
}

function mediacommons_import_create_taxonomy_vocabulary($vocabulary) {
  $new = new stdClass();
  $machine_name = 'd6' . strtolower(preg_replace('/\s+/', '', $vocabulary->name));
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
    db_insert('mediacommons_import_vocabulary_map')
      ->fields(array(
        'vuid' => $vocabulary->vid,
        'vid' => $m->vid,
      )
     )->execute();
     drush_print('Vocabulary saved. ' . $vocabulary->name . "\n");
     return $v;
  }
}

function mediacommons_import_generate_hubs() {
  
  $settigs = mediacommons_import_settings();
  
  $production_url = $settigs['settings']['production']['url'];

  $data = mediacommons_import_d6_hub_list();
  
  /** Get a list of all the D6 users */
  $d6_users = mediacommons_import_d6_users_list();
  
  /** If we have $data proceed */
  if ($data->rowCount()) {

    foreach ($data as $key => $value) {
      
      /** drush_print_r($value); */
      
      /** Find out if user is already in the system */
      $node_exist = db_query('SELECT nid FROM {mediacommons_import_node_map} u WHERE onid = :onid', array(':onid' => $value->nid));
      
      if (!$node_exist->rowCount()) {
        
        /** This node defualt configuration */
        $node = _mediacommons_import_setup_node('hub');
        
        drush_print('Creating ' . $value->title . "\n");        

        /** Node title */
        $node->title = $value->title;

        /** Node status */
        $node->status = $value->status;

        /** Node comment */
        $node->comment = $value->comment;

        /** Find out if user is already in the system */
        $author_result = db_query('SELECT uid FROM {mediacommons_import_user_map} u WHERE ouid = :ouid', array(':ouid' => $value->uid));

        if (!$author_result->rowCount() && isset($d6_users[$value->uid])) {
          $author_record = mediacommons_import_generate_user($d6_users[$value->uid]);

          /** Asume user is a editor since has the rights to create hubs */
          drush_user_add_role('administrator', $author_record->name);

          if ($author_record) {
            db_insert('mediacommons_import_user_map')
              ->fields(array(
                'ouid' => $author_record->ouid,
                'uid' => $author_record->uid,
              )
            )->execute();
          }
        }

        else {
          $author_record = $author_result->fetchObject();
        }      

        /** Node author */
        $node->uid = (int)$author_record->uid;

        /** Contributors */
        $hub_contributors = explode(', ', $value->contributors);

        /** Contributor order */
        $contributor_order = explode(', ', $value->contributor_order);      

        /** Find out if user is already in the system */
        foreach ($hub_contributors as $key => $contributor) {

          $result = db_query('SELECT uid FROM {mediacommons_import_user_map} u WHERE ouid = :ouid', array(':ouid' => $contributor));

          if (!$result->rowCount() && isset($d6_users[$contributor])) {
            $record = mediacommons_import_generate_user($d6_users[$contributor]);
            if ($record) {
              db_insert('mediacommons_import_user_map')
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
          $data = mediacommons_import_find_image($value->field_image_fid);
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
        foreach (explode(', ', $value->terms) as $key => $tid) {
          if ($tid) {
            drush_print('Generating term');
            $term = mediacommons_import_find_term($tid);
            $node->field_cluster_tags[$node->language][] = array('tid' => $term->tid );
          }
        }
        
        /** Prepare node for saving */
        if ($node = node_submit($node)) {
          node_save($node);
          db_insert('mediacommons_import_node_map')
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

function mediacommons_import_generate_user($user) {

  drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_FULL);

  $result = db_query("SELECT * FROM {mediacommons_import_user_map} WHERE ouid = :ouid", array(':ouid' => $user->uid));

  $exclude_by_username = db_query("SELECT * FROM {users} WHERE name = :name", array(':name' => $user->name));

  if ($result->rowCount() == 0 && $exclude_by_username->rowCount() == 0) {

    $new_user = array(
        'name' => $user->name,
        'pass' => user_password(),
        'mail' => $user->mail,
        'init' => $user->init,
        'status' => $user->status,
        'access' => $user->access,
        // 'roles' => $user->roles,
    );

    $account = user_save(null, $new_user);

    db_update('users')->fields(array('mail' => 'dlts.pa@nyu.edu'))
    ->condition('mail', $user->mail, '=')
    ->execute();

    $account->ouid = $user->uid;

    return $account;
  }

  else {

    if ($exclude_by_username->rowCount()) {
      drush_print('User already exist in database');
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

function mediacommons_import_generate_users($new_users_per_rol = 3) {

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

function mediacommons_import_generate_random_content($new_hubs = 4) {

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