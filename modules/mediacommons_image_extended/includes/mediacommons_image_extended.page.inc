<?php

/**
 * @file
 *   Menu callbacks for the DLTS Image module.
 */

/**
 * Get metadata about an image from a Djatoka server.
 *
 *
 * @param $file
 *   A file fid.
 *
 * @return
 *   An array suitable for converting into JSON
 *
 * @see drupal_json_output
 */

function mediacommons_image_extended_getMetadata($resource) {
  return mediacommons_image_extended_djatoka_request( ( ( $item = file_load( $resource ) ) ?  $item : mediacommons_image_extended_resolve($resource) ) , 'info:lanl-repo/svc/getMetadata');
}

function mediacommons_image_extended_djatoka_ping() {
  return mediacommons_image_extended_djatoka_request( array( 'uri' => drupal_get_path('module', 'mediacommons_image_extended') . '/img/djatoka-ping.jp2' ), 'info:lanl-repo/svc/ping');
}

/** Validate URL. For now a hack */
function mediacommons_image_extended_resolve($fid) {
  // find if we have a valid URL.
  if ( $fid === '0' ) {
   return array(
     'uri' => 'http://memory.loc.gov/gmd/gmd433/g4330/g4330/np000066.jp2',
   );
  }
  return FALSE;
}

function mediacommons_image_extended_djatoka_request($file) {
  $file = (object) $file;

  $request = url(variable_get('mediacommons_image_extended_djatoka_service', '') . '/resolver', array(
      'query' => array(
          'url_ver' => variable_get('mediacommons_image_extended_openlayers_openurl', 'Z39.88-2004'),
          'svc_id' => variable_get('mediacommons_image_extended_openlayer_metadaurl_service', 'info:lanl-repo/svc/getMetadata'),
          'rft_id' => file_create_url($file->uri),
      ),
      'absolute' => TRUE)
  );

  $response = drupal_http_request($request);

  if ($response->code == '200' && isset($response->data) ) {
    return drupal_json_decode($response->data);
  }
  else {
    watchdog('dlts_image', 'Error requesting image metadata (url @url). @message (error code @code). Please verify that the ' . l('DLTS Image', 'admin/config/media/mediacommons') . ' module is properly configured. ', array( '@message' => $response->error, '@code' => $response->code, '@url' => $request ), WATCHDOG_ERROR);
    return array(
        'message' => isset($response->status_message) ? $response->status_message : $response->error,
        'error' => $response->error,
        'code' => $response->code,
    );
  }

}