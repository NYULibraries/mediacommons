<?php

function mediacommons_fieldguide_import_d6_survey_response_list() {

  $type = 'survey_response';

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
  	   GROUP_CONCAT(DISTINCT cfri.field_resp_image_fid ORDER by cfri.delta SEPARATOR ', ') AS images
    FROM {node} n
      LEFT JOIN {node_revisions} nr
        ON n.vid = nr.vid
      LEFT JOIN {content_type_survey_response} ct
        ON n.vid = ct.vid
  	  LEFT JOIN {content_field_resp_image} cfri
        ON n.vid = cfri.vid

    WHERE n.type = :type
    AND n.status > 0
    GROUP BY n.nid
    ORDER BY n.nid";

  /** Create and return a database connection to the source (d6) database */
  return Database::getConnection('default', 'drupal6')->query($query, array(':type' => $type), array());

}

function mediacommons_fieldguide_import_d6_attached_images_by_fid ( $fid ) {

  $query = "SELECT fid, filepath, filename FROM {files} WHERE fid = :fid";

  /** Create and return a database connection to the source (d6) database */
  $file = Database::getConnection('default', 'drupal6')->query($query, array(':fid' => $fid), array())->fetchObject();

  return $file;

}

function mediacommons_fieldguide_import_d6_survey_question_list () {

  $type = 'survey_question';

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
    AND n.status > 0
    GROUP BY n.nid
    ORDER BY n.nid";

  /** Create and return a database connection to the source (d6) database */
  return Database::getConnection('default', 'drupal6')->query($query, array(':type' => $type), array());

}