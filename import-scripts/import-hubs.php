<?php

/**
 * Read more: http://zengenuity.com/blog/a/201011/importing-files-drupal-nodes-cck-fieldfield-imagefield#ixzz1v3tz75My
 * http://fooninja.net/2011/04/13/guide-to-programmatic-node-creation-in-drupal-7/
 * http://drupal.org/node/330421#comment-2806336
 * http://www.group42.ca/creating_and_updating_nodes_programmatically_in_drupal_7
 */

/** Implements hook_schema(). */
function mediacommons_import_schema() {
  
  $schema = array();
  
  $schema['mediacommons_import_user_map'] = array(
      'description' => 'Stores a record of node imports.',
      'fields' => array(
          'ouid' => array(
              'description' => 'The primary identifier for the Drupal 6 node',
              'type' => 'serial',
              'unsigned' => TRUE,
              'not null' => TRUE,
          ),
          'uid' => array(
              'description' => 'The primary identifier for the node.',
              'type' => 'int',
              'unsigned' => TRUE,
              'not null' => TRUE,
              'default' => 0,
          ),
      ),
      'indexes' => array(
          'uid' => array('uid'),
      ),
      'primary key' => array('ouid'),
  );
  
  $schema['mediacommons_import_node_map'] = array(
      'description' => 'Stores a record of node imports.',
      'fields' => array(
          'onid' => array(
              'description' => 'The primary identifier for the Drupal 6 node',
              'type' => 'serial',
              'unsigned' => TRUE,
              'not null' => TRUE,
          ),
          'nid' => array(
              'description' => 'The primary identifier for the node.',
              'type' => 'int',
              'unsigned' => TRUE,
              'not null' => TRUE,
              'default' => 0,
          ),
      ),
      'indexes' => array(
          'nid' => array('nid'),
      ),
      'primary key' => array('onid'),
  );  
  
  return $schema;
}

function mediacommons_import_find_nodes($type) {

  /** find all */
  $query = db_query("SELECT DISTINCT nid FROM {node} WHERE type = :type", array('type' => $type));

  /** return nids */
  return $query->fetchAll();

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

function mediacommons_import_prepare() {
  global $settigs;
  
  if (!defined('__DIR__')) define('__DIR__', dirname(__FILE__));
  
  ini_set('memory_limit', '512M');  
  
  $script = pathinfo(__FILE__);
  $settigs = array();
  $settigs = json_decode(utf8_encode(file_get_contents($script['dirname'] . '/settigs.json')), true);
  $settigs['script_path'] = $script;
  
  if (!db_table_exists('mediacommons_import_user_map')) {
    $schema = mediacommons_import_schema();
    mediacommons_import_initialize_schema('mediacommons_import', $schema);
    db_create_table('mediacommons_import_user_map', $schema['mediacommons_import_user_map']);
  }
  
  if (!db_table_exists('mediacommons_import_node_map')) {
    $schema = mediacommons_import_schema();
    mediacommons_import_initialize_schema('mediacommons_import', $schema);
    db_create_table('mediacommons_import_node_map', $schema['mediacommons_import_node_map']);
  }  
    
}

function mediacommons_import_load_cvs_file($filepath) {
  $data = array();
  $header = NULL;
  if (($handle = fopen($filepath, "r")) !== FALSE) {
    while (($row = fgetcsv($handle, 1000, ",")) !== FALSE) {
      if(!$header) {
        $header = $row;
      } else {
        $data[] = array_combine($header, $row);
      }
    }
  }
  fclose($handle);
  return $data;
}

function mediacommons_import_select_csv_file($type) {
  global $settigs;

  $cvs_files = array();
  
  /** find all the csv files */
  $cvs_files = file_scan_directory($settigs['script_path']['dirname'] . '/' . $settigs['settings'][$settigs['environment']]['csv_directory'] . '/', '/.*\.csv$/', array( 'recurse' => FALSE ));
  
  $cvs_files_options = array_keys($cvs_files);
  
  drush_print(t('Please select one of the following files to continue') . ":\n");
  
  if (count($cvs_files_options)) {
    for ($i = 0; $i < count($cvs_files_options); $i++) {
      drush_print($i . ') ' . ($cvs_files_options[$i] ));
    }  
    $handle = fopen ("php://stdin","r");
    $line = fgets($handle);  
    $filepath = $cvs_files[$cvs_files_options[(int)trim($line)]];  
    if (isset($filepath) && is_file($filepath->uri)) {
      return $filepath->uri;
    }
  }
  return NULL;
}

/** Return a plain node template */
function mediacommons_import_setup_node($type) {
  $node = new StdClass();
  $node->type = $type;
  $node->language = LANGUAGE_NONE;
  $node->promote = 0;
  $node->sticky = 0;
  /** Fills in a few default values */
  node_object_prepare($node);  
  return $node;
}

/**
 * Assuming you have already a local copy of TNE, your settings.php should looks somtething like this:

  $databases['drupal6'] = array(
    'default' => array(
        'database' => 'tne_d6_local',
        'username' => 'root',
        'password' => '007love',
        'host' => '127.0.0.1',
        'port' => '',
        'driver' => 'mysql',
        'prefix' => 'tne_',
    ),
  );

*/
function mediacommons_import_d6_users_list() {
  global $databases;
  
  $users = array();
  
  if (!isset($databases['drupal6'])) {
    drush_set_error('About to die like a coward. Unable to find Drupal 6 database.');
    drush_die();
  }
  
  /** Create a database connection to the source (d6) database */
  $query = Database::getConnection('default', 'drupal6')
  ->select('users', 'u')
  ->fields('u', array(
      'uid',
      'name',
      'pass',
      'mail',
      'theme',
      'signature',
      'signature_format',
      'created',
      'access',
      'login',
      'status',
      'language',
      'picture',
      'init',
      'timezone_name'
  ))
  ->condition('u.uid', 1, '>');
  
  $query->leftJoin('users_roles', 'ur', 'u.uid = ur.uid');
  
  $query->groupBy('uid');
  
  $query->addExpression("GROUP_CONCAT(DISTINCT ur.rid SEPARATOR ',')", 'roles');
  
  $results = $query->execute();
  
  foreach ($results as $result) {
    $users[$result->uid] = $result;
  }
  
  return $users;
    
}

function mediacommons_import_initialize_schema($module, &$schema) {
  // Set the name and module key for all tables.
  foreach ($schema as $name => $table) {
    if (empty($table['module'])) {
      $schema[$name]['module'] = $module;
    }
    if (!isset($table['name'])) {
      $schema[$name]['name'] = $name;
    }
  }
}

function mediacommons_import_generate_spokes() {
  /** Promt user which CSV file he want to use */
  $csv_filepath = mediacommons_import_select_csv_file('spoke');
  
  /** Load user selected CSV file */
  if ($csv_filepath) {
    $data = mediacommons_import_load_cvs_file($csv_filepath);
  }
  
  /** Get a list of all the D6 users */
  $d6_users = mediacommons_import_d6_users_list();
  
  /** If we have $data proceed */
  if (isset($data)) {
    
    foreach ($data as $key => $spoke) {
      
      /** Find out if user is already in the system */
      $node_exist = db_query('SELECT nid FROM {mediacommons_import_node_map} u WHERE onid = :onid', array(':onid' => $spoke['nid']));

      if (!$node_exist->rowCount()) {

        /** This node defualt configuration */
        $node = mediacommons_import_setup_node('spoke');
        
        $node->title = $spoke['title'];
        
        /** Find out if user is already in the system */
        $author_result = db_query('SELECT uid FROM {mediacommons_import_user_map} u WHERE ouid = :ouid', array(':ouid' => $spoke['uid']));
      
        if (!$author_result->rowCount() && isset($d6_users[$spoke['uid']])) {
          $author_record = mediacommons_import_generate_user($d6_users[$spoke['uid']]);
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
        
        $node->created = (int)$spoke['created'];
        
        $node->changed = (int)$spoke['changed'];
        
        $node->comment = (int)$spoke['comment'];
        
        /** Additonal authors */
        
        
        if ($spoke['additonal_authors']) {
          /** Contributors */
          $spoke_contributors = explode(', ', $spoke['additonal_authors']);
        
          /** Contributor order */
          // $contributor_order = explode(', ', $value['contributor_order']);
        
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
              //'_weight' => (int)$contributor_order[$key],
            );
        
          }
        }
        
        /** Description */
        $node->field_body[$node->language][0] = array(
          'value' => $spoke['field_document_textarea_body_value'],
          'format' => 'filtered_html',
        );
        
        /** Publication status */
        if ($data[$c]['field_pubstat_value'] == 'Published') {
          $node->field_pubstat[$node->language][0]['value'] = 1;
        }
        else {
          $node->field_pubstat[$node->language][0]['value'] = 0;
        }

        /** Clusters */
        if (isset($spoke['cluster_nid']) && $spoke['cluster_nid'] != 'NULL') { // CSV value NULL, is not really NULL (sort of speak)
          drush_print('testing cluster: ' . $spoke['cluster_nid']);
          
          $hub_result = db_query("SELECT * FROM {mediacommons_import_node_map} WHERE onid = :onid", array(':onid' => $spoke['cluster_nid']));
          
          if ($hub_result->rowCount()) {
            $hub_record = $hub_result->fetchObject();
            drush_print_r($hub_record);
            $node->field_part_of_hub[$node->language][$key]['nid'] = (int)$hub_record->nid;
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
        if ($node = node_submit($node)) {
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

function mediacommons_import_generate_hubs() {
  
  /** Promt user which CSV file he want to use */
  $csv_filepath = mediacommons_import_select_csv_file('hub');
  
  /** Load user selected CSV file */
  if ($csv_filepath) {
    $data = mediacommons_import_load_cvs_file($csv_filepath);
  }
  
  /** Get a list of all the D6 users */
  $d6_users = mediacommons_import_d6_users_list();

  /** If we have $data proceed */
  if (isset($data)) {
    
    foreach ($data as $key => $value) {
      
      /** Find out if user is already in the system */
      $node_exist = db_query('SELECT nid FROM {mediacommons_import_node_map} u WHERE onid = :onid', array(':onid' => $value['nid']));
      
      if (!$node_exist->rowCount()) {
        
        /** This node defualt configuration */
        $node = mediacommons_import_setup_node('hub');

        $node->title = $value['title'];
      
        drush_print('Creating ' . $node->title . "\n");
      
        $node->status = $value['status'];
      
        $node->comment = $value['comment'];
      
        /** Find out if user is already in the system */
        $author_result = db_query('SELECT uid FROM {mediacommons_import_user_map} u WHERE ouid = :ouid', array(':ouid' => $value['uid']));
      
        if (!$author_result->rowCount() && isset($d6_users[$value['uid']])) {
          $author_record = mediacommons_import_generate_user($d6_users[$value['uid']]);
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
          $author_record = $result->fetchObject();
        }      
      
        /** Node author */
        $node->uid = (int)$author_record->uid;
      
        /** Contributors */
        $hub_contributors = explode(', ', $value['contributors']);
      
        /** Contributor order */
        $contributor_order = explode(', ', $value['contributor_order']);      

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
          'value' => $value['field_description_value'],
          'format' => 'filtered_html',
        );
      
        /** Media type */
        $node->field_type[$node->language][0]['value'] = $value['field_cluster_type_value'];
      
        /** Video */
        if (($value['field_video_embed_link_embed'] != 'NULL')) {
          $node->field_video_embed_link[$node->language][0]['video_url'] = $value['field_video_embed_link_embed'];
        }
      
        /** Representative image */
      
        /**
        if (($data[$c]['filename'] != 'NULL')) {
      
          // Some file on our system
          $file_path = $script_path . $image_source . $data[$c]['filename'];
      
          $file = (object) array(
            'uid' => (int)$data[$c]['uid'],
            'uri' => $file_path,
            'filemime' => file_get_mimetype($file_path),
            'status' => 1,
          );
         
          $file = file_copy($file, 'public://'. $image_destination); // Save the file to the root of the files directory.
      
          $node->field_representative_image[$node->language][0] = (array)$file; //associate the file object with the image field:
      
        }
        */
      
        /** spokes */
      
      
        /**
         * 
         * We probably want to let Drupal do this for us with CNR, just so that wi can test that things are working
         * 
        $array = explode(", ", $data[$c]['contributed_pieces']);
      
        foreach ($array as $key => $value) {
          $node->field_spokes[$node->language][$key]['target_id'] = (int)$value;
        }
        */
      
        /** spoke_order */
      
        /**
         *
         * We probably want to let Drupal do this for us with CNR, just so that wi can test that things are working
         *      
        $array = explode(", ", $data[$c]['contributed_pieces_order']);
        foreach ($array as $key => $value) {
          $node->field_spokes[$node->language][$key]['_weight'] = (int)$value;
        }
        */
      
        /** dates */
        $node->field_field_period[$node->language][0]['value'] = $value['field_period_value'];

        $node->field_field_period[$node->language][0]['value2'] = $value['field_period_value2'];
      
        /** taxonomy */
        /** work on something similar to users */
        //$array = explode(", ", $value['terms']);
        //foreach ($array as $key => $value) {
          //$node->field_tags[$node->language][$key]['tid'] = (int)$value;
        //}

        /** Prepare node for saving */
        if ($node = node_submit($node)) {
          node_save($node);
          db_insert('mediacommons_import_node_map')
            ->fields(array(
              'onid' => $value['nid'],
              'nid' => $node->nid,
            )
          )->execute();
          drush_print("Node saved. " . url('node/' . $node->nid, array('absolute'=>TRUE)) . "\n");
        }
      }
      else {
        drush_print('Node already exist');
      }
    }
  }  
}

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

function mediacommons_import_run($task) {
  switch ($task) {

    case 1 :
      drush_print('Generating users');
      break;

    case 2 :
      drush_print('Generating hubs');
      mediacommons_import_generate_hubs();
      break;

    case 3 :
      drush_print('Generating spokes');
      mediacommons_import_generate_spokes();
      break;

    case 4 :
      drush_print('Generating random hubs and spokes');
      mediacommons_import_generate_random_content();
      break;

    case 5 :
      drush_print('Deleting all users');
      mediacommons_import_delete_users();
      break;

    case 6 :
      drush_print('Deleting all hubs and spokes');
      mediacommons_import_delete_content();
      break;
      
      case 7 :
        drush_print('Deleting hubs, spokes  and users');
        mediacommons_import_delete_users();
        mediacommons_import_delete_content();
        break;      

    default :
      drush_print('');
      drush_print('ERROR: Unable to perform task');
      drush_print('');
      mediacommons_import_show_options();
      break;
  }
}

function mediacommons_import_show_help() {
  drush_print('');
  drush_print(t('[1] Generate users'));
  drush_print(t('[2] Generate hubs'));
  drush_print(t('[3] Generate spokes'));
  drush_print(t('[4] Generate random hubs and spokes'));
  drush_print(t('[5] Delete users'));
  drush_print(t('[6] Delete all content'));
  drush_print(t('[7] Delete all user and content'));
  drush_print('');
}

function mediacommons_import_show_options() {
  drush_print(t('Please type one of the following options to continue:'));
  mediacommons_import_show_help();
  $handle = fopen ("php://stdin","r");
  $line = fgets($handle);
  mediacommons_import_run(trim($line));
}

function mediacommons_import_init() {
  mediacommons_import_prepare();
  $args = func_get_args();
  if (isset($args[1])) {
    mediacommons_import_run($args[1]);
  }
  else {
    mediacommons_import_show_options();
  }
}

mediacommons_import_init();
