<?php

/** Users */

/**
 *  Find out if user is already in the system by its Drupal 6 user id
 */
function mediacommons_base_import_user_exists ( $ouid ) {

	$ouid = (int)$ouid;

	if ( $ouid == 1 ) return 1 ;

	$author_exist = db_query('SELECT uid FROM mediacommons_base_import_user_map u WHERE ouid = :ouid', array(':ouid' => $ouid));

	$count = $author_exist->rowCount();

	if ( $count > 0) {
		$author = $author_exist->fetchObject();
		return $author->uid;
	}

	return FALSE;

}

function mediacommons_base_import_d6_user_one() {

	$users = array();

	/** Create a database connection to the source (d6) database */
	$query = Database::getConnection('default', 'drupal6')

	->select('users', 'u')

	->fields('u', array(
			'uid',
			'name',
			'pass',
			'mail',
			'theme',
			'signature',
			'signature_format',
			'created',
			'access',
			'login',
			'status',
			'language',
			'picture',
			'init',
			'timezone_name'
	)
	)
	->condition('u.uid', 1);

	$query->leftJoin('users_roles', 'ur', 'u.uid = ur.uid');

	$query->groupBy('uid');

	$query->addExpression("GROUP_CONCAT(DISTINCT ur.rid SEPARATOR ',')", 'roles');

	$results = $query->execute();

	foreach ($results as $result) {
		$users[$result->uid] = $result;
	}

	return $users;

}

function mediacommons_base_import_d6_users_list() {

	$users = array();

	/** Create a database connection to the source (d6) database */
	$query = Database::getConnection('default', 'drupal6')

	->select('users', 'u')

	->fields('u', array(
			'uid',
			'name',
			'pass',
			'mail',
			'theme',
			'signature',
			'signature_format',
			'created',
			'access',
			'login',
			'status',
			'language',
			'picture',
			'init',
			'timezone_name'
	)
	)
	->condition('u.uid', 1, '>');

	$query->leftJoin('users_roles', 'ur', 'u.uid = ur.uid');

	$query->groupBy('uid');

	$query->addExpression("GROUP_CONCAT(DISTINCT ur.rid SEPARATOR ',')", 'roles');

	$results = $query->execute();

	foreach ($results as $result) {
		$users[$result->uid] = $result;
	}

	return $users;

}

/** Roles */

/**
 *  Drupal 6 roles
 */
function mediacommons_base_import_d6_roles () {
	return Database::getConnection('default', 'drupal6')->query('SELECT * FROM {role}', array(), array());
}

/**
 *  Drupal 7 roles
 */
function mediacommons_base_import_d7_roles() {
	/** find all */
	$query = db_query("SELECT * FROM {role}");

	/** return nids */
	return $query->fetchAll();

}

/**
 *  Return the roles for a given a Drupal 6 user id
 */
function mediacommons_base_import_find_d6_user_roles ( $uid ) {

	$query = Database::getConnection('default', 'drupal6')
	->select('users', 'u')
	->fields('u', array( 'uid', 'name' ) )
	->condition('u.uid', $uid, '=') ;

	$query->leftJoin('users_roles', 'ur', 'u.uid = ur.uid');
	$query->groupBy('uid');
	$query->addExpression("GROUP_CONCAT(DISTINCT ur.rid SEPARATOR ',')", 'roles');
	return $query->execute();
}

function mediacommons_base_import_d7_vid_by_d6_name ( $name ) {

	$vocabulary = db_query("SELECT vid FROM taxonomy_vocabulary WHERE name = :name", array ( ':name' => $name ) ) ;

	if ( $vocabulary->rowCount() ) return $vocabulary->fetchObject()->vid ;

	else return FALSE ;

}

function mediacommons_base_import_d7_vid_by_d6_vid ( $ovid ) {

	$vocabulary = db_query("SELECT vid FROM mediacommons_base_import_vocabulary_map WHERE vuid = :vuid", array ( ':vuid' => $ovid ) ) ;

	if ( $vocabulary->rowCount() ) return  $vocabulary->fetchObject()->vid ;

	else return FALSE ;

}

function mediacommons_base_import_get_role_by_name ($name) {
	return array_search( $name, user_roles() );
}

/** Drupal 6 Realname */

function mediacommons_base_import_d6_users_realname_list() {

	$users = array();

	/** Create a database connection to the source (d6) database */
	$query = Database::getConnection('default', 'drupal6')->select('realname', 'r')->fields('r', array( 'uid', 'realname' ) )->condition('r.uid', 1, '>');

	$results = $query->execute();

	foreach ( $results as $result ) $users[$result->uid] = $result;

	return $users;

}

function mediacommons_base_import_get_role_rid_by_orid ( $orid ) {

	/** Create a database connection to the source (d6) database */
	$result = db_query("SELECT rid FROM {mediacommons_base_import_role_map} WHERE orid = :orid", array(':orid' => $orid ) );

	$rid = $result->fetchObject() ;

	if ( $rid ) return $rid->rid ;

	else return FALSE ;

}