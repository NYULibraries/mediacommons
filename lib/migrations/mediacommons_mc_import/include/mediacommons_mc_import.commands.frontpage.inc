<?php

/**
 * Migrate frontpage nodes.
 */
function migrate_frontpages() {
	foreach ( frontpages () as $key => $old_node ) migrate_frontpage ( $old_node ) ;
}

/**
 * Migrate frontpage nodes.
 */
function migrate_frontpage($old_node) {

	/** prepare node */
	$node = array () ;

	$node['title'] = $old_node->title ;

	$node['ouid'] = $old_node->uid ;

	$node['onid'] = $old_node->nid ;

	$node['language'] = LANGUAGE_NONE ;

	$node['date'] = date( 'Y-m-j H:i:s', $old_node->created ) ;

	$node['status'] = $old_node->status ;

	$node['promote'] = $old_node->promote ;

	$node['moderate'] = $old_node->moderate ;

	$node['sticky'] = $old_node->sticky ;

	// we need to add this to the content type?
	$node['field_pubstat'] = $old_node->status ;

	extract_iframe_sources ( $old_node->field_fpp_description_value ) ;

	// Description
	if (isset($old_node->field_fpp_description_value)) {
	  $node['field_body'] = $old_node->field_fpp_description_value;
	}

	// Link to Resource
	if (isset($old_node->field_fpp_link_title) && isset($old_node->field_fpp_link_url)) {
	  $node['field_rlink'] = (object) array(
		  'title' => $old_node->field_fpp_link_title,
		  'url' => $old_node->field_fpp_link_url,
	  );
	}

	// Project
	$allowed_project_value = array(
	  'MediaCommons' => 'mc',
		'Field Guide' => 'fg',
		'In Media Res' => 'imr',
		'The New Everyday' => 'tne',
		'Alt-Ac' => 'alt',
		'MediaCommons Press' => 'mcpress',
		'[in]Transition' => 'int',
	);

	if (isset($old_node->field_fpp_project_value) && isset($allowed_project_value[$old_node->field_fpp_project_value])) {
		$node['field_project'] = $allowed_project_value[$old_node->field_fpp_project_value];
	}

	if (isset($old_node->filename)) {
		// Featured Image Thumb
		$node['field_featured_image_thumb'] = ( object ) array ( 'filename' => $old_node->filename , 'fid' => $old_node->fid);
		// Large image
		$node['field_large_image'] = ( object ) array ( 'filename' => $old_node->filename, 'fid' => $old_node->fid);
	}

	create_frontpage($node);

}
