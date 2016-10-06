<?php

function clean_images_paths ( $text ) {
  include_once ( libraries_get_path('simplehtmldom') . '/simple_html_dom.php' );
  $html = str_get_html ( $text ) ;
  // Find all img
  foreach ( $html->find('img') as $element ) {
    $parse_url = parse_url ( $element->src ) ;
    $match = preg_match ( '/mediacommons.futureofthebook.org/i' , $parse_url['host'] ) ;

    if ( isset ( $parse_url['host'] ) && ! $match ) { } // nothing to do

    else if ( isset ( $parse_url['host'] ) && $match ) { } // nothing to do

    else {
      $first_character = mb_substr ( $element->src , 0 , 1 , 'utf-8' ) ;
      if ( $first_character == '/' ) {
        $production_url = mediacommons_base_import_get_production_url ( ) . ltrim ( $element->src, '/' ) ;
        $environment_url = environment_url () ;
        $filepath = pathinfo ( $production_url ) ;
        if ( ! $image_by_filename = find_image_by_filename ( $filepath['basename'] ) ) {
          mediacommons_base_import_download_file ( $production_url ) ;
          if ( $wrapper = file_stream_wrapper_get_instance_by_uri('public://' . $filepath['basename'] ) ) {
            // the file directory returns "http://default/" change it for the site URL
            if ( $url = str_replace ( 'http://default/' , $environment_url ,  $wrapper->getExternalUrl() ) ) {
              $element->src = $url ;
            }
          }
        }
        else {
          if ( $wrapper = file_stream_wrapper_get_instance_by_uri('public://' . $filepath['basename'] ) ) {
            // the file directory returns "http://default/" change it for the site URL
            if ( $url = str_replace ( 'http://default/' , $environment_url ,  $wrapper->getExternalUrl() ) ) {
              $element->src = $url ;
            }
          }
        }
      }
    }
  }
  return (string) $html ;
}

function clean_embed_field_input ( $text ) {

  include_once ( libraries_get_path('simplehtmldom') . '/simple_html_dom.php' );

  $source = array ( ) ;

  $elemets = array ( 'object' => array ( ) , 'embed' => array ( ) , 'div' =>  array ( ), 'source' =>  '' ) ;

  $html = str_get_html ( $text ) ;

  // Find all object
  foreach ( $html->find('object') as $element) {
    foreach ( $html->find('embed') as $sub_element ) {
      if ( ! in_array ( $sub_element->src, $elemets['object'] ) ) {
        $alter = TRUE;
        $elemets['object'][] = $sub_element->src;
      }
    }
  }

  // Find all embed
  foreach ( $html->find('embed') as $element ) {
    if ( ! in_array ( $element->src, $elemets['object'] ) ) {
      $alter = TRUE;
      $elemets['embed'][] = $element->src;
    }
  }

  // Find all div
  if ( empty ( $elemets['embed'] ) && empty ( $elemets['object'] ) ) {
    foreach ( $html->find('div') as $element ) {
      foreach ( $element->find('param[name=movie]') as $sub_element ) {
        $alter = TRUE;
        $elemets['div'][] = $sub_element->value ;
      }
    }
  }

  if ( $alter ) {

    $elemets['source'] = $embed['value'];

    $source[] = $elemets ;

  }

  $plain_html = '';

  foreach ( $source as $node_data ) {

    $srcs = array_unique ( array_merge( $node_data['object'], $node_data['embed'], $node_data['div'] ) );

    foreach ( $srcs as $src ) if ( ! empty ( $src ) ) {

    	allowed_iframe_sources ( $src ) ;

    	$plain_html .= theme ( 'html_tag', array( 'element' => array( '#tag' => 'iframe', '#attributes' => array( 'class' => array( 'altered' ), 'src' => $src ) ) ) ) ;

    }

  }

  return $plain_html ;

}

function allowed_iframe_sources ( $url ) {

  $allowed_domains = array ( ) ;

  foreach ( db_query("SELECT * FROM {mediacommons_filter}") as $record ) $allowed_domains[] = $record->domain ;

  $parse_url = parse_url ( $url ) ;

  if ( ! in_array ( $parse_url['host'], $allowed_domains, true ) ) {

  	drush_log ( dt('Adding @domain to allowed iframe source list', array ( '@domain' => $parse_url['host'] ) ) , 'ok' ) ;

    db_insert('mediacommons_filter')->fields( array('domain' => $parse_url['host'] ) )->execute() ;

    drupal_flush_all_caches ( ) ;

    $cache = cache_get('domains', 'cache_mediacommons_filter') ;

    cache_clear_all ( $cache->cid, 'cache_mediacommons_filter' ) ;

  }

}

function extract_iframe_sources ( $text ) {

	include_once ( libraries_get_path('simplehtmldom') . '/simple_html_dom.php' ) ;

  if ( empty ( $text ) ) return  ;

  $elemets = array ( 'object' => array ( ) , 'embed' => array ( ) , 'div' =>  array ( ), 'iframe' =>  array ( ), 'source' =>  '' ) ;

  $html = str_get_html ( $text ) ;

  try {

    foreach ( $html->find('iframe') as $element) {
      if ( ! in_array ( $element->src, $elemets['iframe'] ) ) {
        $elemets['iframe'][] = $element->src;
      }
    }

    // Find all object
    foreach ( $html->find('object') as $element) {
      foreach ( $html->find('embed') as $sub_element ) {
        if ( ! in_array ( $sub_element->src, $elemets['object'] ) ) {
          $elemets['object'][] = $sub_element->src;
        }
      }
    }

    // Find all embed
    foreach ( $html->find('embed') as $element ) {
      if ( ! in_array ( $element->src, $elemets['object'] ) ) {
        $elemets['embed'][] = $element->src;
      }
    }

    // Find all div
    if ( empty ( $elemets['embed'] ) && empty ( $elemets['object'] ) ) {
      foreach ( $html->find('div') as $element ) {
        foreach ( $element->find('param[name=movie]') as $sub_element ) {
          $elemets['div'][] = $sub_element->value ;
        }
      }
    }

    foreach ( $elemets as $elemet ) foreach ( (array) $elemet as $source ) if ( ! empty ( $source ) ) allowed_iframe_sources ( $source ) ;

  }

  catch ( Exception $e ) {
    drush_log ( dt ( 'Caught exception: @error' , array ( '@error' =>  $e->getMessage() ) ) , 'error') ;
  }

}


function mediacommons_base_import_embed_field ( $new_node ) {

  include_once ( libraries_get_path('simplehtmldom') . '/simple_html_dom.php' );

  $source = array ( ) ;

  $nodes = spokes () ;

  foreach ( $nodes as $node ) {

    $alter = FALSE;

    /** load node */
    $entity = node_load ( $node->nid ) ;

    $elemets = array ( 'nid' => $node->nid , 'object' => array ( ) , 'embed' => array ( ) , 'div' =>  array ( ), 'source' =>  '' ) ;

    /** Wrap it with Entity API */
    $ewrapper = entity_metadata_wrapper('node', $entity);

    $embed = $ewrapper->field_embed->value();

    $html = str_get_html( $embed['value'] ) ;

    // Find all object
    foreach ( $html->find('object') as $element) {
      foreach ( $html->find('embed') as $sub_element ) {
        if ( ! in_array ( $sub_element->src, $elemets['object'] ) ) {
          $alter = TRUE;
          $elemets['object'][] = $sub_element->src;
        }
      }
    }

    // Find all embed
    foreach ( $html->find('embed') as $element ) {
      if ( ! in_array ( $element->src, $elemets['object'] ) ) {
        $alter = TRUE;
        $elemets['embed'][] = $element->src;
      }
    }

    // Find all div
    if ( empty ( $elemets['embed'] ) && empty ( $elemets['object'] ) ) {
      foreach ( $html->find('div') as $element ) {
        foreach ( $element->find('param[name=movie]') as $sub_element ) {
          $alter = TRUE;
          $elemets['div'][] = $sub_element->value ;
        }
      }
    }

    if ( $alter ) {

      $elemets['source'] = $embed['value'];

      $source[] = $elemets ;

    }

  }

  foreach ( $source as $node_data ) {

    $html = '';

    $srcs = array_unique ( array_merge( $node_data['object'], $node_data['embed'], $node_data['div'] ) );

    foreach ( $srcs as $src ) {
      if ( $src ) {
        $html .= theme('html_tag', array( 'element' => array( '#tag' => 'iframe', '#attributes' => array( 'class' => array( 'altered' ), 'src' => $src, 'data-onid' => $node_data['nid'], 'data-onid' => $node_data['nid'] ) ) ) ) ;
      }
    }

    /** load node */
    $entity = node_load ( $node_data['nid'] ) ;

    /** Wrap it with Entity API */
    $ewrapper = entity_metadata_wrapper('node', $entity);

    $ewrapper->field_embed->set( array ( 'value' => $html ) );

    drush_log ( url('node/' . $entity->nid , array( 'absolute'=> TRUE ) ) , 'status' ) ;

    $ewrapper->save();

  }

}

function mediacommons_base_import_body_field () {

  include_once ( libraries_get_path('simplehtmldom') . '/simple_html_dom.php' );

  $source = array ( ) ;

  $nodes = mediacommons_base_import_d7_spoke_list () ;

  foreach ( $nodes as $node ) {

    $alter = FALSE;

    /** load node */
    $entity = node_load ( $node->nid ) ;

    $elemets = array ( 'nid' => $node->nid , 'object' => array ( ) , 'embed' => array ( ) , 'div' =>  array ( ), 'source' =>  '' ) ;

    /** Wrap it with Entity API */
    $ewrapper = entity_metadata_wrapper('node', $entity);

    $embed = $ewrapper->field_body->value();

    continue ;

    $html = str_get_html( $embed['value'] ) ;

    // Find all object
    foreach ( $html->find('object') as $element) {
      foreach ( $html->find('embed') as $sub_element ) {
        if ( ! in_array ( $sub_element->src, $elemets['object'] ) ) {
          $alter = TRUE;
          $elemets['object'][] = $sub_element->src;
        }
      }
    }

    // Find all embed
    foreach ( $html->find('embed') as $element ) {
      if ( ! in_array ( $element->src, $elemets['object'] ) ) {
        $alter = TRUE;
        $elemets['embed'][] = $element->src;
      }
    }

    // Find all div
    if ( empty ( $elemets['embed'] ) && empty ( $elemets['object'] ) ) {
      foreach ( $html->find('div') as $element ) {
        foreach ( $element->find('param[name=movie]') as $sub_element ) {
          $alter = TRUE;
          $elemets['div'][] = $sub_element->value ;
        }
      }
    }

    if ( $alter ) {

      $elemets['source'] = $embed['value'];

      $source[] = $elemets ;

    }

  }

  foreach ( $source as $node_data ) {

    $html = '';

    $srcs = array_unique ( array_merge( $node_data['object'], $node_data['embed'], $node_data['div'] ) );

    foreach ( $srcs as $src ) {
      if ( $src ) {
        $html .= theme('html_tag', array( 'element' => array( '#tag' => 'iframe', '#attributes' => array( 'class' => array( 'altered' ), 'src' => $src, 'data-onid' => $node_data['nid'], 'data-onid' => $node_data['nid'] ) ) ) ) ;
      }
    }

    /** load node */
    $entity = node_load ( $node_data['nid'] ) ;

    /** Wrap it with Entity API */
    $ewrapper = entity_metadata_wrapper('node', $entity);

    $ewrapper->field_body->set( array ( 'value' => $html ) );

    drush_log ( url('node/' . $entity->nid , array( 'absolute'=> TRUE ) ) , 'status' ) ;

    $ewrapper->save();

  }

}

function mc_file_save(stdClass $file) {

  $file->timestamp = REQUEST_TIME;

  $file->filesize = filesize($file->uri);

  module_invoke_all('file_presave', $file);

  module_invoke_all('entity_presave', $file, 'file');

  if (empty($file->fid)) {
    drupal_write_record('file_managed', $file);
    // Inform modules about the newly added file.
    module_invoke_all('file_insert', $file);
    module_invoke_all('entity_insert', $file, 'file');
  }
  else {
    drupal_write_record('file_managed', $file, 'fid');
    // Inform modules that the file has been updated.
    module_invoke_all('file_update', $file);
    module_invoke_all('entity_update', $file, 'file');
  }

  // Clear internal properties.
  unset($file->original);
  // Clear the static loading cache.
  entity_get_controller('file')->resetCache(array($file->fid));

  return $file;
}

function mc_user_file_save_data($object) {
  $destination = file_default_scheme() . '://pictures/' . $object['filename'];
  $replace = FILE_EXISTS_REPLACE;
  $data = file_get_contents($destination);
  if ($data) {
    if ($uri = file_unmanaged_save_data($data, $destination, $replace)) {
      // Create a file object.
      $file = new stdClass();
      $file->fid = $object['fid'];
      $file->fid = NULL;
      $file->uri = $uri;
      $file->filename = $object['filename'];
      $file->filemime = file_get_mimetype($file->uri);
      $file->uid = $object['uid'];
      $file->status = FILE_STATUS_PERMANENT;
      return mc_file_save($file);
    }
  }
  return FALSE;
}

function register_users_images() {
  /** find all */
	$query = db_query("SELECT uid, picture FROM {users} WHERE picture <> 0");
	/** return nids */
	foreach ($query->fetchAll() as $user) {
    $result = db_select('mediacommons_base_import_user_map', 'u')
                ->fields('u')
                ->condition('fid', $user->picture)
                ->condition('uid', $user->uid)
                ->execute()
                ->fetchAssoc();

    $data = mc_user_file_save_data($result);
  }
}
