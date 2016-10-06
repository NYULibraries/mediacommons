<?php

function download_user_picture($url) {
	$wrapper = file_stream_wrapper_get_instance_by_uri('public://');
	$file_data = drupal_http_request($url);
	$filepath = pathinfo($url);
	$destination_file = 'public://pictures/' . $filepath['basename'];
	if ($file_data) {
		if ($file_data->code == 200) {
			return file_save_data($file_data->data, $destination_file, FILE_EXISTS_REPLACE);
		}
	}
}

function mediacommons_base_import_download_file($url) {

	$wrapper = file_stream_wrapper_get_instance_by_uri('public://');

	if ($wrapper) {
		$destination_dir = $wrapper->realpath();
	}
	else {
		return drush_log('Unable to find destination directory.', 'error');
	}

	$file_data = drupal_http_request($url);

	$filepath = pathinfo($url);

	$destination_file = 'public://' . $filepath['basename'];

	if ($file_data) {
		if ($file_data->code == 200) {
			return file_save_data($file_data->data, $destination_file, FILE_EXISTS_REPLACE);
		}
	}

}

function download_files() {

	global $settings;

	$production_base_url = $settings['mediacommons']['environments']['production'][ $settings['mediacommons']['umbrella'] ] ;

	$production_url = mediacommons_base_import_get_production_url () ;

	$files = array ( ) ;

	if ( $wrapper = file_stream_wrapper_get_instance_by_uri( 'public://' ) ) $destination_dir = $wrapper->realpath() ;

	else return drush_log ( 'Unable to find destination directory.', 'error' ) ;

	foreach ( d6_files ( ) as $file ) {

		$ofid = $file->fid ;

		if ( ! $ofid ) continue ;

		$pathinfo = pathinfo ( $file->filename ) ;

		$filepath = pathinfo ( $file->filepath ) ;

		if ( $pathinfo['extension'] ) $filename = $file->filename ;

		/** because of some idiot */
		else $filename = basename ( $file->filepath ) ;

		$url_image_path = $production_base_url . ( dirname ( $file->filepath ) . '/' . rawurlencode ( $filename ) ) ;

		$destination_file = $destination_dir . '/' . $filename ;

		if ( $filepath['basename'] !== $filename ) {

			$file_data = drupal_http_request ( $production_base_url . ( dirname ( $file->filepath ) . '/' . rawurlencode ( $filepath['basename'] ) )  ) ;

			$saved_file = file_save_data ( $file_data->data, 'public://' . $filepath['basename'] ) ;

			if ( $saved_file ) {

				drush_log ( dt ( '@filename saved.' , array ( '@filename' => basename ( $destination_file  ) ) ), 'status' ) ;

				/** map relationship */
				if ( ! fid_by_ofid ( $ofid ) ) db_insert('mediacommons_base_import_files_map')->fields( array ( 'ofid' => $file->fid, 'fid' => $saved_file->fid ) )->execute() ;

			}

		}

		if ( ! file_exists ( $destination_file ) ) {
			$file_data = drupal_http_request ( $url_image_path ) ;
			if ( $file_data ) {
				if ( $file_data->code == 200 ) {
					$saved_file = file_save_data ( $file_data->data, 'public://' . $filename ) ;
					if ( $saved_file ) {
						drush_log ( dt('@filename saved.', array( '@filename' => basename ( $destination_file  ) ) ), 'status' ) ;
						/** map relationship */
						if ( ! fid_by_ofid ( $ofid ) ) db_insert('mediacommons_base_import_files_map')->fields( array ( 'ofid' => $file->fid, 'fid' => $saved_file->fid ) )->execute() ;
					}
				}
				else {
					if ( $old_image = mediacommons_base_import_download_file ( $production_url . $file->filepath ) ) {
						/** map relationship */
						if ( ! fid_by_ofid ( $ofid ) ) db_insert('mediacommons_base_import_files_map')->fields( array( 'ofid' => $file->fid, 'fid' => $old_image->fid ) )->execute() ;
					}
				}
			}
			else {
				drush_log ( dt('Unable to save @filename', array ('@filename' => $file->filename ) ) , 'error' ) ;
				if ( ! fid_by_ofid ( $ofid ) ) db_insert('mediacommons_base_import_files_map')->fields( array( 'ofid' => $file->fid, 'fid' => 0 ) )->execute() ;
			}
		}

		/** file exist in destination folder; save us a request */
		else {

			drush_log ( dt('@filename saved in Drupal files registry.' , array ('@filename' => $filename ) ), 'status' ) ;

			$saved_file = (object) array( 'uid' => 1, 'uri' => $destination_file, 'filemime' => file_get_mimetype ( $destination_file ), 'status' => 1 ) ;

			$saved_file = file_copy ( $saved_file, 'public://') ;

			/** map relationship */

			// check if record exist before this

			if ( ! fid_by_ofid ( $ofid ) ) db_insert('mediacommons_base_import_files_map')->fields( array( 'ofid' => $file->fid, 'fid' => $saved_file->fid ) )->execute() ;

		}

	}

}

function download_image_attach_files ( ) {

	global $settings;

	$production_base_url = $settings['mediacommons']['environments']['production'][ $settings['mediacommons']['umbrella'] ] ;

	$production_url = mediacommons_base_import_get_production_url () ;

	$files = array ( ) ;

	if ( $wrapper = file_stream_wrapper_get_instance_by_uri( 'public://' ) ) $destination_dir = $wrapper->realpath() ;

	else return drush_log ( 'Unable to find destination directory.', 'error' ) ;

	foreach ( image_attach_files ( ) as $file ) {

		$ofid = $file->fid ;

		if ( ! $ofid ) continue ;

		$pathinfo = pathinfo ( $file->filename ) ;

		$filepath = pathinfo ( $file->filepath ) ;

		if ( $pathinfo['extension'] ) $filename = $file->filename ;

		/** because of some idiot */
		else $filename = basename ( $file->filepath ) ;

		$url_image_path = $production_base_url . ( dirname ( $file->filepath ) . '/' . rawurlencode ( $filename ) ) ;

		$destination_file = $destination_dir . '/' . $filename ;

		if ( $filepath['basename'] !== $filename ) {

			$file_data = drupal_http_request ( $production_base_url . ( dirname ( $file->filepath ) . '/' . rawurlencode ( $filepath['basename'] ) )  ) ;

			$saved_file = file_save_data ( $file_data->data, 'public://' . $filepath['basename'] ) ;

			if ( $saved_file ) {

				drush_log ( dt ( '@filename saved.' , array ( '@filename' => basename ( $destination_file  ) ) ), 'status' ) ;

				/** map relationship */
				if ( ! fid_by_ofid ( $ofid ) ) db_insert('mediacommons_base_import_files_map')->fields( array ( 'ofid' => $file->fid, 'fid' => $saved_file->fid ) )->execute() ;

			}

		}

		if ( ! file_exists ( $destination_file ) ) {
			$file_data = drupal_http_request ( $url_image_path ) ;
			if ( $file_data ) {
				if ( $file_data->code == 200 ) {
					$saved_file = file_save_data ( $file_data->data, 'public://' . $filename ) ;
					if ( $saved_file ) {
						drush_log ( dt('@filename saved.', array( '@filename' => basename ( $destination_file  ) ) ), 'status' ) ;
						/** map relationship */
						if ( ! fid_by_ofid ( $ofid ) ) db_insert('mediacommons_base_import_files_map')->fields( array ( 'ofid' => $file->fid, 'fid' => $saved_file->fid ) )->execute() ;
					}
				}
				else {
					if ( $old_image = mediacommons_base_import_download_file ( $production_url . $file->filepath ) ) {
						/** map relationship */
						if ( ! fid_by_ofid ( $ofid ) ) db_insert('mediacommons_base_import_files_map')->fields( array( 'ofid' => $file->fid, 'fid' => $old_image->fid ) )->execute() ;
					}
				}
			}
			else {
				drush_log ( dt('Unable to save @filename', array ('@filename' => $file->filename ) ) , 'error' ) ;
				if ( ! fid_by_ofid ( $ofid ) ) db_insert('mediacommons_base_import_files_map')->fields( array( 'ofid' => $file->fid, 'fid' => 0 ) )->execute() ;
			}
		}

		/** file exist in destination folder; save us a request */
		else {

			drush_log ( dt('@filename saved in Drupal files registry.' , array ('@filename' => $filename ) ), 'status' ) ;

			$saved_file = (object) array( 'uid' => 1, 'uri' => $destination_file, 'filemime' => file_get_mimetype ( $destination_file ), 'status' => 1 ) ;

			$saved_file = file_copy ( $saved_file, 'public://') ;

			/** map relationship */

			 // check if record exist before this
			 if ( ! fid_by_ofid ( $ofid ) ) db_insert('mediacommons_base_import_files_map')->fields( array( 'ofid' => $ofid, 'fid' => $saved_file->fid ) )->execute() ;

		}

	}

}
