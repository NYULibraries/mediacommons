<?php

// define('DRUPAL_ROOT', getcwd());
// $_SERVER['REMOTE_ADDR'] = "localhost"; // Necessary if running from command line
// //require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
// drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
// require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
// module_load_include('inc', 'node', 'node.pages');

define('DRUPAL_ROOT', getcwd());
include_once 

DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
bootstrap_invoke_all('init');
ini_set('memory_limit', '512M');
//Authenticate as user 1
user_authenticate('user', 'password');
module_load_include('inc', 'node', 'node.pages');  // new for Drupal 6
$row = 0;
$header = NULL;
$data = array();

if (($handle = fopen("/Applications/MAMP/htdocs/mcd7temp/mediacommons/import-scripts/export-hub-3-13-13-2-18 PM.csv", "r")) !== FALSE) {
//if (($handle = fopen("/www/sites/mediacommons/script-tmp/export-hub-3-13-13-2-18 PM.csv", "r")) !== FALSE) {
  while (($row = fgetcsv($handle, 1000, ",")) !== FALSE) {
  //print_r($row);
    if(!$header) {
      $header = $row;
    } else {
      $data[] = array_combine($header, $row);
    }
  }
  $num = count($data);
  for ($c=0; $c < $num; $c++) {
    $node = new StdClass();
    
    
    //$node->nid = $data[$c]['nid'];
    
    $node->type = $data[$c]['type'];
    $node->language = LANGUAGE_NONE;
    node_object_prepare($node);
    
    
    $form_state = array();
    $form_state['values']['status'] = $data[$c]['status'];
    $form_state['values']['title'] = $data[$c]['title'];   // the node's title
    //$form_state['values']['body'] = 'just my test node'; // the body, not required
    $node->field_contributors[$node->language][]['value'] = $data[$c]['contributors'];
    //$form_state['values']['promote'] = 1; //promote all imported nodes
    $form_state['values']['sticky'] = 0; //remove sticky from imported nodes
    //$form_state['values']['image'] = array();
    $form_state['values']['name'] = 'reilly';
    $form_state['values']['op'] = t('Save');  // this seems to be a required value
    drupal_form_submit("{$node->type}_node_form", $form_state, $node);
    
    //$node->title = $data[$c]['title'];
    // $node->uid = $data[$c]['uid'];
    //   $node->status = $data[$c]['status'];
    //   $node->created = $data[$c]['created'];
    //   $node->changed = $data[$c]['changed'];
    //   $node->comment = $data[$c]['comment'];
    //$node->field_contributors[$node->language][]['value'] = $data[$c]['contributors'];
    //$node->field_response[0]['value'] = $data[$c]['field_response_value'];
    //$node->field_response[0]['format'] = $data[$c]['field_response_format'];
    //$node->field_sr_reference[0]['nid']  = $data[$c]['field_sr_reference_nid'];
    /*if(!empty($data[$c]['filename'])){
    //$file = field_file_save_file('/Users/mar22/Dropbox/NYU/MediaCommons/redesign/Frontpage-tweaks-2013/content/images/'.$data[$c]['filename'], array(), 'files');
    $file = field_file_save_file('/www/sites/mediacommons/script-tmp/images/'.$data[$c]['filename'], array(), 'files/front_page_images');
    $node->field_resp_image = array($file);
  }*/
      // if($node = node_submit($node)) { // Prepare node for saving
      //                                                     node_save($node);
      //                                                     echo "Node with nid " . $node->nid . " saved!\n";
                                    }
      // node_save($node);
      // print_r($node);


    }
     fclose($handle);
  }

/* Read more: http://zengenuity.com/blog/a/201011/importing-files-drupal-nodes-cck-fieldfield-imagefield#ixzz1v3tz75My
http://fooninja.net/2011/04/13/guide-to-programmatic-node-creation-in-drupal-7/
*/