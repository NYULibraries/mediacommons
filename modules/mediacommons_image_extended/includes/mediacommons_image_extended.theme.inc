<?php

/**
 * @file
 * DLTS Image theme functions
 */

module_load_include('inc', 'mediacommons_image_extended', 'includes/mediacommons_image_extended.file');

/**
 * Returns HTML for a high-resolution JPEG 2000 image.
 *
 * This function returns the necessary HTML elements, and sets certain
 * JavaScript settings for the file being displayed. The given elements are
 * then replaced by OpenLayers with a zoomable version of the image.
 *
 * @ingroup themable
 */
function theme_mediacommons_image_extended_hires($variables) {
  
  dpm(__FUNCTION__);
  
  $file = $variables['file'];
  $module_path = drupal_get_path('module', 'mediacommons_image_extended');
  $fid = 'id-'. $file['fid'];
  $fileUri = file_create_url($file['uri']);
  drupal_add_css($module_path . '/css/mediacommons_image_extended.css');
  
  /** Add Openlayers to the page */
  drupal_add_js( variable_get( 'mediacommons_image_extended_openlayers_source', 'sites/all/libraries/openlayers/lib/OpenLayers.js'), array('group' => JS_LIBRARY));

  $openlayers_options = array(
    'zoom' => (isset($file['zoom'])) ? $file['zoom'] : 1,
    'service' => variable_get('mediacommons_image_extended_extended_service', ''),
    'imgMetadata' => array(
      'width' => $file['extended_width'],
      'height' => $file['extended_height'],
      'levels' => $file['extended_levels'],
      'dwtLevels' => $file['extended_dwtLevels'],
      'compositingLayerCount' => $file['extended_compositingLayerCount'],
    ),
  );
  
  $js_inline = '(function(O){O.DLTS.Page("'. $fid .'","'.  $fileUri .'",'. json_encode($openlayers_options) .')})(OpenLayers);';

  $js_options = array(
    'group' => JS_DEFAULT,
    'type' => 'inline',
    'every_page' => FALSE,
    'weight' => 5,
    'scope' => 'footer',
    'cache' => TRUE,
    'defer' => TRUE,
  );
  
  drupal_add_js($js_inline, $js_options);  
  
  return '<div id="id-' . $file['fid'] . '" class="mediacommons_image_extended_map" data-uri="'. $fileUri . '"></div>';
}

function theme_mediacommons_image_extended_thumbnail($variables) {
  dpm(__FUNCTION__);
  
  $item = $variables['file'];
  $classes = array('thumbnail',);
  
  if (isset($item['extended_ri']) && $item['extended_ri']) {
      $classes[] = 'representative';
  }

  $variables = array(
    'path' => mediacommons_image_extended_file_path($item, TRUE),
    'alt' => '',
    'title' => '',
    'attributes' => array('class' => $classes),
  );
  return theme('image', $variables);
}

function theme_mediacommons_image_extended_thumbnail_nodelink($variables) {
  dpm(__FUNCTION__);
  return l(theme('mediacommons_image_extended_thumbnail', $variables), url('node/' . $variables['file']['nid'], array('absolute' => TRUE)), array('html' => TRUE));
}

function theme_mediacommons_image_extended_admin_thumbnail($variables) {
  dpm(__FUNCTION__);
  return theme('mediacommons_image_extended_thumbnail', $variables);
}

function theme_mediacommons_image_extended_original($variables) {
  
  dpm(__FUNCTION__);
  
  $item = $variables['file'];

  $classes = array('original-size',);
  
  if (isset($item['extended_ri']) && $item['extended_ri']) {
    $classes[] = 'representative';
  }

  $variables = array(
    'path' => mediacommons_image_extended_file_path($item, TRUE, 'original'),
    'alt' => '',
    'title' => '',
    'attributes' => array('class' => $classes),
  );
  
  return theme('image', $variables);
}


function theme_mediacommons_image_extended_widget($variables) {
  
  dpm(__FUNCTION__);
  
  $element = $variables['element'];
  $output = '';
  
  $output .= '<div class="image-widget form-managed-file clearfix">';
  if (isset($element['preview'])) {
    $output .= '<div class="image-preview">';
    $output .= drupal_render($element['preview']);
    $output .= '</div>';
  }
  
  $output .= '<div class="image-widget-data">';
  
  if ($element['fid']['#value'] != 0) {
    $element['filename']['#markup'] .= ' <span class="file-size">(' . format_size($element['#file']->filesize) . ')</span> ';
  }
  
  $output .= drupal_render_children($element);
  $output .= '</div>';
  $output .= '</div>';
  
  return $output;
  
}


function theme_mediacommons_image_extended_representative_image($variables) {
  
  dpm(__FUNCTION__);
  
  $item = $variables['file'];

  if ( 
      !isset($item['extended_ri']) || 
      (isset($item['extended_ri']) && !$item['extended_ri'])
  ) {
    return;
  }
  
  $classes = array('extended_ri',);
  
  $variables = array(
    'path' => mediacommons_image_extended_file_path($item, TRUE, 'original'),
    'alt' => '',
    'title' => '',
    'attributes' => array('class' => $classes),
    'style_name' => 'thumbnail',
  );
  
  return theme('image_style', $variables);
  
}