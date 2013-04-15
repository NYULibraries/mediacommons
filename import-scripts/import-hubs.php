<?php

/**
 * Read more: http://zengenuity.com/blog/a/201011/importing-files-drupal-nodes-cck-fieldfield-imagefield#ixzz1v3tz75My
 * http://fooninja.net/2011/04/13/guide-to-programmatic-node-creation-in-drupal-7/
 * http://drupal.org/node/330421#comment-2806336
 * http://www.group42.ca/creating_and_updating_nodes_programmatically_in_drupal_7
 */

/**
 * Assuming you have already a local copy of TNE, your settings.php should looks somtething like this:

 $databases['drupal6'] = array(
   'default' => array(
     'database' => 'db_name',
     'username' => 'your_user',
     'password' => 'your_password',
     'host' => '127.0.0.1',
     'port' => '',
     'driver' => 'mysql',
     'prefix' => 'prefix_',
   ),
 );

 */

include_once "mediacommons_import.install";
include_once "mediacommons_import.create.inc";
include_once "mediacommons_import.datasource.inc";
include_once "mediacommons_import.delete.inc";

function mediacommons_import_prepare() {
  if (!defined('__DIR__')) define('__DIR__', dirname(__FILE__));
  
  global $settigs;

  $settigs = mediacommons_import_settings();
  ini_set('memory_limit', '512M');
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
  if (!db_table_exists('mediacommons_import_vocabulary_map')) {
    $schema = mediacommons_import_schema();
    mediacommons_import_initialize_schema('mediacommons_import', $schema);
    db_create_table('mediacommons_import_vocabulary_map', $schema['mediacommons_import_vocabulary_map']);
  }  
  if (!db_table_exists('mediacommons_import_term_map')) {
    $schema = mediacommons_import_schema();
    mediacommons_import_initialize_schema('mediacommons_import', $schema);
    db_create_table('mediacommons_import_term_map', $schema['mediacommons_import_term_map']);
  }  
}

function mediacommons_import_settings() {
  $script = pathinfo(__FILE__);
  $settigs = array();
  $settigs = json_decode(utf8_encode(file_get_contents($script['dirname'] . '/settigs.json')), true);
  $settigs['script_path'] = $script;
  return $settigs;
}


/** Return a plain node template */
function _mediacommons_import_setup_node($type) {
  $node = new StdClass();
  $node->type = $type;
  $node->language = LANGUAGE_NONE;
  $node->promote = 0;
  $node->sticky = 0;
  /** Fills in a few default values */
  node_object_prepare($node);  
  return $node;
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

    case 8 :
      drush_print('Running test');
        drush_print_r(mediacommons_import_find_d6_user_roles(1));
        
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
  drush_print(t('[8] Run test'));
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