<?php

function d6_post( $onid ) {

  $query = "
    SELECT
      n.nid,
      MAX(n.vid) AS vid,
      n.type,
      n.uid,
      n.status,
      n.created,
      n.changed,
      n.comment,
      n.promote,
      n.moderate,
      n.sticky,
      n.language, 
      n.tnid,
      n.translate,
      nr.title,
      nr.body,
      nr.teaser,
      nr.format,
      ctp.*,
      GROUP_CONCAT(DISTINCT t.tid SEPARATOR ', ') AS terms
    FROM {node} n
    LEFT JOIN {node_revisions} nr
      ON n.vid = nr.vid
    LEFT JOIN {content_type_post} ctp
      ON n.vid = ctp.vid
    LEFT JOIN {term_node} t
      ON n.vid = t.vid      
    WHERE n.type = :type
    AND n.nid = :onid 
    AND n.status > 0
    GROUP BY n.nid
    ORDER BY n.nid";
  
  
  /** Create and return a database connection to the source (d6) database */
  return Database::getConnection('default', 'drupal6')->query($query, array(':type' => 'post', ':onid' => $onid), array());

}

function posts() {
  
  $query = "
    SELECT
      n.nid,
      MAX(n.vid) AS vid,
      n.type,
      n.uid,
      n.status,
      n.created,
      n.changed,
      n.comment,
      n.promote,
      n.moderate,
      n.sticky,
      n.language, 
      n.tnid,
      n.translate,
      nr.title,
      nr.body,
      nr.teaser,
      nr.format,
      ctp.*,
      GROUP_CONCAT(DISTINCT t.tid SEPARATOR ', ') AS terms
    FROM {node} n
    LEFT JOIN {node_revisions} nr
      ON n.vid = nr.vid
    LEFT JOIN {content_type_post} ctp
      ON n.vid = ctp.vid
    LEFT JOIN {term_node} t
      ON n.vid = t.vid      
    WHERE n.type = :type 
    AND n.status > 0
    GROUP BY n.nid
    ORDER BY n.nid";
  
  
  /** Create and return a database connection to the source (d6) database */
  return Database::getConnection('default', 'drupal6')->query ( $query, array(':type' => 'post'), array() ) ;

}

function theme_weeks () {
  
  $query = "
    SELECT
      n.nid,
      MAX(n.vid) AS vid,
      n.type,
      n.uid,
      n.status,
      n.created,
      n.changed,
      n.comment,
      n.promote,
      n.moderate,
      n.sticky,
      n.language, 
      n.tnid,
      n.translate,
      nr.title,
      nr.body,
      nr.teaser,
      nr.format,
      cttwp.*,
      GROUP_CONCAT(DISTINCT t.tid SEPARATOR ', ') AS terms
    FROM {node} n
    LEFT JOIN {node_revisions} nr
      ON n.vid = nr.vid
    LEFT JOIN {content_type_theme_week_page} cttwp
      ON n.vid = cttwp.vid
    LEFT JOIN {term_node} t
      ON n.vid = t.vid      
    WHERE n.type = :type 
    AND n.status > 0
    GROUP BY n.nid
    ORDER BY n.nid";
  
  /** Create and return a database connection to the source (d6) database */
  return Database::getConnection('default', 'drupal6')->query($query, array(':type' => 'theme_week_page'), array());

}

function d6_theme_week ( $onid ) {
  
  $query = "
    SELECT
      n.nid,
      MAX(n.vid) AS vid,
      n.type,
      n.uid,
      n.status,
      n.created,
      n.changed,
      n.comment,
      n.promote,
      n.moderate,
      n.sticky,
      n.language, 
      n.tnid,
      n.translate,
      nr.title,
      nr.body,
      nr.teaser,
      nr.format,
      cttwp.*,
      GROUP_CONCAT(DISTINCT t.tid SEPARATOR ', ') AS terms
    FROM {node} n
    LEFT JOIN {node_revisions} nr
      ON n.vid = nr.vid
    LEFT JOIN {content_type_theme_week_page} cttwp
      ON n.vid = cttwp.vid
    LEFT JOIN {term_node} t
      ON n.vid = t.vid      
    WHERE n.nid = :onid";
  
  /** Create and return a database connection to the source (d6) database */
  return Database::getConnection('default', 'drupal6')->query($query, array(':onid' => $onid), array());

}
