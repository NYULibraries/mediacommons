<?php
/**
 * @file
 * mediacommons_feature.features.user_role.inc
 */

/**
 * Implements hook_user_default_roles().
 */
function mediacommons_feature_user_default_roles() {
  $roles = array();

  // Exported role: administrator.
  $roles['administrator'] = array(
    'name' => 'administrator',
    'weight' => '2',
  );

  // Exported role: curator.
  $roles['curator'] = array(
    'name' => 'curator',
    'weight' => '4',
  );

  // Exported role: managing editor.
  $roles['managing editor'] = array(
    'name' => 'managing editor',
    'weight' => '3',
  );

  return $roles;
}
