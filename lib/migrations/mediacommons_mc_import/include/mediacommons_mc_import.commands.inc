<?php

function config_apachesolr() {

  module_load_include('inc', 'apachesolr', 'apachesolr.index');

  $blacklisted = array(
    'front_page_post'
  );

  $bundles = array_keys(node_type_get_types());

  $bundles_to_index = array_diff($bundles, $blacklisted);

  $env_id = 'solr';

  $entity_types = array('node', 'comment');
  
  foreach ($entity_types as $entity_type) {
    $transaction = db_transaction();
    try {
      db_delete('apachesolr_index_bundles')
        ->condition('env_id', $env_id)
        ->condition('entity_type', $entity_type)
        ->execute();
      $insert = db_insert('apachesolr_index_bundles')
        ->fields(array('env_id', 'entity_type', 'bundle'));
      foreach ($bundles_to_index as $bundle) {
        if ($entity_type == 'comment') $bundle = 'comment_node_' . $bundle;  
        $insert->values(array(
          'env_id' => $env_id,
          'entity_type' => $entity_type,
          'bundle' => $bundle,
        ));
      
      }
      $insert->execute();
    }
    catch (Exception $e) {
      $transaction->rollback();
      throw $e;
    }
  }
  cache_clear_all();
}

