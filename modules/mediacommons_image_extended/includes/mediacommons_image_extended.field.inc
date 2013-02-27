<?php

/**
 * @file
 * Implements Image Extended widgets and formatters.
 */

// http://api.drupal.org/api/drupal/modules%21image%21image.module/function/image_style_save/7
// http://api.drupal.org/api/drupal/modules%21image%21image.module/function/theme_image_style/7

// make this a dependency
module_load_include('inc', 'mediacommons_image_extended', 'includes/mediacommons_image_extended.djatoka');

function mediacommons_image_extended_field_info() {
  return array(
    'mediacommons_image_extended' => array(
      'label' => t('Image extended'),
      'description' => t('This field stores the ID of an image file as an integer value.'),
      'settings' => array(
        'uri_scheme' => variable_get('file_default_scheme', 'public'),
        'default_image' => 0,
      ),
      'instance_settings' => array(
        'file_extensions' => 'jp2 tif tiff jpg jpeg gif',
        'file_directory' => '',
        'max_filesize' => '',
      ),
      'default_widget' => 'mediacommons_image_extended',
      'default_formatter' => 'mediacommons_image_extended_original',
    ),
  );
}

function mediacommons_image_extended_field_settings_form($field, $instance) {
  return image_field_settings_form($field, $instance);
}

function mediacommons_image_extended_field_is_empty($item, $field) {
  return file_field_is_empty($item, $field);
}

/** Implements hook_field_instance_settings_form(). */
function mediacommons_image_extended_field_instance_settings_form($field, $instance) {
  
  /** Use the file field instance settings form */
  $form = file_field_instance_settings_form($field, $instance);
  
  /** Remove the description option. why? */
  unset($form['description_field']);
  
  /** Remove the file_extensions option */
  unset($form['file_extensions']);
  
  return $form;
  
}

function mediacommons_image_extended_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  /** Add display_field setting to field because file_field_widget_form() assumes it is set. */
  $field['settings']['display_field'] = 0;
  
  $elements = file_field_widget_form($form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
  
  $settings = $instance['settings'];
  
  $supported_extensions = _mediacommons_image_extended_files_supported(); 
  
  foreach (element_children($elements) as $delta) {
    
    // why index 0
    $elements[$delta]['#upload_validators']['file_validate_extensions'][0] = implode(' ', $supported_extensions);
    
    /** Add all extra functionality provided by the image widget. */
    $elements[$delta]['#process'][] = 'mediacommons_image_extended_field_widget_process';
    
  }
  
  if ($field['cardinality'] == 1) {
    /** If there's only one field, return it as delta 0. */
    if (empty($elements[0]['#default_value']['fid'])) {
      $elements[0]['#description'] = theme('file_upload_help', array('description' => $instance['description'], 'upload_validators' => $elements[0]['#upload_validators']));
    }
  }
  
  else {
    $elements['#file_upload_description'] = theme('file_upload_help', array('upload_validators' => $elements[0]['#upload_validators']));
  }

  return $elements;
}

function mediacommons_image_extended_field_load($entity_type, $entities, $field, $instances, $langcode, &$items, $age) {
  file_field_load($entity_type, $entities, $field, $instances, $langcode, $items, $age);
}

function mediacommons_image_extended_field_widget_process($element, &$form_state, $form) {

  $variables = array();
  $item = $element['#value'];
  $item['fid'] = $element['fid']['#value'];
  $instance = field_widget_instance($element, $form_state);
  $settings = $instance['settings'];
  $widget_settings = $instance['widget']['settings'];
  $element['#theme'] = 'mediacommons_image_extended_widget';
  
  /** Add image CSS */
  // $element['#attached']['css'][] = drupal_get_path('module', 'image') . '/image.css';
  
  /** Add the image preview. */
  if ($element['#file']) {
    
    $variables['style_name'] = $widget_settings['preview_image_style']; // mediacommons_image_extended_tojpg
    $variables['path'] = $element['#file']->uri;
    
    /** Determine image dimensions and metadata. If extended_*  is available, the others will also be available */
    if (
      isset($element['#value']['extended_width']) && 
      $element['#value']['extended_width'] != 0 
    ) {
      $variables['extended_width'] = $element['#value']['extended_width'];
      $variables['extended_height'] = $element['#value']['extended_height'];
      $variables['extended_levels'] = $element['#value']['extended_levels'];
      $variables['extended_dwtLevels'] = $element['#value']['extended_dwtLevels'];
      $variables['extended_compositingLayerCount'] = $element['#value']['extended_compositingLayerCount'];
      $variables['width'] = $element['#value']['extended_width'];
      $variables['height'] = $element['#value']['extended_height'];
    }
    
    /** call image server */ 
    else {
      
      $djatoka = mediacommons_image_extended_djatoka_request($element['#file']);
      
      /** image server is available */
      if (!isset($djatoka['error'])) {
        $variables['extended_width'] = $djatoka['width'];
        $variables['extended_height'] = $djatoka['height'];
        $variables['extended_levels'] = $djatoka['levels'];
        $variables['extended_dwtLevels'] = $djatoka['dwtLevels'];
        $variables['extended_compositingLayerCount'] = $djatoka['compositingLayerCount'];
        $variables['width'] = $djatoka['width'];
        $variables['height'] = $djatoka['height'];        
      }
      
      /** image server is not available */
      else {
        
        /** dpm('Else: image server is not available'); */
        /** dpm($element['#value']); */
        /** dpm($element['#value']['width']); */
        /** dpm($element['#value']['height']); */

        form_set_error('mediacommons_image_extended', t('Error requesting image metadata from Djakota service. @message (error code @code).', array('@message' => $djatoka['message'], '@error' => $djatoka['error'])));
        
        $variables['extended_width'] = 
        $variables['extended_height'] =  
        $variables['extended_levels'] = 
        $variables['extended_dwtLevels'] = 
        $variables['extended_compositingLayerCount'] = 0;

        /** try to determine image dimensions. */
        if (
          isset($element['#value']['width']) && 
          $element['#value']['width'] != 0
        ) {
          $variables['width'] = $element['#value']['width'];
          $variables['height'] = $element['#value']['height'];
        }

        /** last oportunity to find width and height */
        else {

          $info = image_get_info($element['#file']->uri);

          if (is_array($info)) {
            $variables['width'] = $info['width'];
            $variables['height'] = $info['height'];
          }

          /** unable to find width and height, set values to null */
          else {
            $variables['width'] = $variables['height'] = 0;
          }

        }        
      }
    }
    
    /** Store the metadata in the form */
    
    $element['preview'] = array(
      '#type' => 'markup',
      '#markup' => theme('image_style', $variables),
    );
    
    $element['width'] = array(
      '#type' => 'hidden',
      '#value' => $variables['width'],
    );

    $element['height'] = array(
      '#type' => 'hidden',
      '#value' => $variables['height'],
    );
    
    $element['extended_width'] = array(
      '#type' => 'hidden',
      '#value' => $variables['extended_width'],
    );
    
    $element['extended_height'] = array(
      '#type' => 'hidden',
      '#value' => $variables['extended_height'],
    );
    
    $element['extended_levels'] = array(
      '#type' => 'hidden',
      '#value' => $variables['extended_levels'],
    );

    $element['extended_dwtLevels'] = array(
      '#type' => 'hidden',
      '#value' => $variables['extended_dwtLevels'],
    );
    
    $element['extended_compositingLayerCount'] = array(
      '#type' => 'hidden',
      '#value' => $variables['extended_compositingLayerCount'],
    );
    
    $element['extended_ri'] = array(
      '#type' => 'checkbox',
      '#title' => t('Representative image'),
      '#attributes' => array('class' => array('edit-image-display-image')),
      '#value' => (isset($element['#value']['extended_ri'])) ? $element['#value']['extended_ri'] : 0,
    );
    
    $options = array_merge(array_keys(image_styles()), array(t('Origial size'), t('Crop'))); 
    
    $element['extended_display'] = array(
      '#title' => t('Display'),
      '#type' => 'radios',
      '#attributes' => array('class' => array('edit-image-display-image')),
      '#value' => (isset($element['#value']['extended_display'])) ? $element['#value']['extended_display'] : 0,
      '#options' => $options,
    );

  }
  
  return $element;

}

/** Implements hook_field_widget_info() */
function mediacommons_image_extended_field_widget_info() {
  return array(
    'mediacommons_image_extended' => array(
      'label' => t('Image extended'),
      'field types' => array('mediacommons_image_extended'),
      'settings' => array(
        'progress_indicator' => 'throbber',
        'preview_image_style' => 'thumbnail',
      ),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_CUSTOM,
        'default value' => FIELD_BEHAVIOR_NONE,
      ),
    ),
  );
}

/** Implements hook_field_widget_settings_form() */
function mediacommons_image_extended_field_widget_settings_form($field, $instance) {
  return image_field_widget_settings_form($field, $instance);
}

/**
 * Implements hook_field_formatter_info().
 */
function mediacommons_image_extended_field_formatter_info() {
  return array(
    'mediacommons_image_extended_hires' => array(
      'label' => t('Image extended Hi-Res'),
      'field types' => array('mediacommons_image_extended'),
      'description' => t('Displays image files in their original size.'),
    ),
    'mediacommons_image_extended_thumbnail' => array(
      'label' => t('Image extended Thumbnail Image'),
      'field types' => array('mediacommons_image_extended'),
      'description' => t('Displays image files in their thumbnail size.'),
    ),
    'mediacommons_image_extended_thumbnail_nodelink' => array(
      'label' => t('Image extended Thumbnail Image linked to node'),
      'field types' => array('mediacommons_image_extended'),
      'description' => t('Displays image files in their thumbnail size.'),
    ),
    'mediacommons_image_extended_original' => array(
      'label' => t('Image extended'),
      'field types' => array('mediacommons_image_extended'),
      'description' => t('Displays image file'),
    ),
    'mediacommons_image_extended_representative_image' => array(
      'label' => t('Representative image'),
      'field types' => array('mediacommons_image_extended'),
      'description' => t('Displays representative image'),
    ),      
  );
}

/**
 * Implments hook_field_formatter_view().
 */
function mediacommons_image_extended_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();
  foreach ($items as $delta => $item) {
    $item['nid'] = $entity->nid;
    $element[$delta] = array(
      '#theme' => $display['type'],
      '#file' => $item,
    );
  }
  return $element;
}

function _mediacommons_image_extended_files_supported() {
  return array(
    'tiff',
    'tif',
    'jp2',
    'jpg',
    'jpeg',
    'gif',
    'png',      
  );
}