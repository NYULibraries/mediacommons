<?php

/*
 * Book getters. If you need any of this functionalities load with module_load_include
 * e.g., module_load_include('inc', 'dlts_utilities', 'inc/dlts_utilities.book');
 */

/** Returns book identifier */
function dlts_utilities_book_get_identifier($node) {
  $field = dlts_utilities_field_get_first_item('node', $node, 'field_identifier');
  
  if (isset($field['safe_value']) && !empty($field['safe_value'])) {
    return $field['safe_value'];
  }
  else {
    return FALSE;
  }
}

function dlts_utilities_book_identifier($node) {
  $field = dlts_utilities_field_get_first_item('node', $node, 'field_identifier');
  if (isset($field['safe_value'])) {
    return $field['safe_value'];
  }
  else {
    return '';
  }
}

/**
 * Returns book page sequence number
 */
function dlts_utilities_book_get_sequence_count($node) {
  $field = dlts_utilities_field_get_first_item('node', $node, 'field_sequence_count');
  return $field['value'];
}


/**
 * Returns book volume field
 */
function dlts_utilities_book_get_volume($node) {
	$field = dlts_utilities_field_get_first_item('node', $node, 'field_volume');
	return $field['value'];
}

/**
 * Returns book number field
 */
function dlts_utilities_book_get_number($node) {
	$field = dlts_utilities_field_get_first_item('node', $node, 'field_number');
	return $field['value'];
}

/**
 * Returns book pub date
 */
function dlts_utilities_book_get_publication_date($node) {
	$field = dlts_utilities_field_get_first_item('node', $node, 'field_publication_date');
	return $field['value'];
}