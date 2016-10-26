<?php

function frontpages () {
  $type = 'front_page_post';
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
  		f.fid AS fid
    FROM {node} n
      LEFT JOIN {node_revisions} nr
        ON n.vid = nr.vid
      LEFT JOIN {content_type_front_page_post} ct
        ON n.vid = ct.vid
    LEFT JOIN {files} f
      ON ct.field_fpp_image_fid = f.fid
    LEFT JOIN {term_node} t
      ON n.vid = t.vid
    WHERE n.type = :type
    AND n.status > 0
    GROUP BY n.nid
    ORDER BY n.nid";
  /** Create and return a database connection to the source (d6) database */
  return Database::getConnection('default', 'drupal6')->query($query, array(':type' => $type), array());
}

function imr_term_list() {
	$query = 'SELECT term.tid, term.vid, term.name, term.description, term.weight FROM {term_data} term';
	/** Create a database connection to the source (d6) database */
	return Database::getConnection('default', 'imr6')->query($query, array(), array()) ;
}

function purge_terms() {

}

//function imr_term_list() {
//	$query = 'SELECT term.tid, term.vid, term.name, term.description, term.weight FROM {term_data} term';
//	/** Create a database connection to the source (d6) database */
//	return Database::getConnection('default', 'imr6')->query($query, array(), array()) ;
//}

//SELECT DISTINCT tid FROM mediacommons_d6_content.mc_term_node;
//SELECT DISTINCT tid FROM tne_d6_content.tne_term_node;
//SELECT DISTINCT tid FROM altac_d6_content.altac_term_node;
//SELECT DISTINCT tid FROM imr_d6_content.imr_term_node;
//SELECT DISTINCT tid FROM intransition_d6_content.intransition_term_node;
