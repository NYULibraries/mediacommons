<?php

// define('DRUPAL_ROOT', getcwd());
// $_SERVER['REMOTE_ADDR'] = "localhost"; // Necessary if running from command line
// //require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
// drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
// require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
// module_load_include('inc', 'node', 'node.pages');

define('DRUPAL_ROOT', getcwd());
$_SERVER['REMOTE_ADDR'] = "localhost"; // Necessary if running from command line
require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
bootstrap_invoke_all('init');
ini_set('memory_limit', '512M');
//Authenticate as user 1
//user_authenticate('user', 'password');
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
    //$node = node_load($nid);
    $node->type = $data[$c]['type'];
    $node->language = $data[$c]['language'];
    $node->status = $data[$c]['status'];
    $node->promote = 0;
    $node->sticky = 0;
    node_object_prepare($node);
    
    $node->title = $data[$c]['title'];
    $node->uid = $data[$c]['uid'];
    
    $node->date = (int)$data[$c]['created'];
    //$node->changed = $data[$c]['changed'];
    $node->comment = $data[$c]['comment'];
    // contributors
    $array = explode(", ", $data[$c]['contributors']);
    foreach ($array as $key => $value) {
      $node->field_contributors[$node->language][$key]['uid'] = (int)$value;
    }
    // contributor_order
    $array = explode(", ", $data[$c]['contributor_order']);
    foreach ($array as $key => $value) {
      $node->field_contributors[$node->language][$key]['_weight'] = (int)$value;
    }
    $node->field_description[$node->language][0]['value'] = $data[$c]['field_description_value'];
    $node->field_description[$node->language][0]['format'] = $data[$c]['field_description_format'];
    $node->field_type[$node->language][0]['value'] = $data[$c]['field_type_value'];
    $node->field_video_embed_link[$node->language][0]['video_url'] = $data[$c]['field_video_embed_link_video_url'];
    $node->field_representative_image_fid[$node->language][0]['fid'] = $data[$c]['field_representative_image_fid'];
    // spokes
    $array = explode(", ", $data[$c]['contributed_pieces']);
    foreach ($array as $key => $value) {
      $node->field_spokes[$node->language][$key]['target_id'] = (int)$value;
    }
    // spoke_order
    $array = explode(", ", $data[$c]['contributed_pieces_order']);
    foreach ($array as $key => $value) {
      $node->field_spokes[$node->language][$key]['_weight'] = (int)$value;
    }
    $node->field_field_period[$node->language][0]['value'] = $data[$c]['field_field_period_value'];
    $node->field_field_period[$node->language][0]['value2'] = $data[$c]['field_field_period_value2'];
    /*if(!empty($data[$c]['filename'])){
    //$file = field_file_save_file('/Users/mar22/Dropbox/NYU/MediaCommons/redesign/Frontpage-tweaks-2013/content/images/'.$data[$c]['filename'], array(), 'files');
    $file = field_file_save_file('/www/sites/mediacommons/script-tmp/images/'.$data[$c]['filename'], array(), 'files/front_page_images');
    $node->field_resp_image = array($file);
  }*/
  var_dump($node->date);
      if($node = node_submit($node)) { // Prepare node for saving
        var_dump($node->created);
        node_save($node);
        echo "Node with nid " . $node->nid . " saved!\n";
        }
        
     }
     fclose($handle);
  }

/* Read more: http://zengenuity.com/blog/a/201011/importing-files-drupal-nodes-cck-fieldfield-imagefield#ixzz1v3tz75My
http://fooninja.net/2011/04/13/guide-to-programmatic-node-creation-in-drupal-7/
*/