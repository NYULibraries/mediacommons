<?php

/**
 * @file
 * Admin page.
 */

module_load_include('inc', 'mediacommons_image_extended', 'includes/mediacommons_image_extended.djatoka');

function mediacommons_image_extended_admin($form, &$form_state) {

  $form = $js = array();
  
  $extended = mediacommons_image_extended_djatoka_request(array('uri' => 'http://memory.loc.gov/gmd/gmd433/g4330/g4330/np000066.jp2'));
  
  if (!isset($extended['error'])) {
    $preview = theme( 'mediacommons_image_extended_hires', array( 'file' =>
      array(
        'fid' => 'mediacommons_image_extended',
        'uri' => $extended['identifier'],
        'extended_width' => $extended['width'],
        'extended_height' => $extended['height'],
        'extended_dwtLevels' => $extended['dwtLevels'],
        'extended_levels' => $extended['levels'],
        'extended_compositingLayerCount' => $extended['compositingLayerCount'],
        'zoom' => 5,
        ),
      )
    );
  }
  else {
    $preview = '<div class="messages error"><h2 class="element-invisible">error</h2>'. t('extended: Unable to request metadata from Djakota Jpeg 2000 Image Server. See: @message (code: @code)', array( '@message' => $extended['message'], '@code' => $extended['code'] )) . '</div>';
  }

  $form['extended'] = array(
    '#type' => 'fieldset',
    '#title' => t('Image Server'),
    '#collapsed' => FALSE,
    '#collapsible' => TRUE,
  );

  $form['extended']['mediacommons_image_extended_extended_service_host'] = array(
    '#type' => 'textfield',
    '#title' => t('Service path'),
    '#default_value' => variable_get( 'mediacommons_image_extended_extended_service_host', ( isset( $_SERVER['HTTP_HOST'] ) ? $_SERVER['HTTP_HOST'] : '' ) ),
    '#size' => 60,
    '#maxlength' => 256,
    '#description' => t('Set host name. <acronym title="for example">e.g.</acronym>, http://myhost.bobst.nyu.edu:8080/adore-djatoka'),
    '#required' => TRUE,
  );

  $form['extended']['mediacommons_image_extended_image_service_preview'] = array(
    '#type' => 'item',
    '#title' => t('Preview'),
    '#markup' => $preview,
    '#weight' => 10,
  );
  
  $form['extended']['mediacommons_image_extended_extended_service'] = array(
    '#type' => 'hidden',
    '#value' => variable_get('mediacommons_image_extended_extended_service', ''),
  );
  
  $form['extended']['mediacommons_image_extended_extended_metadataurl'] = array(
    '#type' => 'hidden',
    '#value' => variable_get('mediacommons_image_extended_extended_metadataurl', ''),
  );  

  $form['openurl'] = array(
    '#type' => 'fieldset',
    '#title' => t('OpenURL'),
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
  );

  $form['openurl']['mediacommons_image_extended_openlayers'] = array('#type' => 'hidden', '#value' => variable_get( 'mediacommons_image_extended_openlayers', '' ) );

  $form['openurl']['mediacommons_image_extended_openlayer_metadaurl_service'] = array(
    '#type' => 'textfield',
    '#title' => t('Service to request image metadata'),
    '#default_value' => variable_get('mediacommons_image_extended_openlayer_metadaurl_service', 'info:lanl-repo/svc/getMetadata'),
    '#size' => 60,
    '#maxlength' => 256,
    '#description' => t("The service to request image metadata."),
    '#required' => TRUE,
  );

  $form['openurl']['mediacommons_image_extended_openlayers_openurl'] = array(
    '#type' => 'textfield',
    '#title' => t('OpenURL Resolvers'),
    '#default_value' => variable_get('mediacommons_image_extended_openlayers_openurl', 'Z39.88-2004'),
    '#size' => 60,
    '#maxlength' => 256,
    '#description' => t('Identifier Standard.'),
    '#required' => TRUE,
  );

  $form['openurl']['mediacommons_image_extended_openlayers_format'] = array(
    '#type' => 'textfield',
    '#title' => t('Format'),
    '#default_value' => variable_get( 'mediacommons_image_extended_openlayers_format', 'image/jpeg' ),
    '#size' => 60,
    '#maxlength' => 256,
    '#description' => t("Format."),
    '#required' => TRUE,
  );
  
  $form['openlayers'] = array(
    '#type' => 'fieldset',
    '#title' => t('OpenLayers'),
    '#collapsed' => FALSE,
    '#collapsible' => TRUE,
  );

  $available_files_lib = file_scan_directory( libraries_get_path('openlayers') . '/lib', '/.*\.js$/', array( 'recurse' => FALSE ) );
  
  $available_files_build = file_scan_directory( libraries_get_path('openlayers') . '/build', '/.*\.js$/', array( 'recurse' => FALSE ) );
  
  $available_files = array_merge((array) $available_files_lib, (array) $available_files_build);
  
  foreach ($available_files as $key => $value) { $js[$key] = $value->uri; }
    
  $form['openlayers']['mediacommons_image_extended_openlayers_source'] = array(
    '#type' => 'radios',
    '#title' => t('File'),
    '#default_value' => variable_get( 'mediacommons_image_extended_openlayers_source', 'sites/all/libraries/openlayers/lib/OpenLayers.js'),
    '#options' => $js,
  );  

  /** Make changes to the settings before passing them off to system_settings_form_submit(). */
  $form['#submit'][] = 'mediacommons_image_extended_image_service_admin_settings_submit';

  return system_settings_form($form);
  
}

/** Form submit handler for mediacommons_image admin settings form. */
function mediacommons_image_extended_image_service_admin_settings_submit( $form, &$form_state) {
  /** Ensure no trailing slash */
  $service = rtrim( $form_state['values']['mediacommons_image_extended_extended_service_host'], '/' );
  $form_state['values']['mediacommons_image_extended_extended_service'] = $service;
  $form_state['values']['mediacommons_image_extended_extended_metadataurl'] = $service . '?';  

}