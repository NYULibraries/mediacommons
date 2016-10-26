<?php

function migrate_all_users() {
	foreach (d6_users() as $user) {
		migrate_user($user);
	}
}

function migrate_user($user) {

  /** don't waste time migrating blocked users */
	if ($user->status == 0) return;
	
	drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_FULL);	

	require_once DRUPAL_ROOT . '/' . variable_get('password_inc', 'includes/password.inc');

	$super_users = array('aortiz@nyu.edu', 'lh435@nyu.edu');

  /** URL to be used as the production URL; we need to hard-code this */
  $production_url = umbrella_production_url();

  /** Files folder real path */
  $dupal_file_path = drupal_realpath('public://');

  $user_exists = user_exists_by_ouid($user->uid);

	$hash_count_log2 = 11;

  // User already exist in database.
  if ($user_exists) return;

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
  if (isset($user->roles)) {
		foreach (explode(',', $user->roles) as $orid) {
			$new_user['roles'][rid_by_orid($orid)] = TRUE;
		}
	}
  try {
		// https://api.drupal.org/api/drupal/modules!user!user.module/function/user_save/7.x
		$account = user_save(null, $new_user);
		if ($account) {
			drush_log(dt('User @user created', array ('@user' => $user->name)), 'ok');
      db_insert('mediacommons_base_import_user_map')->fields(array('ouid' => $user->uid, 'uid' => $account->uid))->execute();
		}
	}
	catch (Exception $e) {
		drush_log(dt('Caught exception: @exception' , array('@exception' => $e->getMessage())), 'error');
		drush_log(dt('Error migrating user @user', array ('@user' => $user->name)), 'error');
		return;
	}

	if ($account) {
		// While developing we are using Reroute Email module to aviod sending
		// emails to our users
		// see: https://www.drupal.org/project/reroute_email
    /** User profile picture */
    if ($user->picture) {
			$user_picture = download_user_picture(url($production_url . $user->picture, array('absolute' => TRUE)));
			// picture saved and managed by Drupal (see: file_managed)
      if ($user_picture) {
				db_update('users')->fields(array('picture' => $user_picture->fid))->condition('uid', $account->uid)->execute();
				db_update('mediacommons_base_import_user_map')->fields(array('fid' => $user_picture->fid, 'filename' => $user_picture->filename))->condition('uid', $account->uid)->execute();
				$account->picture = $user_picture->fid;
      }
    }
    /** Update realname module */
    $realname = d6_users_realname($user->uid);
    if ($realname) {
      $realname_account = realname_load_multiple(array($account));
      if (empty($realname_account[0])) {
      	// Save to the database and the static cache.
      	db_merge('realname')->key(array('uid' => $account->uid))->fields(array('realname' => $realname, 'created' => REQUEST_TIME))->execute();
      	// Allow modules to react to the realname being updated.
      	module_invoke_all('realname_update', $realname, $account);
      }
    }
    /** Update profile fields */
    mediacommons_base_import_migrate_profile_fields($user->uid);
  }
}

/** Given a Drupal 6 UID; load D7 user and migrate the profile fields */
function mediacommons_base_import_migrate_profile_fields($ouid) {

	$uid = user_exists_by_ouid($ouid);

  $account = user_load($uid, TRUE);

  /** Profile fields in Drupal 6 */
  $fields = mediacommons_base_import_d6_profile_fields_list();

  /**
   * Map D6 fields to D7 machine name
   * https://jira.nyu.edu/browse/MC-87
   */
  $fields_map = array (
    'profile_name' => array('field' => 'field_profile_name'),
		'profile_first_name' => array( 'field' => 'field_first_name'),
    'profile_last_name' => array( 'field' => 'field_last_name'),
    'profile_url' => array( 'field' => 'field_url'),
    'profile_affiliation' => array( 'field' => 'field_organization'),
    'profile_about' => array( 'field' => 'field_bio'),
    'profile_plan' => array( 'field' => 'field_plan',),
    'profile_city' => array( 'field' => 'field_city'),
    'profile_state' => array( 'field' => 'field_state'),
    'profile_country' => array( 'field' => 'field_country'),
    'profile_phone_number' => array( 'field' => 'field_phone'),
    'profile_AIM' => array( 'field' => 'field_aim'),
    'profile_skype' => array( 'field' => 'field_skype'),
		'profile_title' => array( 'field' => 'field_title'),
    'profile_email' => array( 'field' => 'field_email'),
  );

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
	foreach ($fields_map as $key => $value) {
		$this_field = $wrapper->$fields_map[$key]['field'];
		$value = mediacommons_base_import_d6_profile_field_value($value['ofid'], $ouid);
		if ($value) {
			try {
			  if ($fields_map[$key]['type'] == 'text_textfield') {
          try {
            $this_field->set(substr(utf8_encode($value), 0, $fields_map[$key]['settings']['max_length']));
          }
          catch (EntityMetadataWrapperException $exc) {
            drush_print_r($exc->getTraceAsString());
          }
        }
        else if ($fields_map[$key]['type'] == 'link_field') {
          try {
            $this_field->set(array('url' => utf8_encode($value)));
          }
          catch (EntityMetadataWrapperException $exc ) {
            drush_print_r($exc->getTraceAsString());
          }
        }
			  else if ($fields_map[$key]['type'] == 'text_textarea') {
				  try {
            $this_field->set(array('value' => utf8_encode($value), 'format' => 'filtered_html'));
				  }
				  catch (EntityMetadataWrapperException $exc) {
            drush_print_r($exc->getTraceAsString());
          }
	      }
				else if ($fields_map[$key]['type'] == 'telephone_default') {
          try {
					  $this_field->set(utf8_encode($value));
				  }
					catch (EntityMetadataWrapperException $exc) {
            drush_print_r($exc->getTraceAsString());
          }
				}
				else if ($fields_map[$key]['type'] == 'email_textfield') {
					try {
					  $this_field->set(utf8_encode($value));
				  }
					catch (EntityMetadataWrapperException $exc) {
            drush_print_r($exc->getTraceAsString());
          }
				}
				else if ($fields_map[$key]['type'] === 'taxonomy_autocomplete') {
				  if ( $fields_map[$key]['field'] === 'field_organization') {
					  $value = trim(utf8_encode($value));
						/** Organizations */
						$vocabulary = taxonomy_vocabulary_machine_name_load('organizations');
						/** Does the vocabulary organizations exist */
						if ($vocabulary) {
						  $query = new EntityFieldQuery;
							$term = $query
							  ->entityCondition('entity_type', 'taxonomy_term')
								->propertyCondition('name', $value)
								->propertyCondition('vid', $vocabulary->vid)
								->execute();
						  if ($term) {
							  $keys = array_keys($term['taxonomy_term']);
								$tid = array_pop($keys);
							}
							else {
							  $term = new stdClass();
								$term->name = trim($value);
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
						  $value = trim (utf8_encode($value));
							$query = new EntityFieldQuery;
							$term = $query
							  ->entityCondition('entity_type', 'taxonomy_term')
								->propertyCondition('name', $value )
								->propertyCondition('vid', $vocabulary->vid)
								->execute();
							if ( $term ) {
							  $keys = array_keys($term['taxonomy_term']);
								$tid = array_pop($keys);
								$this_field->set($tid );
							}
						}
					}
				}
				else if ($fields_map[$key]['field'] === 'field_country') {
		      /** Country */
					$vocabulary = taxonomy_vocabulary_machine_name_load('country');
					/** Does the vocabulary organizations exist */
					if ($vocabulary) {
					  $value = trim (utf8_encode($value));
						$query = new EntityFieldQuery;
						$term = $query->entityCondition('entity_type', 'taxonomy_term')
							            ->propertyCondition('name', $value)
							            ->propertyCondition('vid', $vocabulary->vid)
							            ->execute();
						if ($term) {
						      $keys = array_keys($term['taxonomy_term']);
							    $tid = array_pop($keys);
							    $this_field->set($tid);
						}
				  }
		    }
				else {}
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
