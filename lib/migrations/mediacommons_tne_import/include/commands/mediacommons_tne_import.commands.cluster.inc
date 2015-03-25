<?php

function mediacommons_tne_import_add_hub_allowed_iframe_sources () {

	$d6_domains = mediacommons_tne_import_hub_iframe_sources () ;

	$result = db_query("SELECT * FROM {mediacommons_filter}");

	$domains = array();

	foreach ( $result as $record ) $domains[$record->did] = $record->domain ;

	foreach ( $d6_domains as $d6_domain ) {
		if ( ! in_array( $d6_domain, $domains ) ) {
			drush_log ( 'Adding ' . $d6_domain . ' to allowed iframe source list' , 'status' );
			db_insert('mediacommons_filter')->fields( array('domain' => $d6_domain, ) )->execute();
		}
	}

}

function mediacommons_tne_import_migrate_all_clusters() {
  /**
   * Load all clusters
   */
  mediacommons_tne_import_migrate_clusters(
    mediacommons_tne_import_d6_cluster_list()
  );
}

function mediacommons_tne_import_migrate_clusters($data = array()) {

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
      
    if (isset($hub->field_description_value)) {
      $node['field_body'] = $hub->field_description_value;
      $pattern = "/style=\"(.*)\"/i";
      preg_match($pattern, $node['field_body'], $matches);
      if ($matches) {
        $node['field_body'] = str_replace($matches[0], '', $node['field_body']);
        unset($matches);  
      }
    }
      
    /** 
     * Representative image
     * Use media type to find image source:
     * 0 | Embedded video
     * 1 | Image
     */      
    switch ($hub->field_cluster_type_value) {
      case 1:

        // field_image_fid
        if (isset($hub->field_image_fid)) {
          $node['field_representative_image'] = (object) array( 'fid' => $hub->field_image_fid, );
        }
        break;

      default:

      // field_image_fid
      if (isset($hub->field_tease_image_fid)) {
        $node['field_representative_image'] = (object) array( 'fid' => $hub->field_tease_image_fid, );
      }
      
      else {
      	// if youtube find image using the API
      	if (isset($hub->field_video_embed_link_provider) && $hub->field_video_embed_link_provider == 'youtube') {
      		$node['field_representative_image'] = (object) array( 'source' => 'http://img.youtube.com/vi/' . $hub->field_video_embed_link_value  .'/0.jpg', );
      	}
      }
      
      if (isset($hub->field_video_embed_link_embed)) {
      	
      	$temp = $node['field_body'];
      	
        $node['field_body'] = '<div class="embed">' . $hub->field_video_embed_link_embed . '</div>' . '<div class="body_content">' . $temp . '</div>';

      }
    }
      
    $node['field_pubstat'] = $hub->status;

    if (isset($hub->pieces)) {
      $node['field_spokes'] = explode(", ", $hub->pieces);
    }

    if (isset($hub->contributors)) {
      $node['field_contributors'] = explode(", ", $hub->contributors);
    }

    /** 
     * Dates
     * If $dates is set proceed, no good comes from setting dates with wrong values
     */
     if ( isset($hub->field_period_value) ) {
      $node['field_period_1'] = strtotime($hub->field_period_value);
     }

     if ( isset($hub->field_period_value2) ) {
      $node['field_period_2'] = strtotime($hub->field_period_value2);
     }

     mediacommons_base_import_create_hub($node);

  }
}