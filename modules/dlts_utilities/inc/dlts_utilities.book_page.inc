<?php

/*
 * Book page getters. If you need any of this functionalities load with module_load_include
 * e.g., module_load_include('inc', 'dlts_utilities', 'inc/dlts_utilities.book_page');
 */

/**
 * A wildcard loader for dlts_book_page nodes, using the book page sequence number.
 *
 * @parameters:  $identifier and $sequence_number
 * The value of field_sequence_number and field_identifier for a dtls_book node.
 *
 * @return
 * A node object with the given value for field_sequence_number.
 * 
 * TODO: TEST
 */
function dlts_utilities_book_page_sequence_number_load($sequence_number) {
  $query = new EntityFieldQuery;
  $result = $query
    ->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', 'dlts_book_page')
    ->propertyCondition('status', 1)
    ->fieldCondition('field_is_part_of', 'value', arg(1), '=')
    ->fieldCondition('field_sequence_number', 'value', $sequence_number, '=')
    ->execute();

  if (!empty($result['node'])) {
     return node_load(array_keys($result['node']));
  } else {
    return filter_xss($sequence_number);
  }
}

/**
 * Returns book from a book page
 */
function dlts_utilities_book_page_get_book($node) {
  $field = dlts_utilities_field_get_first_item('node', $node, 'field_book');
  return $field;
}

/**
 * Returns book nid
 */
function dlts_utilities_book_page_get_book_nid($node) {
  $field = dlts_utilities_field_get_first_item('node', $node, 'field_book');
  return $field['nid'];
}

/** Returns book page sequence number */
function dlts_utilities_book_page_get_sequence_number($node, $dir = 'ltr') { // use site language? how can I get direction

  if ($node->type == 'dlts_book_stitched_page') {
    $item1 = dlts_utilities_field_get_first_item('node', $node, 'field_sequence_number_left');
    // $item2 = dlts_utilities_field_get_first_item('node', $node, 'field_sequence_number_right');
    $field = $item1['value'];
  }
  
  else {
    $item1 = dlts_utilities_field_get_first_item('node', $node, 'field_sequence_number');
    $field = $item1['value'];
  }
  return $field;
}

/**
 * Returns book page real number
 */
function dlts_utilities_book_page_get_real_number($node) {
  $field = dlts_utilities_field_get_first_item('node', $node, 'field_real_page_number');
  return $field['value'];
}

function dlts_utilities_book_page_get_services_image($node) {
  $field = dlts_utilities_field_get_first_item('node', $node, 'field_service_copy');
  return drupal_realpath($field['uri']);
}

/**
 * Returns book page OCR
 */
function dlts_utilities_book_page_get_ocr($node) {
  $field = dlts_utilities_field_get_first_item('node', $node, 'field_ocr_text');
  if ( isset($field['value']) ) {
    return strip_tags($field['value']);
  }
  else {
    return '';
  }
}

/**
 * Returns (object) book from a book page
 */
function dlts_utilities_book_page_load_book($node) {  
  if (isset($node->field_book)) {
    $book = node_load(dlts_utilities_book_page_get_book_nid($node));
  }
  return ( isset($book) ? $book : FALSE );
}

/**
 * Returns book "identifier". The return value can be isPartOf if available or book->field_identifier
 */
function dlts_utilities_book_page_get_identifier($node) {

  $field = dlts_utilities_field_get_first_item( 'node', $node, 'field_is_part_of' );
  
  /*
   * Check if is_part_of is available
   */  
  if (empty($field)) {
    /*
     * isPartOf not available, load book and fallback to field_identifier
     */
    $book = dlts_utilities_book_page_load_book($node);  
    if ($book) {
      $field = dlts_utilities_field_get_first_item( 'node', $book, 'field_identifier' );
    }
  }

  if ( isset($field) && isset($field['safe_value'])) {
    return $field['safe_value'];
  }  
}

function dlts_utilities_book_page_get_hand_side($node, $dir = 'ltr') {
  $field = dlts_utilities_field_get_first_item('node', $node, 'field_hand_side');
  return $field['value'];
}