<?php

function _mediacommons_editorialworkflow_nodeformscols_hub_field_placements() {
  return array(

    'title' => array(
      'region' => 'main',
      'weight' => 0,
      'has_required' => 1,
      'title' => t('Title'),
    ),
    'additional_settings' => array(
      'region' => 'main',
      'weight' => 8,
      'has_required' => 0,
      'title' => t('Vertical tabs'),
      'hidden' => 0
    ), 
    'actions' => array(
      'region' => 'main',
      'weight' => 9,
      'has_required' => 0,
      'title' => t('Save'),
      'hidden' => 0,
     ),

    'field_representative_image' => array(
      'region' => 'main',
      'weight' => 4,
      'has_required' => 0,
      'title' => t('Representative Image'),
      'hidden' => 0,
    ),		
    
    'field_video_embed_link' => array(
      'region' => 'main',
      'weight' => 3,
      'has_required' => 0,
      'title' => t('Video embed link'),
      'hidden' => 0,
    ),		
    'field_type' => array(
      'region' => 'main',
      'weight' => 2,
      'has_required' => 1,
      'title' => t('Choose type'),
    ),
    
    'field_description' => array(
      'region' => 'main',
      'weight' => 1,
      'has_required' => 0,
      'title' => t('Description'),
      'hidden' => 0,
    ),		
    
    'field_field_period' => array(
      'region' => 'right',
      'weight' => 0,
      'has_required' => 0,
      'title' => t('Start and end dates'),
      'hidden' => 0,
    ),
    
    'field_spokes' => array(
      'region' => 'right',
      'weight' => 1,
      'has_required' => 0,
      'title' => t('Contributed Pieces'),
      'hidden' => 0,
    ),
    
    'field_co_editor' => array(
      'region' => 'right',
      'weight' => 2,
      'has_required' => 0,
      'title' => t('Co-editor'),
      'hidden' => 0,
    ),
    
    'field_contributors' => array(
      'region' => 'right',
      'weight' => 3,
      'has_required' => 0,
      'title' => t('Contributors'),
      'hidden' => 0,
    ),

    'field_cluster_tags' => array(
      'region' => 'right',
      'weight' => 4,
      'has_required' => 0,
      'title' => t('Tags'),
      'hidden' => 0,
    ),
    
  );
}

function _mediacommons_editorialworkflow_nodeformscols_spoke_field_placements() {
	return array(
    'title' => array(
      'region' => 'main',
      'weight' => 0,
      'has_required' => 1,
      'title' => t('Title'),
    ),
    'additional_settings' => array(
      'region' => 'main',
      'weight' => 5,
      'has_required' => 0,
      'title' => t('Vertical tabs'),
      'hidden' => 0,
    ),
    'actions' => array(
      'region' => 'main',
      'weight' => 6,
      'has_required' => 0,
      'title' => t('Save'),
      'hidden' => 0,
    ),
    'field_attached_images' => array(
      'region' => 'right',
      'weight' => 2,
      'has_required' => 0,
      'title' => t('Attached Images'),
      'hidden' => 0,
    ),
    'field_body' => array(
      'region' => 'main',
      'weight' => 1,
      'has_required' => 0, 
      'title' => t('Body'),
      'hidden' => 0,
    ),
    'field_part_of_hub' => array(
      'region' => 'right',
      'weight' => 0,
      'has_required' => 0,
      'title' => t('Part of'),
      'hidden' => 0,
    ),
    'field_pubstat' => array(
      'region' => 'right',
      'weight' => 3,
      'has_required' => 1,
      'title' => t('Publication Status'),
    ),
    'field_contributors' => array(
      'region' => 'right',
      'weight' => 1,
      'has_required' => 0,
      'title' => t('Contributors'),
      'hidden' => 0,
    ),
  );
}