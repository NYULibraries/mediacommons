<?php

mediacommons_base_import_d6_configured () ;

/**
 *  Return configured production URL
 */
function mediacommons_base_import_get_production_url() {

  global $settings;

  /**
   * URL to be used as the production URL
   */
  $production_url = $settings['settings']['production']['url'];

  return $production_url;

}

/**
 *  Find out if node is already in the system by its old Drupal 6 nid
 */
function mediacommons_base_import_node_exists($onid) {

  $node_exist = db_query('SELECT nid FROM mediacommons_base_import_node_map m WHERE onid = :onid', array(':onid' => $onid));

  return ((int)$node_exist->rowCount() > 0) ? TRUE : FALSE;

}

/**
 *  Find out Drupal 7 node id by Drupal 6 nid
 */
function mediacommons_base_import_node_nid($onid) {

  $node = db_query('SELECT nid FROM mediacommons_base_import_node_map m WHERE onid = :onid', array(':onid' => $onid))->fetchObject();

  if (isset($node->nid)) {
    return $node->nid;
  }

  return FALSE;

}


function mediacommons_base_import_d6_find_node_image_attach( $nid ) {

  $query = Database::getConnection('default', 'drupal6')
    ->select('image_attach', 'ia')
    ->fields('f',
      array(
        'filepath',
        'filename',
      )
    )
    ->condition('ia.nid', $nid, '=');

  $query->leftJoin('image', 'i', 'ia.iid = i.nid');

  $query->leftJoin('files', 'f', 'f.fid = i.fid');

  return $query->execute();

}

function mediacommons_base_import_d6_find_node_representative_image ( $nid ) {
  $query = Database::getConnection('default', 'drupal6')
    ->select('image_attach', 'ia')
    ->fields('f',
      array(
        'filepath',
        'filename',
      )
    )
    ->condition('ia.nid', $nid, '=');

  $query->leftJoin('image', 'i', 'ia.iid = i.nid');
  $query->leftJoin('files', 'f', 'f.fid = i.fid');
  return $query->execute();

}

function mediacommons_base_import_find_nodes ( $type ) {

  /** find all */
  $query = db_query("SELECT DISTINCT nid FROM {node} WHERE type = :type", array('type' => $type));

  /** return nids */
  return $query->fetchAll();

}

function mediacommons_base_import_find_image($fid) {

  $query = 'SELECT filename, filepath FROM {files} WHERE fid = :fid';

  /** Create a database connection to the source (d6) database */
  $result = Database::getConnection('default', 'drupal6')->query($query, array(':fid' => $fid), array());

  return $result->fetchObject();

}

function mediacommons_base_import_find_url_alias($src) {

  $query = 'SELECT dst FROM {url_alias} WHERE src = :src';

  /** Create a database connection to the source (d6) database */
  $result = Database::getConnection('default', 'drupal6')->query($query, array(':src' => $src), array());

  return $result->fetchObject();

}

function mediacommons_base_import_find_term_id($tid) {
  $term_d7_tid = db_query("SELECT tid FROM mediacommons_base_import_term_map WHERE toid = :toid", array(':toid' => $tid));
  if ($term_d7_tid->rowCount() > 0 && $term = $term_d7_tid->fetchObject()) {
    return $term->tid;
  }
  return FALSE;
}

function mediacommons_base_import_find_term($tid) {

  $term = array();

  $term_d7_tid = db_query("SELECT tid FROM mediacommons_base_import_term_map WHERE toid = :toid", array(':toid' => $tid));

  if (!$term_d7_tid->rowCount()) {

    drush_print_r('old term does not exist; will try to create');

    $query = '
      SELECT term.tid, term.vid, term.name, term.description, term.weight, hierarchy.parent
      FROM {term_data} term
      LEFT JOIN {term_hierarchy} hierarchy
        ON term.tid = hierarchy.tid
      WHERE term.tid = :tid';

    /** Create a database connection to the source (d6) database */
    $result = Database::getConnection('default', 'drupal6')->query($query, array(':tid' => $tid), array());

    $term = mediacommons_base_import_create_taxonomy_term($result->fetchObject());

  }

  else {
    $t = $term_d7_tid->fetchObject();
    $term = taxonomy_term_load($t->tid);
  }

  return $term;

}


function mediacommons_base_import_find_vocabulary( $vid ) {

  $vocabulary = array();

  $vocabulary_d7_vid = db_query("SELECT vid FROM mediacommons_base_import_vocabulary_map WHERE vuid = :vuid", array(':vuid' => $vid));

  if ( ! $vocabulary_d7_vid->rowCount() ) {
    $query = '
        SELECT
          vocabulary.vid,
          vocabulary.name,
          vocabulary.description,
          vocabulary.help,
          vocabulary.relations,
          vocabulary.hierarchy,
          vocabulary.multiple,
          vocabulary.required,
          vocabulary.tags,
          vocabulary.module,
          vocabulary.weight
        FROM {vocabulary} vocabulary
        WHERE vocabulary.vid = :vid';

    /** Create a database connection to the source (d6) database */
    $result = Database::getConnection('default', 'drupal6')->query($query, array(':vid' => $vid), array());

    if ($result->rowCount()) {
      $vocabulary = mediacommons_base_import_create_taxonomy_vocabulary($result->fetchObject());
    }

  }
  else {
    $v = $vocabulary_d7_vid->fetchObject();
    $vocabulary = taxonomy_vocabulary_load($v->vid);
  }

  return $vocabulary;

}

function mediacommons_base_import_d7_vocabulary_list() {
 return taxonomy_get_vocabularies();
}

function mediacommons_base_import_d7_term_list() {
  return taxonomy_get_vocabularies();
}

function mediacommons_base_import_d6_term_list() {

  $query = 'SELECT term.tid, term.vid, term.name, term.description, term.weight FROM {term_data} term ';

  /** Create a database connection to the source (d6) database */
  return Database::getConnection('default', 'drupal6')->query($query, array(), array()) ;

}

function mediacommons_base_import_d6_vocabulary_list() {
  return Database::getConnection('default', 'drupal6')->query('SELECT * FROM {vocabulary}', array(), array());
}

function mediacommons_base_import_d6_node_comments_list() {
  return Database::getConnection('default', 'drupal6')->query('SELECT * FROM {comments}', array(), array());
}



function mediacommons_base_import_d6_hub_list() {

  $query = "
    SELECT
      node.nid,
      node.type,
      node.uid,
      node.vid,
      node.title,
      node.status,
      node.created,
      node.changed,
      node.comment,
      cluster.field_description_value,
      cluster.field_period_value,
      cluster.field_period_value2,
      cluster.field_cluster_type_value,
      cluster.field_tease_image_fid,
      cluster.field_image_fid,
      cluster.field_video_embed_link_value AS video_link,
      cluster.field_video_embed_link_provider AS provider,
      f.filename AS filename,
      GROUP_CONCAT(DISTINCT cfc.field_contributors_uid SEPARATOR ', ') AS contributors,
      GROUP_CONCAT(DISTINCT cfc.delta SEPARATOR ', ') AS contributor_order,
      GROUP_CONCAT(DISTINCT t.tid SEPARATOR ', ') AS terms
    FROM {node} node
    LEFT JOIN {content_type_cluster} cluster
      ON node.vid = cluster.vid
    LEFT JOIN {content_field_contributors} cfc
      ON node.vid = cfc.vid
    LEFT JOIN {files} f
      ON cluster.field_image_fid = f.fid
    LEFT JOIN {term_node} t
      ON node.vid = t.vid
    WHERE node.type = 'cluster' AND node.status = 1
    GROUP BY node.nid
    ORDER BY node.nid";

  /** Create a database connection to the source (d6) database */
  $result = Database::getConnection('default', 'drupal6')->query($query, array(), array());

  return $result;

}

function mediacommons_base_import_d6_spoke_list() {
  $query = "
    SELECT
      n.nid,
      n.vid,
      n.type,
      n.language,
      n.title,
      n.uid,
      n.status,
      n.created,
      n.changed,
      n.comment,
      n.promote,
      n.moderate,
      n.sticky,
      ctcp.field_document_textarea_body_value,
      ctcp.field_tagline_value,
      ctcp.field_pubstat_value,
      GROUP_CONCAT(DISTINCT ctcp.field_thumbnail_fid SEPARATOR ', ') AS thumbnail_fid,
      GROUP_CONCAT(DISTINCT cfaa.field_additional_authors_uid) AS additonal_authors,
      GROUP_CONCAT(DISTINCT cfaa.delta) AS additonal_authors_delta,
      GROUP_CONCAT(DISTINCT cfcl.field_cluster_nid) AS cluster_nid,
      GROUP_CONCAT(DISTINCT cfcl.delta) AS cluster_delta,
      GROUP_CONCAT(DISTINCT t.tid SEPARATOR ', ') AS terms
    FROM tne_node n
      LEFT JOIN {content_type_contributed_pieces} ctcp
        ON n.vid = ctcp.vid
      LEFT JOIN {content_field_additional_authors} cfaa
        ON n.vid = cfaa.vid
      LEFT JOIN {content_field_cluster} cfcl
        ON n.vid = cfcl.vid
      LEFT JOIN {term_node} t
        ON n.vid = t.vid
    WHERE n.type = 'contributed_pieces' AND n.status = 1
    GROUP BY n.nid
    ORDER BY n.nid";

  /** Create a database connection to the source (d6) database */
  $result = Database::getConnection('default', 'drupal6')->query($query, array(), array());

  return $result;

}


function mediacommons_base_import_d7_hubs_list() {
  /** find all */
  $query = db_query("SELECT nid, vid, title FROM {node} WHERE type = :type AND status = :status", array(':status' => 1, ':type' => 'hub'));

  /** return nids */
  return $query->fetchAll();
}

function mediacommons_base_import_d7_spoke_list() {
	/** find all */
	$query = db_query("SELECT nid, vid, title FROM {node} WHERE type = :type AND status = :status", array(':status' => 1, ':type' => 'spoke'));

	/** return nids */
	return $query->fetchAll();
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

function mediacommons_base_import_d6_profile_field_value( $ofid, $ouid ) {

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

	$results = $query->execute();

	foreach ( $results as $result ) {
	  $fields[$result->name] = $result->fid;
	}

	return $fields;

}



function mediacommons_base_import_d6_list_files (  ) {

	/** Create a database connection to the source (d6) database */
    //$query = Database::getConnection('default', 'drupal6')->select('files', 'f')->fields('f', array( 'fid', 'filename', 'filepath' ) );

    $query = 'SELECT fid, filename, filepath FROM {files} ORDER BY fid ASC' ;

	return Database::getConnection('default', 'drupal6')->query($query, array(), array());

}

function mediacommons_base_import_get_file_fid_by_ofid ( $ofid ) {

	/** Create a database connection to the source (d6) database */
	$result = db_query("SELECT fid FROM {mediacommons_base_import_files_map} WHERE ofid = :ofid", array(':ofid' => $ofid ) );

	$fid = $result->fetchObject() ;

	if ( $fid ) return $fid->fid ;

	else return FALSE ;

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
  $nodes = Database::getConnection('default', 'drupal6')->query($query, array(':type' => $type), array());

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
