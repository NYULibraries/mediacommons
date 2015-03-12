<?php

function mediacommons_altac_import_migrate_all_contributed_pieces () {

  /** Load data */
  mediacommons_altac_import_migrate_contributed_pieces ( mediacommons_altac_import_d6_contributed_pieces_list() ) ;

}

function mediacommons_altac_import_migrate_contributed_pieces( $data = array() ) {

  /** Files folder real path */
  $dupal_public_file_path = variable_get('file_public_path', conf_path() . '/files');

  foreach ( $data as $key =>$entity ) {

    $path_alias = mediacommons_base_import_find_url_alias( 'node/' . $entity->nid );

    /** Find out if node is already in the system */
    $node_exist = mediacommons_base_import_node_exists( $entity->nid ) ;

    /** Find out if user is already in the system */
    $author_uid = mediacommons_base_import_user_exists( $entity->uid ) ;

    /** Conditions to be meet before creating content */
    if ( ! $node_exist ) {

      if ( $author_uid ) {

        /** Prepare node */
        $node = array();

        $node['title'] = $entity->title;

        $node['ouid'] = $entity->uid;

        $node['onid'] = $entity->nid;

        $node['language'] = LANGUAGE_NONE;

        $node['date'] = date('Y-m-j H:i:s',$entity->created);

        $node['status'] = $entity->status;

        $node['promote'] = $entity->promote;

        $node['moderate'] = $entity->moderate;

        $node['sticky'] = $entity->sticky;

        /** Field body */
        if ( isset ( $entity->field_document_textarea_body_value ) ) $node['field_body'] = $entity->field_document_textarea_body_value ;

        /** Publication Status */
        $node['field_pubstat'] = $entity->status;

        /**
         * Tagline
         * @TODO: We don't have a migration path for this field. Ask Mark  about what we want to do with this
         */
        if ( isset ($entity->field_tagline ) ) $node['field_tagline'] = $entity->field_tagline ;

        /** Tags */
        foreach ( explode(', ',$entity->terms) as $tid ) $node['field_tags'][] = $tid ;

        /** Additional authors */
        if ( $entity->additonal_authors ) {

          $node['field_contributors'] = array();

          /** Contributors */
          foreach ( explode(', ', $entity->additonal_authors ) as $key => $contributor ) $node['field_contributors'][$key] = array( 'uid' => $contributor, '_weight' => $key ) ;

        }

        /** Attached images */
        $node['field_attached_images'] = array();

        /** Representative image */
        if ( isset ( $entity->thumbnail_fid ) ) $node['field_attached_images_ri'] = (object) array( 'fid' =>$entity->thumbnail_fid ) ;


        $attached_images = mediacommons_altac_import_d6_attached_images ( $entity->nid ) ;

        foreach ( $attached_images as $key => $image ) $node['field_attached_images'][] = $image ;

        mediacommons_base_import_create_spoke ( $node ) ;

      }

      else drush_log( dt('Unable to import "@title" author does not exist in system.', array ( '@title' =>$entity->title ) ) , 'warning' ) ;

    }

    else drush_log ( dt( '"@title" already exist.', array('@title' =>$entity->title) ) , 'warning' ) ;

  }

}