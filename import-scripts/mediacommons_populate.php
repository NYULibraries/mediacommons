<?php

if (!defined('__DIR__') ) define('__DIR__', dirname(__FILE__));

ini_set('memory_limit', '512M');

function mediacommons_populate_generate_users($new_users_per_rol = 3) {

  drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_FULL);
  
  $sharedemail_enable = FALSE;
  $sharedemail_path = drupal_get_path('module', 'sharedemail');
  
  if (empty($sharedemail_path)) {
    drush_print('I need sharedemai module in order to use the same email for all the accounts');
    return;
  }

  drush_set_option('password', 'dlts2010'); 
  
  /** make sure we can user the same email */
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

function mediacommons_populate_generate_hubs($new_hubs = 4) {

  drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_FULL);
  
  // http://digg.com/rss/top.rss
  // http://digg.com/rss/popular.rss
  
  $feed = 'http://digg.com/rss/popular.rss';

  /** find users */
  $user_roles = db_query("SELECT DISTINCT uid FROM {users_roles} WHERE uid <> 1 AND (rid <> 1 OR rid <> 2)", array());

  /** fetch users */
  $user = $user_roles->fetchAll();
  
  //$xml = simplexml_load_file(__DIR__ . "/top.rss");
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

function mediacommons_populate_run($task) {
  switch ($task) {

    case 0 :
      drush_print('Genereting users (password for each user will be dlts2010');
      mediacommons_populate_generate_users();
      drush_print('Genereting hubs and spokes');
      mediacommons_populate_generate_hubs();
      drush_print('Password for each user will be dlts2010)'); 
      break;

    case 1 :
      drush_print('Genereting users');
      mediacommons_populate_generate_users();
      drush_print('Password for each user will be dlts2010');       
      break;
      
    case 2 :
      drush_print('Genereting hubs and spokes');
      mediacommons_populate_generate_hubs();
      break;

    default : 
      drush_print('');
      drush_print('ERROR: Unable to perform task');
      drush_print('');
      mediacommons_populate_show_options();
      break;
  }
}

function mediacommons_populate_show_help() {
  drush_print('');
  drush_print('[0] Do all');
  drush_print('[1] Step 1: Generate users');
  drush_print('[2] Step 2: Generate content');  
  drush_print('');
}

function mediacommons_populate_show_options() {
  drush_print('Please type one of the following options to continue:');
  mediacommons_populate_show_help();
  $handle = fopen ("php://stdin","r");
  $line = fgets($handle);
  mediacommons_populate_run(trim($line));
}

function mediacommons_populate_init() {
  $args = func_get_args();
  if (isset($args[1])) {
    mediacommons_populate_run($args[1]);
  } 
  else {
    mediacommons_populate_show_options();
  }
}

mediacommons_populate_init();