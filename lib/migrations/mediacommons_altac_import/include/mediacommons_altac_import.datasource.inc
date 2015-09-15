<?php

function response () {

  $query = "
    SELECT
      c.cid,
      c.pid,
      c.nid,
      c.nid,
      c.hostname,
      c.thread,
      c.name,
      c.uid,
      c.mail,
      c.homepage,
      nr.title,
      nr.body,
      nr.teaser,
      nr.timestamp
    FROM {node_comments} c
      LEFT JOIN {node_revisions} nr
        ON nr.nid = c.cid
      LEFT JOIN {node} n
        ON n.nid = c.cid
    WHERE n.status = 1
  ";

  $results = Database::getConnection('default', 'drupal6')->query($query, array(), array());

  /** Create and return a database connection to the source (d6) database */
  return Database::getConnection('default', 'drupal6')->query($query, array(':onid' => $onid), array());
}

function clusters () {
  $query = "
    SELECT
      n.nid,
      MAX(n.vid) AS vid,
      n.type,
      n.language,
      n.uid,
      n.status,
      n.created,
      n.changed,
      n.comment,
      n.promote,
      n.moderate,
      n.sticky,
      nr.title,
      ct.*,
      f.filename AS filename,
      GROUP_CONCAT(DISTINCT cfc.field_contributors_uid ORDER by cfc.delta SEPARATOR ', ') AS contributors,
      GROUP_CONCAT(DISTINCT cfp.field_contributed_piece_nid ORDER by cfp.delta SEPARATOR ', ') AS pieces,
      GROUP_CONCAT(DISTINCT t.tid SEPARATOR ', ') AS terms
    FROM {node} n
      LEFT JOIN {node_revisions} nr
        ON n.vid = nr.vid
      LEFT JOIN {content_type_cluster} ct
        ON n.vid = ct.vid
    LEFT JOIN {content_field_contributors} cfc
      ON n.vid = cfc.vid
    LEFT JOIN {content_field_contributed_piece} cfp
      ON n.vid = cfp.vid
    LEFT JOIN {files} f
      ON ct.field_image_fid = f.fid
    LEFT JOIN {term_node} t
      ON n.vid = t.vid
    WHERE n.type = :type
    GROUP BY n.nid
    ORDER BY n.nid";

  /** Create and return a database connection to the source (d6) database */
  return Database::getConnection('default', 'drupal6')->query($query, array(':type' => 'cluster'), array()) ;

}

function contributed_pieces ( $nid = NULL ) {

	$options = array () ;

  $query = "
    SELECT
      n.nid,
      MAX(n.vid) AS vid,
      n.type,
      n.language,
      nr.title,
      nr.teaser,
      nr.format,
      n.uid,
      n.status,
      n.created,
      n.changed,
      n.comment,
      n.promote,
      n.moderate,
      n.sticky,
      ct.field_document_textarea_body_value,
      ct.field_tagline_value,
      ct.field_pubstat_value,
      GROUP_CONCAT(DISTINCT ct.field_thumbnail_fid SEPARATOR ', ') AS thumbnail_fid,
      GROUP_CONCAT(DISTINCT cfaa.field_additional_authors_uid) AS additonal_authors,
      GROUP_CONCAT(DISTINCT cfaa.delta) AS additonal_authors_delta,
      GROUP_CONCAT(DISTINCT cfcl.field_cluster_nid) AS cluster_nid,
      GROUP_CONCAT(DISTINCT cfcl.delta) AS cluster_delta,
      GROUP_CONCAT(DISTINCT t.tid SEPARATOR ', ') AS terms
    FROM {node} n
      LEFT JOIN {node_revisions} nr
        ON n.vid = nr.vid
      LEFT JOIN {content_type_contributed_pieces} ct
        ON n.vid = ct.vid
      LEFT JOIN {content_field_additional_authors} cfaa
        ON n.vid = cfaa.vid
      LEFT JOIN {content_field_cluster} cfcl
        ON n.vid = cfcl.vid
      LEFT JOIN {term_node} t
        ON n.vid = t.vid
    WHERE n.type = :type" ;

  if ( $nid ) {
    $query .= " AND n.nid = :nid" ;
  	$options[':nid'] = $nid ;
  }

  $query .= " GROUP BY n.nid ORDER BY n.nid" ;

  $options[':type'] = 'contributed_pieces' ;

  /** Create and return a database connection to the source (d6) database */
  return Database::getConnection('default', 'drupal6')->query(  $query, $options, array() ) ;

}

function find_iframe_sources ( $body ) {

	include_once ( libraries_get_path('simplehtmldom') . '/simple_html_dom.php' );

	$hosts = array ( 'iframe' => array ( ) , 'embed' => array ( ) , 'img' =>  array ( ) , 'sources' ) ;

  $html = str_get_html( $body ) ;

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
