<?php

function mediacommons_survey_import_migrate_all_survey_response() {

  /**
   * Load data
   */
  $nodes = mediacommons_survey_import_d6_survey_response_list();

  /**
   * Migrate data
   */
  mediacommons_survey_import_migrate_survey_response( $nodes );
  
}

function mediacommons_survey_import_migrate_survey_response( $data = array() ) {
  
  /**
   * Files folder real path
   */
  $dupal_public_file_path = variable_get('file_public_path', conf_path() . '/files');   
  
  foreach ($data as $key => $spoke) {
  	
    $path_alias = mediacommons_base_import_find_url_alias('node/' . $spoke->nid);

    /** 
     * Find out if node is already in the system 
     */
    $node_exist = mediacommons_base_import_node_exists($spoke->nid);

    /**
     * Find out if user is already in the system 
     */
    $author_uid = mediacommons_base_import_user_exists($spoke->uid);

    /**
     * Conditions to be meet before creating content
     */
    if ( $node_exist ) {
      drush_log( t('Spoke "@title" already exist.', array('@title' => $spoke->title), 'warning' ) );
      continue;
    }

    if ( !$author_uid ) {
      drush_log( t("Can't import spoke @title author does not exist in system. Please make sure the author is created before creating content under his username", array('@title' => $spoke->title)), 'warning' );
      continue;
    }
      
    /**
     * Prepare node
     */
    $node = array();

    $node['title'] = $spoke->title;

    $node['ouid'] = $spoke->uid;

    $node['onid'] = $spoke->nid;

    $node['language'] = LANGUAGE_NONE;

    $node['date'] = date('Y-m-j H:i:s', $spoke->created);

    $node['status'] = $spoke->status;

    $node['promote'] = $spoke->promote;

    $node['moderate'] = $spoke->moderate;

    $node['sticky'] = $spoke->sticky;
    
    /**
     * Attached images
     * @var unknown_type
     */

    $node['field_attached_images'] = array();
    

    if ( isset( $spoke->images ) ) {
    	
      $images = explode(',', $spoke->images);
    	
      foreach ( (array) $images as $image ) {
    	  
    	$fid = (int) trim($image);
    	  
    	$attached_image = mediacommons_survey_import_d6_attached_images_by_fid( $fid );
    	
    	if ( $attached_image ) {
    		
    	  $find = str_replace("/", "\/", $image->filename);
    	 
    	  $pattern = "/" . $find . "/i";

    	  preg_match($pattern, $node['field_body'], $matches);

    	  if ( $matches ) {

    		$node['field_body'] = str_replace($matches[0], $dupal_public_file_path . '/' . basename($image->filepath), $node['field_body']);

    		unset($matches);

    	  } 

    	  // $fe = str_replace("files/","", $attached_image->filepath);

    	  // $attached_image->filepath = 'files/' . urlencode($fe);
    	  
    	  $node['field_attached_images'][] = $attached_image;

    	}

      }

    }

    /*
     * Field body
     */
    if ( isset( $spoke->field_response_value ) ) {

      $node['field_body'] = $spoke->field_response_value;

      $pattern = "/style=\"(.*)\"/i";

      preg_match($pattern, $node['field_body'], $matches);

      if ( $matches ) {
        
      	$node['field_body'] = str_replace($matches[0], '', $node['field_body']);
        
      	unset( $matches );
      	  
      }

    }

    /*
     * Publication Status
     */
    $node['field_pubstat'] = $spoke->status;

    mediacommons_base_import_create_spoke($node);
    
  }

};