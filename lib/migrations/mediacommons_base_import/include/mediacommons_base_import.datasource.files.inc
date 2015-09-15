<?php

function mediacommons_base_import_d6_find_node_image_attach ( $nid ) {

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

function fid_by_ofid ( $ofid ) {
	/** return nids */
	return db_query("SELECT fid FROM {mediacommons_base_import_files_map} WHERE ofid = :ofid", array ( ':ofid' => $ofid ) )->fetchObject() ;
}


function image_fid_by_ofid ( $ofid ) {

  $query = 'SELECT filename, filepath FROM {files} WHERE fid = :fid' ;

  /** Create a database connection to the source (d6) database */
  $result = Database::getConnection('default', 'drupal6')->query($query, array(':fid' => $fid), array());

  return $result->fetchObject();

}

function attached_images_by_onid ( $nid ) {

	$query = Database::getConnection('default', 'drupal6')
	  ->select('image_attach', 'ia')
	  ->fields('f',
			array (
					'fid',
					'filepath',
					'filename',
		)
	)
	->condition('ia.nid', $nid, '=');

	$query->leftJoin('image', 'i', 'ia.iid = i.nid');

	$query->leftJoin('files', 'f', 'f.fid = i.fid');

	return $query->execute();

}

function mediacommons_base_import_find_image ( $fid ) {

  $query = 'SELECT filename, filepath FROM {files} WHERE fid = :fid' ;

  /** Create a database connection to the source (d6) database */
  $result = Database::getConnection('default', 'drupal6')->query($query, array(':fid' => $fid), array());

  return $result->fetchObject();

}

function find_image_by_filename ( $filename ) {

  $query = 'SELECT filename, filepath FROM {files} WHERE filename = :filename';

  /** Create a database connection to the source (d6) database */
  $result = Database::getConnection('default', 'drupal6')->query( $query, array ( ':filename' => $filename ) , array ( ) ) ;

  return $result->fetchObject();

}

function d6_files (  ) {

  /** Create a database connection to the source (d6) database */
  return Database::getConnection('default', 'drupal6')->query('SELECT fid, filename, filepath FROM {files} ORDER BY fid ASC' , array() , array() ) ;

}

function image_attach_files ( ) {

	$query = Database::getConnection('default', 'drupal6')
	->select('image_attach', 'ia')
	->fields('f',
			array (
					'fid',
					'filepath',
					'filename',
			)
	);

	$query->leftJoin('image', 'i', 'ia.iid = i.nid');

	$query->leftJoin('files', 'f', 'f.fid = i.fid');

	return $query->execute();

}

function mediacommons_base_import_get_file_fid_by_ofid ( $ofid ) {

  /** Create a database connection to the source (d6) database */
  $result = db_query("SELECT fid FROM {mediacommons_base_import_files_map} WHERE ofid = :ofid", array(':ofid' => $ofid ) ) ;

  $fid = $result->fetchObject() ;

  if ( $fid ) {
    if (  $fid->fid  > 0 ) return $fid->fid ;
    else {

      $msg =  dt('Error occur while saving image with Drupal 6 fid @fid; content will not include this image.' , array ( '@fid' => $ofid ) )  ;

      drush_log ( $msg , 'error' ) ;

      return FALSE ;
    }
  }

  else {
    drush_log ( dt('Unable to find Drupal fid @fid file.' , array ( '@fid' => $ofid ) ) , 'error' ) ;
    return FALSE ;
  }

}
