<?php

$base_file = __DIR__ . '/../mediacommons_base_import/' . 'mediacommons_base_import.module';

function mediacommons_altac_import_init() {

  $commands = array(
    
    //array(
    //  'label' => t('Import D6 Content (all the stepts)'),
    //  'callback' => array(),
    //),
    
    array(
      'label' => t('Migrate Contributed Pieces'),
      'callback' => array(
        'mediacommons_altac_import_migrate_all_contributed_pieces',
      ),
    ),
    array(
      'label' => t('Migrate Clusters'),
      'callback' => array(
        'mediacommons_altac_import_migrate_all_clusters'
      ),
    ),
    array(
      'label' => t('Migrate Responses'),
      'callback' => array(
        'mediacommons_altac_import_migrate_all_response'
      ),
    ),
    array(
      'label' => t('Delete Hubs, Spokes, alias, comments and file registry'),
      'callback' => array(
        'mediacommons_base_import_delete_post',
        'mediacommons_base_import_delete_hubs',
        'mediacommons_base_import_delete_comments',
        'mediacommons_base_import_delete_files',
        'mediacommons_base_import_delete_alias'
      ),
    ),
    
    //array(
    //  'label' => t('Run test'),
    //  'callback' => array(
    //    'mediacommons_altac_import_quickhack'
    //  ),
    //),
    
  );
  
  mediacommons_base_import_init(
    array('commands' => $commands)
  );
  
}

if (file_exists($base_file)) {
  include_once($base_file);
  mediacommons_altac_import_init();
}