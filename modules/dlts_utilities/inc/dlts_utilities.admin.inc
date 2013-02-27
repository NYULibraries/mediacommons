<?php

/**
 * Configure Site-specific metadata form
 */
function dlts_utilities_metadata_settings($form, &$form_state) {

  $form['dlts_utilities_metadata'] = array(
    '#type' => 'fieldset',
    '#title' => t('Site-specific metadata'),
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
  );
  
  $form['dlts_utilities_metadata']['dlts_utilities_metadata_collection_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Collection ID'),
    '#default_value' => dlts_utilities_collection(),
    '#size' => 20,
    '#maxlength' => 100,
    '#description' => t('The ID or short code of the NYU DLTS collection that this site represents.'),
  );
  
  $form['dlts_utilities_metadata']['dlts_utilities_metadata_partner_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Partner name'),
    '#default_value' => dlts_utilities_partner_name(),
    '#size' => 20,
    '#maxlength' => 100,
    '#description' => t('The name or title of the partner or other organizational entity who partnered with DLTS to create this website.'),
  );

  $form['dlts_utilities_metadata']['dlts_utilities_metadata_partner_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Partner code'),
    '#default_value' => dlts_utilities_partner_code(),
    '#size' => 20,
    '#maxlength' => 100,
    '#description' => t('The code DLTS uses to identify the repository or other organizational entity who partnered with DLTS to create this website.'),
  );

  $form['dlts_utilities_metadata']['dlts_utilities_metadata_collection_abstract'] = array(
    '#type' => 'textarea',
    '#title' => t('Collection abstract'),
    '#default_value' => dlts_utilities_collection_abstract(),
    '#description' => t('An abstract of the full description of the collection. This should be around 200 to 300 characters and should serve as a blurb on the home page and in the collection catalog.'),
  );
  
  $form['dlts_utilities_metadata']['dlts_utilities_metadata_collection_creator'] = array(
    '#type' => 'textfield',
    '#title' => t('Collection creator'),
    '#default_value' => dlts_utilities_collection_creator(),
    '#size' => 20,
    '#maxlength' => 100,
    '#description' => t('The name of the person or organization responsible for creating the intellectual content of this collection, if applicable.'),
  );

  $form['dlts_utilities_metadata']['dlts_utilities_metadata_collection_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Collection code'),
    '#default_value' => dlts_utilities_collection_code(),
    '#size' => 20,
    '#maxlength' => 100,
    '#description' => t('The code DLTS uses to identify this collection. Also, the key for referencing site-level metadata from the item in cross-collection search.'),
  );

  $form['dlts_utilities_metadata']['dlts_utilities_metadata_collection_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Collection title'),
    '#default_value' => dlts_utilities_collection_title(),
    '#size' => 20,
    '#maxlength' => 100,
    '#description' => t('The title of the collection. This may be identical to the title of the website.'),
  );

  $form['dlts_utilities_metadata']['dlts_utilities_metadata_collection_call_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Collection call number'),
    '#default_value' => dlts_utilities_collection_call_number(),
    '#size' => 20,
    '#maxlength' => 100,
    '#description' => t('If you are an archivist, please send us a nice description of this field to go here - dlts.pa@nyu.edu'),
  );

  $form['dlts_utilities_metadata']['dlts_utilities_metadata_collection_type'] = array(
    '#type' => 'textfield',
    '#title' => t('Collection type'),
    '#default_value' => dlts_utilities_collection_type(),
    '#size' => 20,
    '#maxlength' => 100,
    '#description' => t('The default resource type for items in this collection. Example "book", "photograph". Try to use a controlled vocabulary and to treat this like DC resource type.'),
  ); 

  $form['dlts_utilities_metadata']['dlts_utilities_metadata_collection_finding_aid_link'] = array(
    '#type' => 'textfield',
    '#title' => t('Finding aid link'),
    '#default_value' => dlts_utilities_collection_finding_aid_link(),
    '#size' => 40,
    '#maxlength' => 100,
    '#description' => t('If there is a finding aid for the collection represented by this website, you can enter the URL here and a link will appear on the site.'),
  );

  $form['dlts_utilities_metadata']['dlts_utilities_metadata_collection_bobcat_record'] = array(
      '#type' => 'textfield',
      '#title' => t('BobCat Record'),
      '#default_value' => dlts_utilities_collection_bobcat_record(),
      '#size' => 40,
      '#maxlength' => 100,
      '#description' => t('BobCat Record URL.'),
  );  
  
  return system_settings_form($form);
}