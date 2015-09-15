<?php

function mediacommons_base_import_d6_hub_list ( ) {

	$query = "
    SELECT
      node.nid,
      node.type,
      node.uid,
      node.vid,
      node.title,
      node.status,
      node.created,
      node.changed,
      node.comment,
      cluster.field_description_value,
      cluster.field_period_value,
      cluster.field_period_value2,
      cluster.field_cluster_type_value,
      cluster.field_tease_image_fid,
      cluster.field_image_fid,
      cluster.field_video_embed_link_value AS video_link,
      cluster.field_video_embed_link_provider AS provider,
      f.filename AS filename,
      GROUP_CONCAT(DISTINCT cfc.field_contributors_uid SEPARATOR ', ') AS contributors,
      GROUP_CONCAT(DISTINCT cfc.delta SEPARATOR ', ') AS contributor_order,
      GROUP_CONCAT(DISTINCT t.tid SEPARATOR ', ') AS terms
    FROM {node} node
    LEFT JOIN {content_type_cluster} cluster
      ON node.vid = cluster.vid
    LEFT JOIN {content_field_contributors} cfc
      ON node.vid = cfc.vid
    LEFT JOIN {files} f
      ON cluster.field_image_fid = f.fid
    LEFT JOIN {term_node} t
      ON node.vid = t.vid
    WHERE node.type = 'cluster' AND node.status = 1
    GROUP BY node.nid
    ORDER BY node.nid";

	/** Create a database connection to the source (d6) database */
	$result = Database::getConnection('default', 'drupal6')->query($query, array(), array());

	return $result;

}
