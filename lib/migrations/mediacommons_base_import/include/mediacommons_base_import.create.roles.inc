<?php

function mediacommons_base_import_map_roles () { map_roles(); }

function map_roles() {
  $roles_d7 = array();
  $roles_d6_list = array();
  /** Drupal 7 roles */
  foreach (d7_roles() as $role) $roles_d7[$role->name] = $role->rid;
  $map_roles = array(
    'admin' => 'administrator',
    'editor' => 'contributor',
    'cluster curator' => 'contributor',
    'managing editor' => 'contributor',
    'student researcher' => 'contributor',
  );
  foreach (mediacommons_base_import_d6_roles() as $role ) {
  	if ( array_key_exists ( $role->name, $roles_d7 ) ) {
    	$record = array( 'orid' => $role->rid, 'rid' => $roles_d7[$role->name] ) ;
    	$rid = db_query("SELECT rid FROM {mediacommons_base_import_role_map} WHERE rid = :rid", array( ':rid' => $roles_d7[$role->name] ) );
	    if ( ! $rid->rowCount() ) db_insert('mediacommons_base_import_role_map')->fields( $record )->execute() ;
	    else db_update('mediacommons_base_import_role_map')->fields( $record )->condition( 'rid', $roles_d7[$role->name])->execute() ;
    }
	  /** we need to map the roles */
	  else {
	    /** check if this roled have a map */
	    if ( isset ( $map_roles[$role->name] ) ) {
		    /** we have a map ; find the Drupal 7 rid */
		    $rid = $roles_d7[ $map_roles[ $role->name ] ] ;
		    /** find if we already have this role mapped */
		    $orid = db_query("SELECT rid FROM {mediacommons_base_import_role_map} WHERE orid = :orid", array( ':orid' => $role->rid ) );
		    /** map relationship */
		    $record = array( 'orid' => $role->rid, 'rid' => $rid ) ;
			  /** here we care for the orid ; since we will be mapping to new rid */
  		  drush_log ( 'Role "' . $role->name . '" was mapped to role "' . $map_roles[$role->name] . '"' , 'status' ) ;
  		  /** insert record */
		    if ( ! $orid->rowCount() ) db_insert('mediacommons_base_import_role_map')->fields( $record )->execute() ;
        else db_update('mediacommons_base_import_role_map')->fields( $record )->condition( 'orid', $role->rid)->execute() ;
	    }
    }
  }
}

/** deprecated */
function mediacommons_base_import_generate_roles() {

  $roles_d7 = $roles_d6_list = array();

  $roles_d6_list = mediacommons_base_import_d6_roles();

  foreach ( mediacommons_base_import_d7_roles() as $role ) {
  	$roles_d7[$role->name] = $role->rid ;
  }

  foreach ( $roles_d6_list as $record ) {

    $rid = db_query("SELECT rid FROM {mediacommons_base_import_role_map} WHERE orid = :orid", array(':orid' => $record->rid));

    if (!$rid->rowCount()) {
       drush_log('Role exist in Drupal 7', "error");
      /** Role exist in Drupal 7 */
      if (array_key_exists($record->name, $roles_d7)) {
        drush_log('Role ' . $record->name . ' already exist', "error");
        db_insert('mediacommons_base_import_role_map')
          ->fields(array(
            'orid' => $record->rid,
            'rid' => $roles_d7[$record->name]
          )
        )->execute();
      }

      /** Role does not exist in Drupal 7 */
      else {
      	if ( $record->name =! 'admin' ) {
          drush_log('Creating role ' . $record->name, "status");
          $role = new stdClass();
          $role->name = $record->name;
          $new = user_role_save($role);
          $r = user_role_load_by_name($role->name);
          db_insert('mediacommons_base_import_role_map')->fields(array( 'orid' => $record->rid, 'rid' => $r->rid, ) )->execute();
        }
        else {
          drush_log('Skip role ' . $record->rid, "status");
        }
      }
    }
    else {
      drush_log('Role "' . $record->name . '" already imported', "status");
    }
  }
}
