<?php


$repo_path = "/Applications/MAMP/htdocs/mcd7temp/mediacommons/";
$script_folder = '/import-scripts';
$script_path = "/Applications/MAMP/htdocs/mcd7temp/mediacommons/" . $script_folder;
$image_destination = '/images/teaser-images/';
$csv_file = 'export-hub-3-20-13-5-14 PM.csv';

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

if (($handle = fopen($script_path . $csv_file, "r")) !== FALSE) {
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
    //$node->type = $data[$c]['type'];
    $node->type = 'hub';
    $node->language = LANGUAGE_NONE;
    $node->status = $data[$c]['status'];
    $node->promote = 0;
    $node->sticky = 0;
    node_object_prepare($node);
    
    $node->title = $data[$c]['title'];
    $node->uid = (int)$data[$c]['uid'];
    
    //$node->created = (int)$data[$c]['created'];
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
    $node->field_description[$node->language][0]['format'] = 'filtered_html';
    $node->field_type[$node->language][0]['value'] = $data[$c]['field_cluster_type_value'];
    if(($data[$c]['field_video_embed_link_embed'] != 'NULL')){
      $node->field_video_embed_link[$node->language][0]['video_url'] = $data[$c]['field_video_embed_link_embed'];
    }
    if(($data[$c]['image'] != 'NULL')){
      // Some file on our system
      $file_path = $script_path . $image_destination . $data[$c]['image'];
      
      $file = (object) array(
              'uid' => (int)$data[$c]['uid'],
              'uri' => $file_path,
              'filemime' => file_get_mimetype($file_path),
              'status' => 1,
            ); 
            $file = file_copy($file, 'public://images/teaser-images'); // Save the file to the root of the files directory. You can specify a subdirectory, for example, 'public://images' 
            $node->field_representative_image[$node->language][0] = (array)$file; //associate the file object with the image field:
    }
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
    $node->field_field_period[$node->language][0]['value'] = $data[$c]['field_period_value'];
    $node->field_field_period[$node->language][0]['value2'] = $data[$c]['field_period_value2'];
    
    
      if($node = node_submit($node)) { // Prepare node for saving
        //var_dump($node->created);
        node_save($node);
        echo "Node with nid " . $node->nid . " saved!\n";
        }
        
     }
     fclose($handle);
  }

/* Read more: http://zengenuity.com/blog/a/201011/importing-files-drupal-nodes-cck-fieldfield-imagefield#ixzz1v3tz75My
http://fooninja.net/2011/04/13/guide-to-programmatic-node-creation-in-drupal-7/
http://drupal.org/node/330421#comment-2806336
http://www.group42.ca/creating_and_updating_nodes_programmatically_in_drupal_7
*/