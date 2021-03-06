<?php

function migrate_clusters() {
  foreach (clusters() as $entity) migrate_cluster($entity);
}

function migrate_cluster($entity) {

  $node = array();

  $node['title'] =  $entity->title;

  $node['ouid'] = $entity->uid;

  $node['onid'] = $entity->nid;

  $node['language'] = LANGUAGE_NONE;

  $node['date'] = date('Y-m-j H:i:s' , $entity->created);

  $node['status'] = $entity->status;

  $node['field_pubstat'] = $entity->status;

  $node['promote'] = $entity->promote;

  $node['moderate'] = $entity->moderate;

  $node['sticky'] = $entity->sticky;

  if (isset($entity->field_description_value)) {
  	extract_iframe_sources($entity->field_description_value);
    $node['field_body'] = $entity->field_description_value;
  }

  /**
   * Representative image
   * Use media type to find image source:
   * 0 | Embedded video
   * 1 | Image
   */
  switch ( $entity->field_cluster_type_value ) {
  	case 1:
      // field_image_fid
      if (isset($entity->field_image_fid)) {
        $node['field_representative_image'] = (object) array('fid' => $entity->field_image_fid);
      }
      break;
    default:
      // field_image_fid
      if (isset($entity->field_tease_image_fid)) {
        $node['field_representative_image'] = (object) array('fid' => $entity->field_tease_image_fid);
      }
      else {
    	  // if youtube find image using the API
    	  if (
          isset($entity->field_video_embed_link_provider) &&
          $entity->field_video_embed_link_provider == 'youtube'
        ) {
    		  $node['field_representative_image'] = (object) array('source' => 'http://img.youtube.com/vi/' . $entity->field_video_embed_link_value  .'/0.jpg');
    	  }
      }
      if (isset($entity->field_video_embed_link_embed)) {
         // we need to find watch?v= and replace for embed/
         // /watch?v=
         
        // https://www.youtube.com/watch?v=WH5m8c-tQNI
        // should be
        // https://www.youtube.com/embed/WH5m8c-tQNI
      
        $temp = str_replace('watch?v=', 'embed/', $entity->field_video_embed_link_embed);
        // http://stackoverflow.com/questions/1252693/using-str-replace-so-that-it-only-acts-on-the-first-match#1252710
        $pos = strpos($temp, '&');
        if ($pos !== false) {
          $newstring = substr_replace($temp, '?', $pos, strlen('&'));
          $temp = $newstring;
        }
        
        $t = $node['field_body'];
        
        $node['field_body']  =  '<div class="embed">';
        $node['field_body'] .=  '<iframe src="' . $temp . '" frameborder="0" allowfullscreen>';
        $node['field_body'] .=  '<p>Your browser does not support iframes.</p>';
        $node['field_body'] .=  '</iframe>';
        $node['field_body'] .=  '</div>';
        $node['field_body'] .=  '<div class="body_content">' . $t . '</div>';
        unset($temp);
      }
  }

  if ( isset ( $entity->pieces ) ) $node['field_spokes'] = explode( ',', $entity->pieces ) ;

  if ( isset ( $entity->contributors ) ) $node['field_contributors'] = explode ( ',', $entity->contributors ) ;

  if (isset ($entity->terms)) {
    foreach (explode (', ', $entity->terms ) as $tid) {
      if (!empty($tid)) $node['field_tags'][] = $tid;
    }
  }

  /**
   * Dates
   * If $dates is set proceed, no good comes from setting dates with wrong values
   */
  if ( isset($entity->field_period_value) ) $node['field_period_1'] = strtotime ( $entity->field_period_value ) ;

  if ( isset($entity->field_period_value2) ) $node['field_period_2'] = strtotime ( $entity->field_period_value2 ) ;

  create_hub ( $node ) ;

}
