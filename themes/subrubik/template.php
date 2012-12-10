<?php

/** Preprocessor for theme('button') */
function subrubik_preprocess_button(&$vars) {
  
  if (isset($vars['element']['#name']) && $vars['element']['#name'] == 'field_contributed_piece_add_more') {
    $vars['element']['#value'] = t('Add another contributed piece');
  }
  
  if (isset($vars['element']['#name']) && $vars['element']['#name'] == 'field_contributors_add_more') {
    $vars['element']['#value'] = t('Add another contributor');
  }

  if (isset($vars['element']['#value'])) {

    $classes = array(
      t('Save') => 'yes',
      t('Submit') => 'yes',
      t('Add') => 'yes',
      t('Delete') => 'no',
      t('Cancel') => 'no',
    );
    
    foreach ($classes as $search => $class) {
      if (strpos($vars['element']['#value'], $search) !== FALSE) {
        $vars['element']['#attributes']['class'][] = 'button-' . $class;
        break;
      }
    }
    
  }
  
}

function subrubik_preprocess_field(&$variables, $hook) {
  die('here');
}