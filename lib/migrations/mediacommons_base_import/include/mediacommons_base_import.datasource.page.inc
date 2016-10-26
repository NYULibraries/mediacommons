<?php

function pages () {
  $query = "
    SELECT
      n.nid,
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
  		nr.body,
  		nr.teaser
    FROM {node} n
      LEFT JOIN {node_revisions} nr
        ON n.vid = nr.vid
    WHERE n.type = :type
    GROUP BY n.nid
    ORDER BY n.nid";
  /** Create and return a database connection to the source (d6) database */
  return Database::getConnection('default', 'drupal6')->query($query, array(':type' => 'page'), array());
}
