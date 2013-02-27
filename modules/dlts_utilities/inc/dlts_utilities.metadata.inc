<?php

/*
 * Site-Specific Metadata Getters
 */

/**
 * Returns site URL
 * TODO: Do we want a form for $base_url
 */
function dlts_utilities_base_url() {
  global $base_url;
  return $base_url;
}

/**
 * Returns collection id
 */
function dlts_utilities_collection() {
  return _dlts_utilities_metadata_get('collection_id');
}

/**
 * Return repository name
 */
function dlts_utilities_partner_name() {
  return _dlts_utilities_metadata_get('partner_name');
}

/**
 * Return repository code
 */
function dlts_utilities_partner_code() {
  return _dlts_utilities_metadata_get('partner_code');
}

/**
 * Return collection abstract
 */
function dlts_utilities_collection_abstract() {
  return _dlts_utilities_metadata_get('collection_abstract');
}

/**
 * Return collection creator 
 */
function dlts_utilities_collection_creator() {
  return _dlts_utilities_metadata_get('collection_creator');
}

/**
 * Return collection code
 */
function dlts_utilities_collection_code() {
  return _dlts_utilities_metadata_get('collection_code');
}

/**
 * Return collection title
 */
function dlts_utilities_collection_title() {
  return _dlts_utilities_metadata_get('collection_title');
}

/**
 * Return collection call number
 */
function dlts_utilities_collection_call_number() {
  return _dlts_utilities_metadata_get('collection_call_number');
}

/**
 * Return collection type
 */
function dlts_utilities_collection_type() {
  return _dlts_utilities_metadata_get('collection_type');
}

/**
* Return collection finding aid
*/
function dlts_utilities_collection_bobcat_record() {
  return _dlts_utilities_metadata_get('collection_bobcat_record');
}

/**
 * Return collection finding aid
 */
function dlts_utilities_collection_finding_aid_link() {
  return _dlts_utilities_metadata_get('collection_finding_aid_link');
}

/**
 * Site-specific metadata key/values
 * 
 * @return
 * A keyed array site-specific metadata values
 */
function dlts_utilities_site_metadata() {
  global $conf;
  $metadata = array();
  if (isset($conf)) {
    foreach( $conf as $key => $value) {
      if ( preg_match('/dlts_utilities_metadata_(.*)/', $key, $matches )  ) {
        $metadata[$matches[1]] = $value;
      }      
    }
  }
  return $metadata;
}

/**
 * Returns site-specific metadata value
 * TODO: Get metadata from site variables, if not-set, ask R* .
 */
function _dlts_utilities_metadata_get($name) {
  $value = variable_get('dlts_utilities_metadata_' . $name, FALSE);
  if (!$value) {
    // Ask r* logic
    return FALSE;
  }
  return $value;
}
