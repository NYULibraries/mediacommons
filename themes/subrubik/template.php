<?php

/** Preprocessor for theme('button') */
function subrubik_preprocess_button(&$vars) {
  
  if (isset($vars['element']['#value']) && isset($vars['element']['#name'])) {
    
    switch ($vars['element']['#name']) {
      
      case 'field_contributed_piece_add_more' :
        $vars['element']['#value'] = t('Add another contributed piece');
        break;      

      case 'field_contributors_add_more' :
        $vars['element']['#value'] = t('Add another contributor');
        break;
    
    }

  }
  
}