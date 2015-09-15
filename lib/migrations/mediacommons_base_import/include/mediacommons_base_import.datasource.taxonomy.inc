<?php

function mediacommons_base_import_find_term_id ( $tid ) {
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
