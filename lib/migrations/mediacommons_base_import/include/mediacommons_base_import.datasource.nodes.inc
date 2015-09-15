<?php

/**
 *  Find out if node is already in the system by its old Drupal 6 nid
 */
function node_exists ( $onid ) {

	$node_exist = db_query('SELECT nid FROM {mediacommons_base_import_node_map} WHERE onid = :onid', array ( ':onid' => $onid ) ) ;

	return ( (int) $node_exist->rowCount() > 0 ) ? TRUE : FALSE ;

}

/**
 *  Find out Drupal 7 node id by Drupal 6 nid
 */
function nid_by_onid ( $onid ) {

	$node = db_query('SELECT nid FROM mediacommons_base_import_node_map m WHERE onid = :onid', array(':onid' => $onid ) )->fetchObject() ;

	if ( isset ( $node->nid ) ) return $node->nid ;

	return FALSE ;

}

function mediacommons_base_import_find_nodes ( $type ) {

	/** find all */
	$query = db_query("SELECT DISTINCT nid FROM {node} WHERE type = :type", array('type' => $type));

	/** return nids */
	return $query->fetchAll();

}

function mediacommons_base_import_find_url_alias($src) {
	$query = 'SELECT dst FROM {url_alias} WHERE src = :src';
	/** Create a database connection to the source (d6) database */
	$result = Database::getConnection('default', 'drupal6')->query($query, array(':src' => $src), array());
	return $result->fetchObject();
}


function url_alias_by_onid ( $onid ) {

	$src = 'node/' . $onid ;

	$query = 'SELECT dst FROM {url_alias} WHERE src = :src';

	/** Create a database connection to the source (d6) database */
	$result = Database::getConnection('default', 'drupal6')->query($query, array(':src' => $src), array());

	return $result->fetchObject();

}