<?php

/**
 * Given an *old* node (Drupal 6), create a frontpage node.
 */
function mediacommons_mc_import_create_frontpage(array $old_node) {

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
  $new_node = _mediacommons_base_import_setup_node('front_page_post');
  
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
      t('Front page @title already exist.', array('@title' => $old_node->title))
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

  $new_node->language = LANGUAGE_NONE;

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
  
  if (isset($old_node->field_link)) {
    $new_node->field_link[$new_node->language][0] = array(
      'url' => $old_node->field_link->link,
      'title' => $old_node->field_link->title,
    );
  }
  
  if (isset($old_node->field_project)) {
    $new_node->field_project[$new_node->language][0] = array(
      'value' => $old_node->field_project,
    );
  }  
        
  /** 
   * Attached representative image
   */
  if (isset($old_node->field_representative_image)) {
    
  	if (
  	  isset($old_node->field_representative_image->fid)
  	) {
  
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
    
    $put_success = file_put_contents($source_image_path, mediacommons_base_import_get_content($url_image_path));
    
    if ($put_success) {
    
      $file = (object) array(
        'uid' => (int)$new_node->uid,
        'uri' => $source_image_path,
        'filemime' => file_get_mimetype($source_image_path),
        'status' => 1,
        'cropbox_x' => (int)$old_node->field_representative_image->x,
        'cropbox_y' => (int)$old_node->field_representative_image->y,
        'cropbox_height' => (int)$old_node->field_representative_image->height,
        'cropbox_width' => (int)$old_node->field_representative_image->width,
      );

      $file = file_copy($file, 'public://');

      $new_node->field_representative_image[LANGUAGE_NONE][] = (array)$file;
      
    }    
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

    path_save($path);
        
    mediacommons_base_import_comment($new_node->nid, $old_node->onid);
        
    db_insert('mediacommons_base_import_node_map')->fields(
      array(
        'onid' => $old_node->onid,
        'nid' => $new_node->nid,
      )
    )->execute();
        
    drush_print(
      t('Front page node saved.')
    );
      
  }
  
}

function mediacommons_mc_import_migrate_all_frontpage() {
  mediacommons_mc_import_migrate_frontpage(
    mediacommons_mc_import_d6_frontpage_list()
  );
}

function mediacommons_mc_import_migrate_frontpage($data = array()) {

  /**
   * Files folder real path
   */
  $dupal_public_file_path = variable_get('file_public_path', conf_path() . '/files');   

  foreach ($data as $key => $hub) {
      
    /** Prepare node */
    
  	$node = array();

    $node['title'] =  $hub->title;

    $node['ouid'] = $hub->uid;

    $node['onid'] = $hub->nid;

    $node['language'] = (!empty($hub->language)) ? $hub->language : LANGUAGE_NONE;

    $node['date'] = date('Y-m-j H:i:s', $hub->created);

    $node['status'] = $hub->status;

    $node['promote'] = $hub->promote;

    $node['moderate'] = $hub->moderate;

    $node['sticky'] = $hub->sticky;
      
    if (isset($hub->field_fpp_description_value)) {
      $node['field_body'] = strip_tags($hub->field_fpp_description_value);
    }

    if (isset($hub->field_fpp_image_fid)) {

      $x = $y = $height = $width = 0;

      if (isset($hub->field_fpp_image_data)) {
        $data = unserialize($hub->field_fpp_image_data);
        //$x = empty($data['crop']['x']) ? 0 : (int)$data['crop']['x'];
        //$y = empty($data['crop']['y']) ? 0 : (int)$data['crop']['y'];
        $height = empty($data['crop']['height']) ? 0 : (int)$data['crop']['height'];
        $width = empty($data['crop']['width']) ? 0 : (int)$data['crop']['width'];
      }

      $node['field_representative_image'] = (object) array( 
        'fid' => $hub->field_fpp_image_fid,
        'x' => $x,
        'y' => $y,        
        'height' => $height,
        'width' => $width,                
      );
      
      unset($data);
      unset($x);
      unset($y);
      unset($height);
      unset($width);
      
    }
    
    if (isset($hub->field_fpp_link_url) && isset($hub->field_fpp_link_title)) {

      $node['field_link'] = (object) array(
        'title' => $hub->field_fpp_link_title,
        'link' => $hub->field_fpp_link_url,
      );
    }
    
    if (isset($hub->field_fpp_project_value)) {
      $node['field_project'] = $hub->field_fpp_project_value;
    }
    
    mediacommons_mc_import_create_frontpage($node);

  }
}