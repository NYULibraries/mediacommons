<?php

/**
 * Migrate frontpage nodes.
 */
function migrate_frontpages ( ) {
	foreach ( frontpages () as $key => $old_node ) migrate_frontpage ( $old_node ) ;
}

/**
 * Migrate IMR posts.
 */
function migrate_frontpage ( $old_node ) {

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

	$node['field_pubstat'] = $old_node->status ;

	extract_iframe_sources ( $old_node->field_fpp_description_value ) ;

	$node['field_body'] = $old_node->field_fpp_description_value ;

	/**
	if (isset($hub->field_fpp_link_url) && isset($hub->field_fpp_link_title)) {

		$node['field_link'] = (object) array(
				'title' => $hub->field_fpp_link_title,
				'link' => $hub->field_fpp_link_url,
		);
	}
	*/

	/**
	if (isset($hub->field_fpp_project_value)) {
		$node['field_project'] = $hub->field_fpp_project_value;
	}
	*/

	/** representative image */
	// if ( isset ( $old_node->field_fpp_image_fid ) ) $node['field_attached_images_ri'] = ( object ) array ( 'fid' => $old_node->field_fpp_image_fid ) ;

	create_spoke ( $node ) ;

}
