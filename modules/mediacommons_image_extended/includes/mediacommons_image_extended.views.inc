<?php

// http://www.kdweb.co.uk/blog/drupal-module-views-hook-sequence

/**
 * This hook is called right before the render process. The query has
 * been executed, and the pre_render() phase has already happened for
 * handlers, so all data should be available.
 *
 * Adding output to the view can be accomplished by placing text on
 * $view->attachment_before and $view->attachment_after. Altering the
 * content can be achieved by editing the items of $view->result.
 *
 * This hook can be utilized by themes.
 */
function mediacommons_image_extended_views_pre_render(&$view) {
  if ($view->name == 'image') {
    foreach ($view->result as $a => $result) {
      if (isset($result->field_field_image_extended)) {
        foreach ($result->field_field_image_extended as $b => $value) {
          if (!$value['raw']['extended_ri']) { 
            unset($view->result[$a]->field_field_image_extended[$b]); 
          }
        }
      }
    }
  }
}