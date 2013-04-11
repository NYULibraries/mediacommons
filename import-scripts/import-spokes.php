<?php


$repo_path = "/Applications/MAMP/htdocs/mcd7temp/mediacommons/";
$script_folder = 'import-scripts/';
$script_path = "/Applications/MAMP/htdocs/mcd7temp/mediacommons/" . $script_folder;

$image_source = '/images/contributed-pieces/';
$image_destination = '/images/contributed-pieces/';
$csv_file = 'export-spokes-4-11-13-8-15 AM.csv';

$row = 0;
$header = NULL;
$data = array();

if (($handle = fopen($script_path . $csv_file, "r")) !== FALSE) {
  while (($row = fgetcsv($handle, 1000, ",")) !== FALSE) {
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
    $node->type = 'spoke';
    $node->language = LANGUAGE_NONE;
    $node->status = $data[$c]['status'];
    $node->promote = 0;
    $node->sticky = 0;
    node_object_prepare($node);
    $node->title = $data[$c]['title'];
    $node->uid = (int)$data[$c]['uid'];
    $node->created = (int)$data[$c]['created'];
    $node->changed = $data[$c]['changed'];
    $node->comment = $data[$c]['comment'];
    
    // additonal_authors
    
    if(($data[$c]['additonal_authors'] != 'NULL')) {
          $array = explode(", ", $data[$c]['additonal_authors']);
          foreach ($array as $key => $value) {
            $node->field_contributors[$node->language][$key]['uid'] = (int)$value;
            
          }
        }
    
    // cluster_nid
    
    if(($data[$c]['cluster_nid'] != 'NULL')) {
      //$node->field_part_of_hub[$node->language][$key]['nid'] = (int)$data[$c]['cluster_nid'];
    }
    // description
    $node->field_body[$node->language][0]['value'] = $data[$c]['field_document_textarea_body_value'];
    $node->field_body[$node->language][0]['format'] = 'filtered_html';
    // publication status
    if($data[$c]['field_pubstat_value'] == 'Published') {
      $node->field_pubstat[$node->language][0]['value'] = 1;
    } elseif($data[$c]['field_pubstat_value'] == 'Unpublished')  {
      $node->field_pubstat[$node->language][0]['value'] = 0;
    }
    //representative image
    if(($data[$c]['filename'] != 'NULL')){
      // Some file on our system
      $file_path = $script_path . $image_source . $data[$c]['filename'];
      
      $file = (object) array(
                   'uid' => (int)$data[$c]['uid'],
                   'uri' => $file_path,
                   'filemime' => file_get_mimetype($file_path),
                   'status' => 1,
                 ); 
           $file = file_copy($file, 'public://'. $image_destination); // Save the file to the root of the files directory. 
           
           // ALBERTO I'm having trouble loading the file into the attached image table
           
           //$node->field_attached_images[$node->language][0] = (array)$file; //associate the file object with the image field:
           $node->field_representative_image[$node->language][0] = (array)$file; //associate the file object with the image field:
     
    }
    //attached images
    if(($data[$c]['attached_image_filepath'] != 'NULL')){
      $array = explode(", ", $data[$c]['attached_image_filepath']);
      foreach ($array as $key => $value) {
      $filename = pathinfo($value, PATHINFO_BASENAME);
      $file_path = $script_path . $image_source . $filename;
      $file = (object) array(
                    'uid' => (int)$data[$c]['uid'],
                    'uri' => $file_path,
                    'filemime' => file_get_mimetype($file_path),
                    'status' => 1,
                  ); 
           $file = file_copy($file, 'public://'. $image_destination); // Save the file to the root of the files directory. 
            //$node->field_attached_images[$node->language][0] = (array)$file; //associate the file object with the image field:
      
      }
    }
    // taxonomy
    $array = explode(", ", $data[$c]['terms']);
    foreach ($array as $key => $value) {
      //$node->field_tags[$node->language][$key]['tid'] = (int)$value;
    }
    
    if($node = node_submit($node)) { // Prepare node for saving
        //var_dump($node->created);
        node_save($node);
        echo "Node with nid " . $node->nid . " saved!\n";
    }
  }
  fclose($handle);
}

/*
Read more: http://zengenuity.com/blog/a/201011/importing-files-drupal-nodes-cck-fieldfield-imagefield#ixzz1v3tz75My
http://fooninja.net/2011/04/13/guide-to-programmatic-node-creation-in-drupal-7/
http://drupal.org/node/330421#comment-2806336
http://www.group42.ca/creating_and_updating_nodes_programmatically_in_drupal_7
http://timonweb.com/how-programmatically-create-nodes-comments-and-taxonomies-drupal-7
*/