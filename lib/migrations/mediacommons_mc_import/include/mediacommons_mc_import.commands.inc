<?php

function _file_save_data($data, $destination, $user) {
  $uri = $destination;
  if ($uri) {
    $file = new stdClass();
    $file->fid = NULL;
    $file->uri = $uri;
    $file->filename = drupal_basename($uri);
    $file->filemime = file_get_mimetype($file->uri);
    $file->uid = $user->uid;
    $file->status = FILE_STATUS_PERMANENT;
    $existing_files = file_load_multiple(array(), array('uri' => $uri));
    if (count($existing_files)) {
      $existing = reset($existing_files);
      $file->fid = $existing->fid;
      $file->filename = $existing->filename;
    }
    return file_save($file);
  }
  return FALSE;
}

function _MC277() {
  $users = db_query('SELECT * FROM {mediacommons_base_import_user_map}');
  foreach ($users as $user) {
    if ($user->uid) { // always true
      if (!empty($user->filename)) {
        $files_directory_realpath = drupal_realpath('public://');
        $pictures_directory_realpath = $files_directory_realpath . '/pictures';
        $picture_realpath = $pictures_directory_realpath . '/' . $user->filename;
        if (is_file($picture_realpath)) {
          $entity = user_load($user->uid, TRUE);
          if (empty($entity->picture) || $entity->picture != 0) {
            $is_fid = find_image_by_filename($user->filename); 
            // file in database
            if ($is_fid) {
              drush_log($is_fid, 'ok');
            }
            // File is not regiter in database
            else {
              $file = _file_save_data($picture_realpath, 'public://pictures/' . $user->filename, $user);
              if ($file) {
                $transaction = user_save($entity, array('picture' => $file));
              }
            }
          }
        }
      }
    }
  }
}

