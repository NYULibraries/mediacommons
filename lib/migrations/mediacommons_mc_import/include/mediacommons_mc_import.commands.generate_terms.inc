<?php

function generate_terms () {

  // Looks like we are only have to migrate tags
  $g = taxonomy_vocabulary_get_names();
  
  // Drupal 7 vocabulary id
  $vid = $g['tags']->vid;

  unset($g);

  $v = array();

  $d = array('drupal6', 'imr6', 'int6');

  foreach ($d as $p) foreach (_d($p) as $t) (!isset($v[$p])) ? ($v[$p] = array() && $v[$p][] = $t) : $v[$p][] = $t;

  foreach ($v as $p => $c) foreach ($c as $term) {
	if (empty($term->name)) continue;
	if (strpos($term->name, 'file=') !== false) continue;
	if (strpos($term->name, 'file_links') !== false) continue;
	if (strpos($term->name, 'url=') !== false) continue;
	if (strpos($term->name, 'href=') !== false) continue;
	$name = trim($term->name);
	$name = str_replace('"','',$name);
	$name = str_replace("/'{2,}/g",'',$name);
	$name = trim($name);
	if (empty($name)) continue;
	$md5 = md5($name);
	// find out if the md5 for this sting already exists
	if ($exists = _m($md5)) {
	  $fields = array(
		'toid' => $term->tid,
		'tid' => $exists,
		'project' => $p,
		'name' => $name,
		'md5' => $md5,
	  );
	  db_insert('mediacommons_base_import_term_map')->fields($fields)->execute();
	}
	else {
	  // we need a new term
	  $new = new stdClass();
	  $new->vid = $vid;
	  $new->name = $name;
	  $new->description = $term->description;
	  $new->weight = $term->weight;
	  $new->parent = 0;
      if (taxonomy_term_save($new)) {
	    $fields = array(
		  'toid' => $term->tid,
		  'tid' => $new->tid,
		  'project' => $p,
		  'name' => $name,
		  'md5' => $md5,
		);
		db_insert('mediacommons_base_import_term_map')->fields($fields)->execute();
		unset($fields);
		unset($new);
	  }
	}
  }
}

function _d ($db) {
  return Database::getConnection('default', $db)->query('SELECT * FROM {term_data}');
}

function _m ($md5) {
  $exist = db_query('SELECT tid FROM {mediacommons_base_import_term_map} WHERE md5 = :md5', array(':md5' => $md5));
  return ( (int) $exist->rowCount() > 0 ) ? $exist->fetchField() : FALSE;
}
