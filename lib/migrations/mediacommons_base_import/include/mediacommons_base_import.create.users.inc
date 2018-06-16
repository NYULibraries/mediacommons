<?php

function _update_profile_fields() {
  drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_FULL);
  try {
  	$users = d6_users();
  	foreach ($users as $d6_user) {
  		$account = user_by_ouid($d6_user->uid);
  		if ($account) {
  		  mediacommons_base_import_migrate_profile_fields($account, $d6_user);
  		}
  	}
  }
  catch (Exception $e) {
  	drush_log('Caught exception: ',  $e->getMessage(), 'error');
  }
}

function users_file_save($file, $object) {
  $user = user_load($object['uid']);
  $file->timestamp = REQUEST_TIME;
  $file->filesize = filesize($file->uri);
  module_invoke_all('file_presave', $file);
  module_invoke_all('entity_presave', $file, 'file');
  if (empty($file->fid)) {
    drupal_write_record('file_managed', $file);
    // Inform modules about the newly added file.
    module_invoke_all('file_insert', $file);
    module_invoke_all('entity_insert', $file, 'file');
    $query = 'SELECT fid FROM {files} WHERE filename = :filename';
    $fid = db_query('SELECT fid FROM {file_managed} WHERE uri = :uri', array(':uri' => $file->uri))->fetchField();
  }
  else {
    drupal_write_record('file_managed', $file, 'fid');
    // Inform modules that the file has been updated.
    module_invoke_all('file_update', $file);
    module_invoke_all('entity_update', $file, 'file'); // true
    $fid = $file->fid;
  }
  $user->picture = $fid;
  user_save($user);
  // Clear internal properties.
  unset($file->original);
  // Clear the static loading cache.
  entity_get_controller('file')->resetCache(array($file->fid));
  return array($file, $user);
}

function mc_user_file_save_data($object) {
  $destination = file_default_scheme() . '://pictures/' . $object['filename'];
  $data = file_get_contents($destination);
  if ($data) {
    $uri = file_unmanaged_save_data($data, $destination, FILE_EXISTS_REPLACE);
    if ($uri) {
      $file = new stdClass();
      $file->fid = $object['fid'];
      $file->fid = NULL;
      $file->uri = $uri;
      $file->filename = $object['filename'];
      $file->filemime = file_get_mimetype($file->uri);
      $file->uid = $object['uid'];
      $file->status = FILE_STATUS_PERMANENT;
      return users_file_save($file, $object);
    }
  }
  return FALSE;
}

function register_users_images() {
  /** find all */
  $query = db_query("SELECT uid, picture FROM {users} WHERE picture <> 0");
  /** return nids */
  foreach ($query->fetchAll() as $user) {
    $result = db_select('mediacommons_base_import_user_map', 'u')
      ->fields('u')
      ->condition('fid', $user->picture)
      ->condition('uid', $user->uid)
      ->execute()
      ->fetchAssoc();
    //$done = mc_user_file_save_data($result);
    print_r($result);
  }
}

function migrate_all_users() {
  drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_FULL);
  $users = d6_users();
  foreach ($users as $user) {
    migrate_user($user);
  }
}

function create_test_users() {
  $users = array(
    // Cluster curator
    'mc.cluster.curator.1' => array(
      'name' => 'mc.cluster.curator.1',
      'mail' => 'mc.cluster.curator.1@nyu.edu',
      'roles' => array(4 => true),
      'pass' => '000000',
      'status' => 1,
    ),
    'mc.cluster.curator.2' => array(
      'name' => 'mc.cluster.curator.2',
      'mail' => 'mc.cluster.curator.2@nyu.edu',
      'roles' => array(4 => true),
      'pass' => '000000',
      'status' => 1,
    ),
    'mc.cluster.curator.3' => array(
      'name' => 'mc.cluster.curator.3',
      'mail' => 'mc.cluster.curator.3@nyu.edu',
      'roles' => array(4 => true),
      'pass' => '000000',
      'status' => 1,
    ),

    // Editors
    'mc.managing.editor.1' => array(
      'name' => 'mc.managing.editor.1',
      'mail' => 'mc.managing.editor.1@nyu.edu',
      'roles' => array(6 => true),
      'pass' => '000000',
      'status' => 1,
    ),
    'mc.managing.editor.2' => array(
      'name' => 'mc.managing.editor.2',
      'mail' => 'mc.managing.editor.2@nyu.edu',
      'roles' => array(6 => true),
      'pass' => '000000',
      'status' => 1,
    ),
    'mc.managing.editor.3' => array(
      'name' => 'mc.managing.editor.3',
      'mail' => 'mc.managing.editor.3@nyu.edu',
      'roles' => array(6 => true),
      'pass' => '000000',
      'status' => 1,
    ),

    // Contributor
    'mc.contributor.1' => array(
      'name' => 'mc.contributor.1',
      'mail' => 'mc.contributor.1@nyu.edu',
      'roles' => array(5 => true),
      'pass' => '000000',
      'status' => 1,
    ),
    'mc.contributor.2' => array(
      'name' => 'mc.contributor.2',
      'mail' => 'mc.contributor.2@nyu.edu',
      'roles' => array(5 => true),
      'pass' => '000000',
      'status' => 1,
    ),
    'mc.contributor.3' => array(
      'name' => 'mc.contributor.3',
      'mail' => 'mc.contributor.3@nyu.edu',
      'roles' => array(5 => true),
      'pass' => '000000',
      'status' => 1,
    ),

    // Administrator
    'mc.administrator.1' => array(
      'name' => 'mc.administrator.1',
      'mail' => 'mc.administrator.1@nyu.edu',
      'roles' => array(3 => true),
      'pass' => '000000',
      'status' => 1,
    ),
    'mc.administrator.2' => array(
      'name' => 'mc.administrator.2',
      'mail' => 'mc.administrator.2@nyu.edu',
      'roles' => array(3 => true),
      'pass' => '000000',
      'status' => 1,
    ),
    'mc.administrator.3' => array(
      'name' => 'mc.administrator.3',
      'mail' => 'mc.administrator.3@nyu.edu',
      'roles' => array(3 => true),
      'pass' => '000000',
      'status' => 1,
    ),
  );
  foreach ($users as $user) {
    user_save(null, $user);
  }
}

function migrate_user($user) {
  
  /** don't waste time migrating blocked users */
  if ($user->status == 0) return;

  /** User already exist in database. */
  if (user_exists_by_ouid($user->uid)) return;
  
  require_once DRUPAL_ROOT . '/' . variable_get('password_inc', 'includes/password.inc');

  /** URL to be used as the production URL; we need to hard-code this */
  $production_url = 'http://mediacommons.futureofthebook.org/';
  
  /** Files folder real path */
  $dupal_file_path = drupal_realpath('public://');
  
  $hash_count_log2 = 11;
  
  $new_user = array(
    'name' => $user->name,
    'pass' => 'U' . user_hash_password($user->pass, $hash_count_log2),
    'mail' => $user->mail,
    'init' => $user->init,
    'status' => $user->status,
    'access' => $user->access,
    'theme' => $user->theme,
    'signature' => $user->signature,
    'signature_format' => $user->signature_format,
    'created' => $user->created,
    'login' => $user->login,
    'language' => $user->language,
    'timezone_name' => $user->timezone_name,
  );
  
  /** if roles availble; map roles to the new rid */
  if (isset($user->roles)) foreach (explode(',', $user->roles) as $orid) $new_user['roles'][rid_by_orid($orid)] = TRUE;

  try {
    /** user picture file identifier + replacement token for user file basename */
    $fid = $ufp = 0;
    if (!empty($user->picture)) {
      $user_picture_url = url($production_url . $user->picture, array('absolute' => TRUE));
      $user_picture = download_user_picture($user_picture_url);
      if ($user_picture) {
         $new_user['picture'] = $user_picture->fid;
         $fid = $user_picture->fid;
         $pathinfo = pathinfo($user->picture);
         $ufp = $pathinfo['basename'];
      }
    }
  }
  catch (Exception $e) {
    drush_log(dt('Caught exception: @exception' , array('@exception' => $e->getMessage())), 'error');
  }
  
  try {
    // https://api.drupal.org/api/drupal/modules!user!user.module/function/user_save/7.x
    $account = user_save(null, $new_user);
    if ($account) {
      // While developing we are using Reroute Email module to aviod sending
      // emails to our users
      // see: https://www.drupal.org/project/reroute_email
      /** User profile picture */
      /** Update realname module */
      if ($realname = d6_users_realname($user->uid)) {
        $realname_account = realname_load_multiple(array($account));
        if (empty($realname_account[0])) {
          $account->realname = $realname;
        	// Save to the database and the static cache.
        	db_merge('realname')->key(array('uid' => $account->uid))->fields(array('realname' => $realname, 'created' => REQUEST_TIME))->execute();
        	// Allow modules to react to the realname being updated.
        	module_invoke_all('realname_update', $realname, $account);
        }
      }

      /** Update profile fields */
      // mediacommons_base_import_migrate_profile_fields($account, $user);

      db_insert('mediacommons_base_import_user_map')->fields(array(
        'ouid' => $user->uid, 
        'uid' => $account->uid, 
        'fid'=> $fid, 
        'filename' => $ufp)
      )->execute();

      drush_log(dt('User @user created', array ('@user' => $user->name)), 'ok');
    }
  }
  catch (Exception $e) {
    drush_log(dt('Caught exception: @exception' , array('@exception' => $e->getMessage())), 'error');
    if ($account) {
      drush_log(dt('Error migrating user @user', array ('@user' => $user->name)), 'error');
      user_delete($account->uid);
    }
  }
}

/** Given a Drupal 6 UID; load D7 user and migrate the profile fields */
function mediacommons_base_import_migrate_profile_fields(&$account, $olduser) {
  /** Profile fields in Drupal 6 */
  $fields = mediacommons_base_import_d6_profile_fields_list();
  // Research Interests
  // Production site use https://www.drupal.org/project/user_terms
  // this field is provided by user_terms
  $fields['profile_research_interests'] = count($fields) + 1;
  /**
   * Map D6 fields to D7 machine name
   * https://jira.nyu.edu/browse/MC-87
   */
  $fields_map = array(
    'profile_name' => array('field' => 'field_profile_name'),
    'profile_url' => array('field' => 'field_url'),
    'profile_affiliation' => array('field' => 'field_organization'),
    'profile_about' => array('field' => 'field_bio'),
    'profile_plan' => array('field' => 'field_plan',),
    'profile_city' => array('field' => 'field_city'),
    'profile_state' => array('field' => 'field_state'),
    'profile_country' => array('field' => 'field_country'),
    'profile_phone_number' => array('field' => 'field_phone'),
    'profile_AIM' => array('field' => 'field_aim'),
    'profile_skype' => array('field' => 'field_skype'),
    'profile_first_name' => array('field' => 'field_first_name'),
    'profile_last_name' => array('field' => 'field_last_name'),
    'profile_email' => array('field' => 'field_email'),
    'profile_title' => array('field' => 'field_title'),
    'profile_research_interests' => array( 'field' => 'field_taxonomy'),
  );

  // map fields
  foreach ($fields as $key => $ofid) {
    if (isset($fields_map[$key])) {
      $fields_map[$key]['ofid'] = $ofid;
      $fields_map[$key]['ofn'] = $key;
      $field_info_instance = field_info_instance('user', $fields_map[$key]['field'], 'user') ;
      $field_info_field = field_info_field ( $fields_map[$key]['field'] ) ;
      if ($field_info_instance) {
        $fields_map[$key]['required'] = $field_info_instance['required'];
        $fields_map[$key]['type'] = $field_info_instance['widget']['type'];
        $fields_map[$key]['settings'] = $field_info_field['settings'];
      }
    }
  }
  
  /**
   * Wrapp user account with an entity metadata wrappers
   * see: https://www.drupal.org/documentation/entity-metadata-wrappers
   */
  $wrapper = entity_metadata_wrapper('user', $account);
  
  // fields
	foreach ($fields_map as $key => $value) {
    try {
    	if (isset($fields_map[$key]['field'])) {
    		$this_field = $wrapper->{$fields_map[$key]['field']};
    	}
    }
    catch (Exception $e) {
    	drush_log(dt('Caught exception: @exception' , array('@exception' => $e->getMessage())), 'error');
    }
    
    if ($key != 'profile_research_interests') {
      $field_value = mediacommons_base_import_d6_profile_field_value($value['ofid'], $olduser->uid);
    }
    elseif ($key == 'profile_research_interests') {
      $field_value = d6_fields_research_interests_field_by_ouid($olduser->uid);
    }
    if ($field_value) {
    	try {
    		if ($fields_map[$key]['type'] == 'text_textfield') {
          try {
            $this_field->set(substr($field_value, 0, $fields_map[$key]['settings']['max_length']));
          }
          catch (EntityMetadataWrapperException $exc) {
            drush_print_r($exc->getTraceAsString());
          }
        }
        else if ($fields_map[$key]['type'] == 'link_field') {
          try {
            $this_field->set(array('url' => $field_value));
          }
          catch (EntityMetadataWrapperException $exc ) {
            drush_print_r($exc->getTraceAsString());
          }
        }
    		else if ($fields_map[$key]['type'] == 'text_textarea') {
    			try {
            $this_field->set(array('value' => $field_value, 'format' => 'filtered_html'));
    			}
    			catch (EntityMetadataWrapperException $exc) {
            drush_print_r($exc->getTraceAsString());
            }
    	    }
    			else if ($fields_map[$key]['type'] == 'telephone_default') {
            try {
    					  $this_field->set($field_value);
    				  }
    					catch (EntityMetadataWrapperException $exc) {
                drush_print_r($exc->getTraceAsString());
              }
    				}
    				else if ($fields_map[$key]['type'] == 'email_textfield') {
    					try {
    					  $this_field->set($field_value);
    				  }
    					catch (EntityMetadataWrapperException $exc) {
                drush_print_r($exc->getTraceAsString());
              }
    				}
    				else if ($fields_map[$key]['type'] === 'taxonomy_autocomplete') {
              if ( $fields_map[$key]['field'] === 'field_taxonomy') {
                $this_field->set($field_value);
              }
    				  elseif ($fields_map[$key]['field'] === 'field_organization') {
    					  $field_value = trim($field_value);
    						/** Organizations */
    						$vocabulary = taxonomy_vocabulary_machine_name_load('organizations');
    						/** Does the vocabulary organizations exist */
    						if ($vocabulary) {
    						  $query = new EntityFieldQuery;
    							$term = $query
    							  ->entityCondition('entity_type', 'taxonomy_term')
    								->propertyCondition('name', $field_value)
    								->propertyCondition('vid', $vocabulary->vid)
    								->execute();
    						  if ($term) {
    							  $keys = array_keys($term['taxonomy_term']);
    								$tid = array_pop($keys);
    							}
    							else {
    							  $term = new stdClass();
    								$term->name = trim($field_value);
    								$term->vid = $vocabulary->vid;
    								taxonomy_term_save($term);
    								$tid = $term->tid;
    							}
    							if ($tid) {
    							  $this_field->set($tid);
    							}
    						}
    					}
    				}
    				else if ($fields_map[$key]['type'] === 'options_select') {
    				  if ($fields_map[$key]['field'] === 'field_city') {
    						// nothing to do at the moment
    					}
    					elseif ( $fields_map[$key]['field'] === 'field_state') {
    					  /** State */
    						$vocabulary = taxonomy_vocabulary_machine_name_load ( 'state' );
    						/** Does the vocabulary organizations exist */
    						if ( $vocabulary ) {
    						  $field_value = trim ($field_value);
    							$query = new EntityFieldQuery;
    							$term = $query
    							  ->entityCondition('entity_type', 'taxonomy_term')
    								->propertyCondition('name', $field_value )
    								->propertyCondition('vid', $vocabulary->vid)
    								->execute();
    							if ( $term ) {
    							  $keys = array_keys($term['taxonomy_term']);
    								$tid = array_pop($keys);
    								$this_field->set($tid );
    							}
    						}
    					}
          elseif ($fields_map[$key]['field'] === 'field_country') {
            /** Country */
      		$vocabulary = taxonomy_vocabulary_machine_name_load('country');
      		/** Does the vocabulary organizations exist */
      		if ($vocabulary) {
      		  $field_value = trim ($field_value);
      		  $query = new EntityFieldQuery;
      		  $term = $query->entityCondition('entity_type', 'taxonomy_term')->propertyCondition('name', $field_value)->propertyCondition('vid', $vocabulary->vid)->execute();
      		  if ($term) {
      			$keys = array_keys($term['taxonomy_term']);
      			$tid = array_pop($keys);
                $this_field->set($tid);
      		  }
      		}
          }
        }
      }
      catch (Exception $e) {
        drush_log('Caught exception: ',  $e->getMessage(), 'error');
      }
    }
  }
  try {
    $wrapper->save();
  }
  catch (Exception $e) {
    drush_log('Caught exception: ',  $e->getMessage(), 'error');
  }
}
