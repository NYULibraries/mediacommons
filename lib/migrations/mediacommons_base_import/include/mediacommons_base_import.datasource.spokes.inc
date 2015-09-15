<?php

function mediacommons_base_import_d6_spoke_list ( ) {
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

function mediacommons_base_import_d7_hubs_list ( ) {
	/** find all */
	$query = db_query("SELECT nid, vid, title FROM {node} WHERE type = :type AND status = :status", array(':status' => 1, ':type' => 'hub'));

	/** return nids */
	return $query->fetchAll();
}

function spokes () {

	/** find all */
	$query = db_query("SELECT nid, vid, title FROM {node} WHERE type = :type", array(':type' => 'spoke' ) ) ;

	/** return nids */
	return $query->fetchAll() ;

}
