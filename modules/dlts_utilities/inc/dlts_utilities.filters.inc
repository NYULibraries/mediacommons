<?php
/**
 * @file
 * Module file for dlts_input_filters.
 */

/**
 * Filter out EAD tags and such
 */

/**
 * Implements hook_filter_info().
 */
function dlts_utilities_filter_info() {
  $filters['filter_foo'] = array(
    'title' => t('EAD Filter'),
    'description' => t('Replace EAD tags with appropriate HTML or nothing.'),
    'process callback'  => '_dlts_utilities_filter_ead_process',
    'default settings' => array(
      'dlts_input_filters_foo' => 'bar',
    ),
    'tips callback' => '_dlts_input_filters_filter_ead_tips',
  );

  return $filters;
}

/**
 * EAD filter process callback.
 *
 * Data derived from the Archivists Toolkit will generally have EAD markup. 
 * This needs to get converted into HTML.
 */
function _dlts_utilities_filter_ead_process($text, $filter, $format) {
  $text = preg_replace('!<lb/>!', '<br />', $text);
  $text = preg_replace('!<head>!', '<span class="ead-label">', $text);
  $text = preg_replace('!</head>!', '</span>', $text);
  return $text;
}

/**
 * @} End of "defgroup dlts_input_filters".
 */