<?php
/**
 * @file
 * mediacommons_types.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function mediacommons_types_user_default_permissions() {
  $permissions = array();

  // Exported permission: 'access mediacommons content'.
  $permissions['access mediacommons content'] = array(
    'name' => 'access mediacommons content',
    'roles' => array(),
    'module' => 'mediacommons_editorialworkflow',
  );

  // Exported permission: 'access mediacommons dashboard'.
  $permissions['access mediacommons dashboard'] = array(
    'name' => 'access mediacommons dashboard',
    'roles' => array(),
    'module' => 'mediacommons_editorialworkflow',
  );

  // Exported permission: 'admin contributors content'.
  $permissions['admin contributors content'] = array(
    'name' => 'admin contributors content',
    'roles' => array(),
    'module' => 'mediacommons_editorialworkflow',
  );

  // Exported permission: 'contribute content hub'.
  $permissions['contribute content hub'] = array(
    'name' => 'contribute content hub',
    'roles' => array(),
    'module' => 'mediacommons_editorialworkflow',
  );

  // Exported permission: 'contribute content spoke'.
  $permissions['contribute content spoke'] = array(
    'name' => 'contribute content spoke',
    'roles' => array(),
    'module' => 'mediacommons_editorialworkflow',
  );

  // Exported permission: 'promote hub content'.
  $permissions['promote hub content'] = array(
    'name' => 'promote hub content',
    'roles' => array(),
    'module' => 'mediacommons_editorialworkflow',
  );

  // Exported permission: 'promote users role'.
  $permissions['promote users role'] = array(
    'name' => 'promote users role',
    'roles' => array(),
    'module' => 'mediacommons_editorialworkflow',
  );

  // Exported permission: 'set current hub'.
  $permissions['set current hub'] = array(
    'name' => 'set current hub',
    'roles' => array(),
    'module' => 'mediacommons_editorialworkflow',
  );

  // Exported permission: 'set upcoming hub'.
  $permissions['set upcoming hub'] = array(
    'name' => 'set upcoming hub',
    'roles' => array(),
    'module' => 'mediacommons_editorialworkflow',
  );

  return $permissions;
}
