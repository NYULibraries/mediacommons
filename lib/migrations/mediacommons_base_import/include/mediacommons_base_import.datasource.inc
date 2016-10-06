<?php

/** Return configured production URL */
function umbrella_production_url() {

	global $settings ;

	$info_file = $settings['script_path']['dirname'] . '/' . $settings['script_path']['filename'] . '.info' ;

	$info = drupal_parse_info_file ( $info_file ) ;

	/** URL to be used as the production URL */
	$production_url = $info['mediacommons']['environments']['production']['mediacommons'] ;

	return $production_url;

}

function mediacommons_base_import_get_umbrella_production_url() {
	return umbrella_production_url();
}

/** Return configured production URL */
function mediacommons_base_import_get_production_url() {

  global $settings ;

  $info_file = $settings['script_path']['dirname'] . '/' . $settings['script_path']['filename'] . '.info' ;

  $info = drupal_parse_info_file ( $info_file ) ;

  /** URL to be used as the production URL */
  $production_url = $info['mediacommons']['environments']['production'][$info['mediacommons']['project']] ;

  return $production_url;

}

/** Return configured production URL */
function environment_url ( ) {

  global $settings ;

  $info_file = $settings['script_path']['dirname'] . '/' . $settings['script_path']['filename'] . '.info' ;

  $info = drupal_parse_info_file ( $info_file ) ;

  /** URL to be used as the production URL */
  $environment_url = $info['mediacommons']['environments'][$info['mediacommons']['environment']][$info['mediacommons']['project']] ;

  return $environment_url;

}

/** Drupal 6 profile fields value by Drupal 6 user id */

function mediacommons_base_import_d6_profile_fields_values_list( $oid ) {

  $users = array();

  /** Create a database connection to the source (d6) database */
  $query = Database::getConnection('default', 'drupal6')->select('profile_values', 'v')->fields('v', array( 'fid', 'uid', 'value' ) )->groupBy('v.uid')->condition('v.uid', $oid );

  $results = $query->execute();

  foreach ( $results as $result ) $users[$result->uid] = $result;

  return $users;

}

function mediacommons_base_import_d6_profile_field_value ( $ofid, $ouid ) {

  $value = FALSE ;

  /** Create a database connection to the source (d6) database */
  $query = Database::getConnection('default', 'drupal6')->select('profile_values', 'v')->fields('v', array( 'value' ) )->condition('v.fid', $ofid )->condition('v.uid', $ouid );

  $results = $query->execute() ;

  $field = $results->fetchObject() ;

  if ( isset( $field->value ) && ! empty ( $field->value ) ) $value = $field->value ;

  return $value;

}

/** Drupal 6 profile fields */

function mediacommons_base_import_d6_profile_fields_list() {

  $fields = array();

  /** Create a database connection to the source (d6) database */
  $query = Database::getConnection('default', 'drupal6')->select('profile_fields', 'v')->fields('v', array( 'fid', 'name' ) );

  foreach ( $query->execute() as $result ) $fields[$result->name] = $result->fid ;

  return $fields ;

}

function mediacommons_base_import_find_iframe_sources () {

	include_once ( libraries_get_path('simplehtmldom') . '/simple_html_dom.php' );

	$type = 'post';

	$query = "
    SELECT
      n.nid,
      MAX(n.vid) AS vid,
  	  nr.body,
      ct.*
    FROM {node} n
    LEFT JOIN {node_revisions} nr ON n.vid = nr.vid
    LEFT JOIN {content_type_post} ct ON n.vid = ct.vid
    WHERE n.type = :type
    AND n.status > 0
    GROUP BY n.nid
    ORDER BY n.nid";

	/** Create and return a database connection to the source (d6) database */
	$nodes = Database::getConnection('default', 'drupal6')->query($query, array(':type' => $type), array());

	$hosts = array ( 'iframe' => array ( ) , 'embed' => array ( ) , 'img' =>  array ( ) , 'sources' ) ;

	foreach ( $nodes as $node ) {

		$html = str_get_html( $node->field_video_embed_link_value . ' ' . $node->body ) ;
		// Find all iframe
		foreach ( $html->find('iframe') as $element) {
			$parse_url = parse_url( $element->src ) ;
			if ( ! in_array( $parse_url['host'], $hosts['iframe'], true ) ) {
				$hosts['iframe'][] = $parse_url['host'];
			}
		}
		// Find all embed
		foreach ( $html->find('embed') as $element ) {
			$parse_url = parse_url( $element->src ) ;
			if ( ! in_array( $parse_url['host'], $hosts['embed'], true ) ) {
				$hosts['embed'][] = $parse_url['host'];
			}
		}
		// Find all img
		foreach ( $html->find('img') as $element ) $hosts['img'][] = $element->src ;
	}

	$hosts['sources'] = array_merge( $hosts['iframe'] , $hosts['embed'] ) ;

	return $hosts['sources'] ;

}

function mediacommons_base_import_contributed_pieces_find_iframe_sources ( ) {

  include_once ( libraries_get_path('simplehtmldom') . '/simple_html_dom.php' );

  $query = "
    SELECT
      n.nid,
      MAX(n.vid) AS vid,
  	  nr.body,
      ct.*
    FROM {node} n
    LEFT JOIN {node_revisions} nr ON n.vid = nr.vid
    LEFT JOIN {content_type_contributed_pieces} ct ON n.vid = ct.vid
    WHERE n.type = 'contributed_pieces'
    AND n.status
    GROUP BY n.nid
    ORDER BY n.nid";

  /** Create and return a database connection to the source (d6) database */
  $nodes = Database::getConnection('default', 'drupal6')->query( $query, array( ':type' => $type ), array ( ) ) ;

  $hosts = array ( 'iframe' => array ( ) , 'embed' => array ( ) , 'img' =>  array ( ) , 'sources' ) ;

  foreach ( $nodes as $node ) {

    $html = str_get_html( $node->field_document_textarea_body_value . ' ' . $node->body ) ;
    // Find all iframe
    foreach ( $html->find('iframe') as $element) {
      $parse_url = parse_url( $element->src ) ;
      if ( ! in_array( $parse_url['host'], $hosts['iframe'], true ) ) {
        $hosts['iframe'][] = $parse_url['host'];
      }
    }
    // Find all embed
    foreach ( $html->find('embed') as $element ) {
      $parse_url = parse_url( $element->src ) ;
      if ( ! in_array( $parse_url['host'], $hosts['embed'], true ) ) $hosts['embed'][] = $parse_url['host'] ;
    }
    // Find all img
    foreach ( $html->find('img') as $element ) {
      $hosts['img'][] = $element->src ;
    }
  }

  $hosts['sources'] = array_merge( $hosts['iframe'] , $hosts['embed'] ) ;

  return $hosts['sources'] ;

}
