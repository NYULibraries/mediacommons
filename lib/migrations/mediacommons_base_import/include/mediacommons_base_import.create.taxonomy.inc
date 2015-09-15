<?php

function mediacommons_base_import_generate_vocabulary () {

  foreach ( mediacommons_base_import_d6_vocabulary_list () as $record ) {
  	$vid = vid_by_ovid ( $record->vid ) ;
  	/** vocabulary has mapping ? */
  	if ( ! $vid ) {
  	  /** vocabulary does not have mapping; check if exist in Drupal 7 */
  	  $vid = mediacommons_base_import_d7_vid_by_d6_name ( $record->name ) ;
  	  if ( $vid ) {
    	/** vocabulary exist in Drupal 7; but does not have mapping */
  	  	$vocabulary = taxonomy_vocabulary_load ( $vid ) ;
  	  	/** map vocabulary */
  		if ( $vocabulary ) {
  		  drush_log ( dt('Vocabulary @vocabulary mapped.', array ( '@vocabulary' => $vocabulary->name ) ) , 'ok' ) ;
  		  db_insert('mediacommons_base_import_vocabulary_map')->fields( array( 'vuid' => $record->vid, 'vid' => $vid ) )->execute() ;
  	    }
  	  }
    }
  }
}

function mediacommons_base_import_generate_terms() {

  /** Just in case */
  mediacommons_base_import_generate_vocabulary();

  $vocabulary_map = db_query("SELECT * FROM {mediacommons_base_import_vocabulary_map}");

  $terms_d6 = mediacommons_base_import_d6_term_list();

  $vocabularies = array();

  foreach ($vocabulary_map as $vocabulary) {
    $vocabularies[$vocabulary->vuid] = $vocabulary->vid;
  }

  foreach ($terms_d6 as $term) {
    if (isset($vocabularies[$term->vid])) {
      $tid = db_query("SELECT tid FROM {mediacommons_base_import_term_map} WHERE toid = :toid", array('toid' => $term->tid));
      if ( ! $tid->rowCount()) mediacommons_base_import_create_taxonomy_term( $term ) ;
    }
  }
}

/** Create a taxonomy term and return the tid */
function mediacommons_base_import_create_taxonomy_term ( $term ) {
  $new = new stdClass();
  $vocabulary = mediacommons_base_import_find_vocabulary($term->vid);
  $new->vid = $vocabulary->vid;
  $new->name = $term->name;
  $new->description = $term->description;
  $new->weight = $term->weight;
  $new->parent = (int)$term->parent;

  if ( taxonomy_term_save($new) ) {

    db_insert('mediacommons_base_import_term_map')
      ->fields(array(
        'toid' => $term->tid,
        'tid' => $new->tid,
      )
    )->execute();

    drush_log ( dt('Term @term saved!', array ( '@term' => $new->name ) ) , 'ok' ) ;

    return $new;

  }
}

/** Create a taxonomy vocabulary */
function mediacommons_base_import_create_taxonomy_vocabulary ( $vocabulary ) {
  $new = new stdClass();
  $machine_name = strtolower(preg_replace('/\s+/', '', $vocabulary->name));
  $new->name = $vocabulary->name;
  $new->description = $vocabulary->description;
  $new->help = $vocabulary->help;
  $new->relations = $vocabulary->relations;
  $new->machine_name = $machine_name;
  $new->hierarchy = $vocabulary->hierarchy;
  $new->multiple = $vocabulary->multiple;
  $new->required = $vocabulary->required;
  $new->tags = $vocabulary->tags;
  $new->module = $vocabulary->module;
  $new->weight = $vocabulary->weight;
  if ( $v = taxonomy_vocabulary_save($new) ) {
    $m = taxonomy_vocabulary_machine_name_load($new->machine_name);
    db_insert('mediacommons_base_import_vocabulary_map')
      ->fields(array(
        'vuid' => $vocabulary->vid,
        'vid' => $m->vid,
      )
     )->execute();

     drush_log ( dt('Vocabulary @vocabulary saved.', array ( '@vocabulary' => $vocabulary->name ) ) , 'ok' ) ;

     return $v;
  }
}
