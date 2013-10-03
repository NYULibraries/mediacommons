<?php

function mediacommons_altac_import_migrate_all_contributed_pieces() {

  /**
   * Load data
   */
  mediacommons_altac_import_migrate_contributed_pieces(
    mediacommons_altac_import_d6_contributed_pieces_list()
  );

}

/**
 * @TODO
 */
function mediacommons_altac_import_migrate_contributed_pieces($data = array()) {
  
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
    if ($node_exist) {
      drush_print(
        t(
          'Spoke "@title" already exist.', 
          array('@title' => $spoke->title)
        )
      );
      continue;
    }

    if (!$author_uid) {
      drush_print(
        t("Can't import spoke @title author does not exist in system. Please make sure the author is created before creating content under his username", array('@title' => $spoke->title))
      );
      continue;
    }
      
    /**
     * Prepare node
     */
    $node = array();

    $node['title'] = $spoke->title;

    $node['ouid'] = $spoke->uid;

    $node['onid'] = $spoke->nid;

    // $node['language'] = (!empty($spoke->language)) ? $spoke->language : LANGUAGE_NONE;
    
    $node['language'] = LANGUAGE_NONE;

    $node['date'] = date('Y-m-j H:i:s', $spoke->created);

    $node['status'] = $spoke->status;

    $node['promote'] = $spoke->promote;

    $node['moderate'] = $spoke->moderate;

    $node['sticky'] = $spoke->sticky;
    
    /*
     * Field body
     */
    if (isset($spoke->field_document_textarea_body_value)) {
      $node['field_body'] = $spoke->field_document_textarea_body_value;

      $pattern = "/style=\"(.*)\"/i";

      preg_match($pattern, $node['field_body'], $matches);

      if ($matches) {
        $node['field_body'] = str_replace($matches[0], '', $node['field_body']);
        unset($matches);  
      }

    }

    /*
     * Publication Status
     */
    $node['field_pubstat'] = $spoke->status;

    /*
     * Tagline
     * @TODO: We don't have a migration path for this field. Ask Mark  about what we want to do with this
     */    
    if (isset($spoke->field_tagline)) {
      $node['field_tagline'] = $spoke->field_tagline;
    }

    /*
     * Tags
     */
    foreach (explode(', ', $spoke->terms) as $tid) {
      $node['field_tags'][] = $tid;
    }
      
    /** Additional authors */
    if ($spoke->additonal_authors) {

      $node['field_contributors'] = array();

      /** Contributors */
      $spoke_contributors = explode(', ', $spoke->additonal_authors);

      /** Contributor order */
  //  $contributor_order = explode(', ', $spoke->additonal_authors_delta);
      foreach ($spoke_contributors as $key => $contributor) {
        $node['field_contributors'][$key] = array(
            'uid' => $contributor,
            '_weight' => $key,
         // '_weight' => (int)$contributor_order[$key],
        );
      }
    }

    /** 
     * Representative image
     */
    if (isset($spoke->thumbnail_fid)) {
      $node['field_attached_images_ri'] = (object) array( 'fid' => $spoke->thumbnail_fid, );
    }
      
    /**
     * Attached images
     * @var unknown_type
     */
    $node['field_attached_images'] = array();

    $attached_images = mediacommons_altac_import_d6_attached_images($spoke->nid);

    foreach ($attached_images as $key => $image) {
      $node['field_attached_images'][] = $image;
      $find = str_replace("/", "\/", $image->filepath);
      $pattern = "/" . $find . "/i";
      preg_match($pattern, $node['field_body'], $matches);
      if ($matches) {
        $node['field_body'] = str_replace($matches[0], $dupal_public_file_path . '/' . basename($image->filepath), $node['field_body']);
        unset($matches);  
      }
      if ($image->filename != '_original') {
        $image->skip = TRUE;
      }
    }
      
    mediacommons_base_import_create_spoke($node);
      
  }

}