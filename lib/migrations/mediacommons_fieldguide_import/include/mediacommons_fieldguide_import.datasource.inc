<?php

function survey_response_by_question_nid($nid) {
	$query = "
      SELECT n.nid,
			 nodequeue.position
    FROM {node} n
      LEFT JOIN {content_type_survey_response} ct
        ON n.vid = ct.vid
  	  LEFT JOIN {nodequeue_nodes} nodequeue
        ON n.nid = nodequeue.nid
    WHERE n.type = :type
    AND ct.field_sr_reference_nid = :nid
    GROUP BY n.nid
    ORDER BY nodequeue.position";

	/** Create and return a database connection to the source (d6) database */
	return Database::getConnection('default', 'drupal6')->query( $query, array(':type' => 'survey_response' , ':nid' => $nid ), array() ) ;

}

function survey_responses () {
  $query = "
    SELECT n.nid,
       MAX(n.vid) AS vid,
       n.type,
       n.language,
       n.uid,
       n.status,
       n.created,
       n.changed,
       n.comment,
       n.promote,
       n.moderate,
       n.sticky,
       nr.title,
       ct.*,
  		 nodequeue.position,
  	   GROUP_CONCAT(DISTINCT cfri.field_resp_image_fid ORDER by cfri.delta SEPARATOR ', ') AS images
    FROM {node} n
      LEFT JOIN {node_revisions} nr
        ON n.vid = nr.vid
      LEFT JOIN {content_type_survey_response} ct
        ON n.vid = ct.vid
  	  LEFT JOIN {content_field_resp_image} cfri
        ON n.vid = cfri.vid
  	  LEFT JOIN {nodequeue_nodes} nodequeue
        ON n.nid = nodequeue.nid
    WHERE n.type = :type
    AND n.status > 0
    GROUP BY n.nid
    ORDER BY n.nid";

  /** Create and return a database connection to the source (d6) database */
  return Database::getConnection('default', 'drupal6')->query($query, array(':type' => 'survey_response'), array());

}

function attached_images_by_ofid ( $ofid ) {

  $query = "SELECT fid, filepath, filename FROM {files} WHERE fid = :ofid";

  /** Create and return a database connection to the source (d6) database */
  $file = Database::getConnection('default', 'drupal6')->query($query, array(':ofid' => $ofid), array())->fetchObject();

  return $file;

}

function survey_questions ( ) {

  $query = "
    SELECT n.nid,
       MAX(n.vid) AS vid,
       n.type,
       n.language,
       n.uid,
       n.status,
       n.created,
       n.changed,
       n.comment,
       n.promote,
       n.moderate,
       n.sticky,
       nr.title
    FROM {node} n
      LEFT JOIN {node_revisions} nr
        ON n.vid = nr.vid
    WHERE n.type = :type
    GROUP BY n.nid
    ORDER BY n.nid";

  /** Create and return a database connection to the source (d6) database */
  return Database::getConnection('default', 'drupal6')->query( $query, array( ':type' => 'survey_question'), array());

}
