<?php

/*
 * Main
 * Usually empty. Add any site specific routine here
 */
 
/** Kill 'em all */
function mediacommons_mc_import_delete_content($ype = 'all') {

  $nodes = array();

  $nodes += mediacommons_base_import_find_nodes('front_page_post');
  
  foreach ($nodes as $node) {
    node_delete($node->nid);
  }

  db_delete('mediacommons_base_import_node_map')
    ->condition('nid', 'kill em all', '<>')
    ->execute();

  db_delete('mediacommons_base_import_term_map')
    ->condition('tid', 'kill em all', '<>')
    ->execute();  
  
  db_delete('taxonomy_term_data')
    ->condition('tid', 'kill em all', '<>')
    ->execute();

  db_delete('taxonomy_term_hierarchy')
    ->condition('tid', 'kill em all', '<>')
    ->execute();
    
  db_delete('taxonomy_term_hierarchy')
    ->condition('tid', 'kill em all', '<>')
    ->execute();
    
  db_delete('mediacommons_base_import_vocabulary_map')
    ->condition('vid', 'kill em all', '<>')
    ->execute();

  db_delete('taxonomy_vocabulary')
    ->condition('vid', 'kill em all', '<>')
    ->execute();

}
 