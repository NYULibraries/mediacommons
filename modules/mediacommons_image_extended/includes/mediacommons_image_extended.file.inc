<?php

/**
 * @file
 * hook_file and mediacommons_image_extended file functions.
 */

/** Implements hook_file_insert(). Generate a thumbnail and admin thumbnail for the image. */

// function mediacommons_image_extended_file_insert($file) {}

/** Implements hook_file_delete(). Delete the thumbnail and admin thumbnail when the original is deleted. */
// function mediacommons_image_extended_file_delete($file) {}

/** Given a file, return the path the image thumbnail used while editing */
function mediacommons_image_extended_file_path($file, $create_thumb = FALSE, $type = 'thumbnail') {
  
  dpm(__FUNCTION__);
  dpm('file');
  dpm($file);
  dpm('create_thumb ' . $create_thumb);
  dpm('type: ' . $type);
  
  $filepath = file_default_scheme() . ':/' . '/mediacommons_image_extended/' . mediacommons_image_extended_file_filename_convert($file);
  
  dpm('filepath: ' . $filepath);
  
  if ($create_thumb) {
    
    if ($type == 'thumbnail') {
      mediacommons_image_extended_file_create_derivative_image(((isset($file['uri'])) ? drupal_realpath($file['uri']) : $file['filepath']), $filepath);
    }
    
    /** if we are asking for the original image, why create a derivative ? */
    if ($type == 'original') {
      mediacommons_image_extended_file_create_derivative_image(((isset($file['uri'])) ? drupal_realpath($file['uri']) : $file['filepath']), $filepath, $file['width'], $file['height'], 'resize');
    }

  }
  return $filepath;
}

/**
 * Given a source image, create a derivative image.
 *
 * @param $source_file
 * @param $destination_file
 * @param $width
 * @param $height
 * @param $op
 * @return
 *   TRUE if the derivative image is created.
 *   FALSE otherwise.
 */
function mediacommons_image_extended_file_create_derivative_image($source_file, $destination_file, $width = 100, $height = 100, $op = 'scale') {
  
  if (!is_file($source_file)) {
    drupal_set_message(t('Source image @path does not appear to exist.', array('@path' => $source_file)), 'warning', TRUE);
    /** watchdog('mediacommons_image_extended', 'Source image does not appear to exist.', array(), 'WATCHDOG_WARNING'); */
    return false;
  }

  /** If the destination file already exists, see if it needs to be regenerated. */
  if (is_file($destination_file)) {
    
    dpm('If the destination file already exists, see if it needs to be regenerated');
        
    dpm('width: ' . $width);
    
    dpm('height: ' . $height);
    
    $source_info = mediacommons_image_extended_file_image_get_info($source_file);
    
    dpm($source_info);
    
    $destination_info = mediacommons_image_extended_file_image_get_info($destination_file);
    
    dpm($destination_info);
    
    if (
      $destination_info['width'] != $width && $destination_info['height'] != $height &&
      ($source_info['width'] > $width || $source_info['height'] > $height)
    ) {
      unlink($destination_file);
    }
    
    else {
      return;
    }
    
  }
  
  /** Ensure the destination directory exists and is writable */
  if (!mediacommons_image_extended_file_directory_exists($destination_file)) {
    drupal_set_message(t('Unable to save image in directory.'), 'error');
    watchdog('mediacommons_image_extended', 'Unable to save image in directory.', array(), WATCHDOG_WARNING);
    return FALSE; 
  }

  /** Create the derivative image */
  if (
      module_exists('imagemagick') && 
      image_get_toolkit() == 'imagemagick'
  ) {

    $image = image_load($source_file);
    
    (($op == 'scale') ? image_scale($image, $width) : image_resize($image, $width, $height)); 
    
    // FIXME: It appears we need to create an empty file for image_save to
    // work. There is a drupal_realpath() call in _imagemagick_convert which
    // appears to fail if nothing exists at a given destination. The error
    // here should be run down and a bug filed.
    
    $destination_file = file_unmanaged_save_data('', $destination_file);
    
    return image_save($image, $destination_file);
  }
  
  else {
    drupal_set_message(t('Unable to create image.'), 'error');
  }
  
  return false;
  
}

/**
 *
 * Taken from filefield.module and modified
 *
 * An #upload_validators callback. Check that a file is an image.
 *
 * This check should allow any image that PHP can identify, including png, jpg,
 * gif, tif, bmp, psd, swc, iff, jpc, jp2, jpx, jb2, xbm, and wbmp.
 *
 * This check should be combined with filefield_validate_extensions() to ensure
 * only web-based images are allowed, however it provides a better check than
 * extension checking alone if the mimedetect module is not available.
 *
 * @param $file
 *   A Drupal file object.
 * @return
 *   An array of any errors cause by this file if it failed validation.
 */
function mediacommons_image_extended_file_validate_is_image( &$file ) {
  $errors = array();
  
  $info = mediacommons_image_extended_file_image_get_info($file->filepath);
  if ( !$info || empty( $info['extension'] ) ) {
    $errors[] = t('The file is not a known image format.');
  }
  return $errors;
}

/**
 * Taken from includes/image.inc and modified; aof
 *
 * Get details about an image.
 */
function mediacommons_image_extended_file_image_get_info( $file ) {
  if ( !is_file( $file ) ) {
    return FALSE;
  }
  
  $details = FALSE;
  
  $data = @getimagesize($file);
  
  $file_size = @filesize($file);
   
  if (isset($data) && is_array($data)) {
    $extensions = array(
      '2' => 'jpg',
      '7' => 'tif',
      '10' => 'jp2',
    );

    $extension = array_key_exists( $data[2], $extensions) ?  $extensions[$data[2]] : '';

    $details = array(
      'width' => $data[0],
      'height' => $data[1],
      'extension' => $extension,
      'file_size' => $file_size,
      'mime_type' => $data['mime'],
    );
  }
  return $details;
}

/** Simple utility function to check if a file is an image */
function mediacommons_image_extended_file_is_image($file) {
  return in_array($file->filemime, array('image/jpg', 'image/jpeg', 'image/tiff', 'application/octet-stream', 'image/jp2', 'image/jpeg2000', 'image/jpeg2000-image', 'image/x-jpeg2000-image'));
}

/**
 * Is the available ImageMagick library able to work with JP2 files?
 *
 * @todo Assuming that the destination file existing is proof of this test
 * passing is probably not safe. Ideally, the destination file would be
 * created on every run of this function and then deleted.
 *
 * @param $source_file
 *   The path to a JP2 image.
 * @param $destination_file
 *   The path to a file. The path will be created if it does not exist.
 * @return
 *   TRUE if ImageMagick is able to work with JP2 files.
 *   FALSE otherwise.
 */
function mediacommons_image_extended_file_test_imagemagick($source_file, $destination_file) {
  
  /**
   * The source file needs to exist, the imagemagick api needs to be the
   * default image toolkit, and the destination must exist and be writable.
   */
  if (!is_file($source_file)
      || (image_get_toolkit() != 'imagemagick')
      || (!mediacommons_image_extended_file_directory_exists($destination_file))) {
    return FALSE;
  }
  
  /** 
   * Check if the destination image exists; if image exists then we already pass this test.
   * Add a check to verify that the file in question is an image file.
   */
  elseif (is_file($destination_file) && mediacommons_image_extended_file_image_get_info($destination_file)) {
    return TRUE;
  }
  
  /** Verify source is a valid image */  
  elseif (mediacommons_image_extended_file_image_get_info($source_file)) {
    return mediacommons_image_extended_file_create_derivative_image($source_file, $destination_file, 10, 10, 'resize');
  }
  
  else {
    return FALSE;
  }

}

/**
 * Try to ensure that a directory path exists and is writable.
 *
 * If a path does not exist, try to create it. If a path is not writable, try
 * to make it writable.
 *
 * @param $file
 *   The path to a file. The path does not need to exist.
 * @return
 *   TRUE if the directory exists and is writable.
 *   FALSE otherwise.
 */
function mediacommons_image_extended_file_directory_exists($file) {
  $dir = drupal_dirname($file);
  return file_prepare_directory($dir, FILE_CREATE_DIRECTORY^FILE_MODIFY_PERMISSIONS);
}

/** Helper function to ensure derivative image are websafe */
function mediacommons_image_extended_file_filename_convert($file, $extension = 'jpg') {
  if (in_array($extension, array('png', 'jpg', 'jpeg')) ) {
    $short_path = pathinfo(drupal_realpath($file['filename']));
    $filename = $short_path['filename'];
    if ($extension != 'jpg') {
      $fileextension = $short_path['extension'];
    }
    else {
      $fileextension = $extension;
    }
    $pathinfo = pathinfo($file['filename']);
    return $pathinfo['filename'] . '.' . $extension;
  }
}
