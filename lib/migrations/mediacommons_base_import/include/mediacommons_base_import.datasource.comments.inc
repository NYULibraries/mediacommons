<?php

function mediacommons_base_import_d6_node_comments_list ( ) {
  return Database::getConnection('default', 'drupal6')->query('SELECT * FROM {comments}', array ( ) , array ( ) ) ;
}
