<?php


function mediacommons_tne_import_d6_contributed_pieces_list() {
  
  $type = 'contributed_pieces';
  
  $query = "
    SELECT
      n.nid,
      MAX(n.vid) AS vid,
      n.type,
      n.language,
      nr.title,
      nr.body,
      nr.teaser,
      nr.format,
      n.uid,
      n.status,
      n.created,
      n.changed,
      n.comment,
      n.promote,
      n.moderate,
      n.sticky,
      ctcp.field_document_textarea_body_value,
      ctcp.field_tagline_value,
      ctcp.field_pubstat_value,
      GROUP_CONCAT(DISTINCT ctcp.field_thumbnail_fid SEPARATOR ', ') AS thumbnail_fid,
      GROUP_CONCAT(DISTINCT cfaa.field_additional_authors_uid) AS additonal_authors,
      GROUP_CONCAT(DISTINCT cfaa.delta) AS additonal_authors_delta,
      GROUP_CONCAT(DISTINCT cfcl.field_cluster_nid) AS cluster_nid,
      GROUP_CONCAT(DISTINCT cfcl.delta) AS cluster_delta,
      GROUP_CONCAT(DISTINCT t.tid SEPARATOR ', ') AS terms
    FROM tne_node n
      LEFT JOIN {node_revisions} nr
        ON n.vid = nr.vid
      LEFT JOIN {content_type_contributed_pieces} ctcp
        ON n.vid = ctcp.vid
      LEFT JOIN {content_field_additional_authors} cfaa
        ON n.vid = cfaa.vid
      LEFT JOIN {content_field_cluster} cfcl
        ON n.vid = cfcl.vid
      LEFT JOIN {term_node} t
        ON n.vid = t.vid
    WHERE n.type = 'contributed_pieces'
    AND n.status > 0
    GROUP BY n.nid
    ORDER BY n.nid";  
  
  /** Create and return a database connection to the source (d6) database */
  return Database::getConnection('default', 'drupal6')->query($query, array(':type' => $type), array());

}

function mediacommons_tne_import_d6_attached_images($nid) {

  $query = Database::getConnection('default', 'drupal6')
    ->select('image_attach', 'ia')
    ->fields('f', 
      array(
        'filepath',
        'filename',        
      )
    )
    ->condition('ia.nid', $nid, '=');
  
  $query->leftJoin('image', 'i', 'ia.iid = i.nid');
  
  $query->leftJoin('files', 'f', 'f.fid = i.fid');      

  return $query->execute();
  
}