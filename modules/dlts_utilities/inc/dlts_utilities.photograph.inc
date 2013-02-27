<?php

/*
 * Photograph getters. If you need any of this functionalities load with module_load_include
 * e.g., module_load_include('inc', 'dlts_utilities', 'inc/dlts_utilities.photograph');
 */
function dlts_utilities_photograph_get_services_image_path($node) {

  $field = dlts_utilities_field_get_first_item('node', $node, 'field_service_copy');
  
  if (isset($field['uri'])) { 
    return array(
      'path' => drupal_realpath($field['uri']),
      'url' => file_create_url($field['uri'])
    );
  }
  else {
    return FALSE;
  }
}