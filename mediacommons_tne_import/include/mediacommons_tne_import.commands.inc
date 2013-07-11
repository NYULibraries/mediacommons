<?php

/**
 *
 */
function mediacommons_tne_import_migrate_contributed_pieces() {

  /**
   * Load data, this will take time; go get yourself a coffee 
   */
  $data = mediacommons_tne_import_d6_contributed_pieces_list();

  /** 
   * If we have $data proceed 
   */
  if (isset($data)) {

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

      $node['language'] = (!empty($spoke->language)) ? $spoke->language : LANGUAGE_NONE;

      $node['date'] = date('Y-m-j H:i:s', $spoke->created);

      $node['status'] = $spoke->status;

      $node['promote'] = $spoke->promote;

      $node['moderate'] = $spoke->moderate;

      $node['sticky'] = $spoke->sticky;

      if (isset($spoke->field_document_textarea_body_value)) {
        $node['field_body'] = $spoke->field_document_textarea_body_value;
      }

      $node['field_pubstat'] = $spoke->status;
      
      foreach (explode(', ', $spoke->terms) as $tid) {
        $node['field_tags'][] = $tid;
      }
      
      /** Additonal authors */
      if ($spoke->additonal_authors) {
      	
      	$node['field_contributors'] = array();
          
        /** Contributors */
        $spoke_contributors = explode(', ', $spoke->additonal_authors);

        /** Contributor order */
        $contributor_order = explode(', ', $spoke->additonal_authors_delta);

        foreach ($spoke_contributors as $key => $contributor) {
          $node['field_contributors'][$key] = array(
            'uid' => $contributor,
            '_weight' => (int)$contributor_order[$key],
          );
        }
      }
      
      /** 
       * Representative image
       */
      if (isset($spoke->thumbnail_fid)) {
        $node['field_attached_images_ri'] = $spoke->thumbnail_fid;
      }
      
      /**
       * Attached images
       * @var unknown_type
       */
      
      $node['field_attached_images'] = mediacommons_tne_import_d6_attached_images($spoke->nid);
      
      drush_print_r($node->field_attached_images);
      // drush_print_r($node);
      
      //mediacommons_base_import_create_spoke($node);

    }
  }

  drush_print(
    t('Done.')
  );

}

function mediacommons_tne_import_migrate_cluster() {

}

function mediacommons_tne_import_migrate_response() {

}

function mediacommons_tne_import_test() {
}