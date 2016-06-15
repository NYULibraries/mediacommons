<?php

function migrate_clusters () {
  foreach (clusters () as $entity) migrate_cluster($entity);
}

function migrate_cluster($entity) {

	/** Prepare node */

	$node = array();

	$node['title'] =  $entity->title;

	$node['ouid'] = $entity->uid;

	$node['onid'] = $entity->nid;

	$node['language'] = LANGUAGE_NONE;

	$node['date'] = date('Y-m-j H:i:s', $entity->created);

	$node['status'] = $entity->status;

	$node['promote'] = $entity->promote;

	$node['moderate'] = $entity->moderate;

	$node['field_pubstat'] = $entity->status;

	$node['sticky'] = $entity->sticky;

	if (isset($entity->field_description_value)) $node['field_body'] = $entity->field_description_value;

	/**
	 * Representative image
	 * Use media type to find image source:
	 * 0 | Embedded video
	 * 1 | Image
	 */
	switch ( $entity->field_cluster_type_value ) {

		case 1 :

			// field_image_fid
			if ( isset ( $entity->field_image_fid ) ) $node['field_representative_image'] = (object) array ( 'fid' => $entity->field_image_fid ) ;

			break ;

		default :

			// field_image_fid
			if ( isset ( $entity->field_tease_image_fid ) ) $node['field_representative_image'] = (object) array( 'fid' => $entity->field_tease_image_fid ) ;

			// if youtube find image using the API
			else {

				if ( isset ( $entity->field_video_embed_link_provider )
						&&
						$entity->field_video_embed_link_provider == 'youtube'
				) {
					$node['field_representative_image'] = (object) array ( 'source' => 'http://img.youtube.com/vi/' . $entity->field_video_embed_link_value  .'/0.jpg', );
				}

			}

			if ( isset ( $entity->field_video_embed_link_embed ) ) {

				$node['field_body'] = '<div class="embed">' . $entity->field_video_embed_link_embed . '</div>' . '<div class="body_content">' . $node['field_body'] . '</div>' ;

			}
	}

	if (isset($entity->pieces)) $node['field_spokes'] = explode( ', ', $entity->pieces );

	if (isset($entity->contributors)) $node['field_contributors'] = explode (', ', $entity->contributors);

  foreach (explode (', ', $entity->terms ) as $tid) {
    if (!empty($tid)) $node['field_tags'][] = $tid;
  }

	/**
	 * Dates
	 * If $dates is set proceed, no good comes from setting dates with wrong values
	 */
	if ( isset ( $entity->field_period_value ) ) $node['field_period_1'] = strtotime ( $entity->field_period_value ) ;

	if ( isset ( $entity->field_period_value2 ) ) $node['field_period_2'] = strtotime ( $entity->field_period_value2 ) ;

	create_hub($node);

}
