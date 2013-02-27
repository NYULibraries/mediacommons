<?php

/** Djatoka request */
function mediacommons_image_extended_djatoka_request($file) {
  $file = (object) $file;

  $response = drupal_http_request(
    url(variable_get('mediacommons_image_extended_extended_service', '') . '/resolver', 
    array(
      'query' => 
        array(
          'url_ver' => variable_get('mediacommons_image_extended_openlayers_openurl', 'Z39.88-2004'),
          'svc_id' => variable_get('mediacommons_image_extended_openlayer_metadaurl_service', 'info:lanl-repo/svc/getMetadata'),
          'rft_id' => file_create_url($file->uri),
        ),
        'absolute' => TRUE
      )
    )
  );
  
  if ($response->code == '200' && isset($response->data) ) { 
    return drupal_json_decode($response->data); 
  }
  else {
    watchdog('mediacommons_image_extended', 'Error requesting image metadata from service. @message (error code @code). Please verify that the ' . l('Image extended', 'admin/config/media/mediacommons') . ' module is properly configured. ', array('@message' => $response->error, '@code' => $response->code), WATCHDOG_ERROR);
    return array(
      'message' => isset($response->status_message) ? $response->status_message : $response->error,
      'error' => $response->error,
      'code' => $response->code,
    );
  }
}