<?php

/** http://api.drupal.org/api/drupal/includes%21theme.inc/function/template_preprocess_html/7 */
function mediacommons_base_process_html(&$vars) {
  $vars['classes'] = 'yui3-skin-sam yui-skin-sam ' . $vars['classes'];
}

/** See: http://api.drupal.org/api/drupal/includes%21theme.inc/function/template_process_page/7 */
function mediacommons_base_preprocess_page(&$vars) {
  /** Remove logo */
  $vars['logo'] = null;
}

/**
 * field.tpl.php
 * Default template implementation to display the value of a field.
 *
 * This file is not used and is here as a starting point for customization only.
 * @see theme_field()
 *
 * Available variables:
 * - $items: An array of field values. Use render() to output them.
 * - $label: The item label.
 * - $label_hidden: Whether the label display is set to 'hidden'.
 * - $classes: String of classes that can be used to style contextually through
 *   CSS. It can be manipulated through the variable $classes_array from
 *   preprocess functions. The default values can be one or more of the
 *   following:
 *   - field: The current template type, i.e., "theming hook".
 *   - field-name-[field_name]: The current field name. For example, if the
 *     field name is "field_description" it would result in
 *     "field-name-field-description".
 *   - field-type-[field_type]: The current field type. For example, if the
 *     field type is "text" it would result in "field-type-text".
 *   - field-label-[label_display]: The current label position. For example, if
 *     the label position is "above" it would result in "field-label-above".
 *
 * Other variables:
 * - $element['#object']: The entity to which the field is attached.
 * - $element['#view_mode']: View mode, e.g. 'full', 'teaser'...
 * - $element['#field_name']: The field name.
 * - $element['#field_type']: The field type.
 * - $element['#field_language']: The field language.
 * - $element['#field_translatable']: Whether the field is translatable or not.
 * - $element['#label_display']: Position of label display, inline, above, or
 *   hidden.
 * - $field_name_css: The css-compatible field name.
 * - $field_type_css: The css-compatible field type.
 * - $classes_array: Array of html class attribute values. It is flattened
 *   into a string within the variable $classes.
 *
 * @see template_preprocess_field()
 * @see theme_field()
 *
 * @ingroup themeable
 */
function mediacommons_base_process_field(&$vars) {
  $vars['safe_label'] = null;
  if (!$vars['label_hidden']) {
    $vars['safe_label'] = '<div class="field-label yui3-u-1" ' . $vars['title_attributes'] . '>' . $vars['label'] . '</div>';
  }
}

/**
 * @file
 * Default theme implementation to present all user profile data.
 *
 * This template is used when viewing a registered member's profile page,
 * e.g., example.com/user/123. 123 being the users ID.
 *
 * Use render($user_profile) to print all profile items, or print a subset
 * such as render($user_profile['user_picture']). Always call
 * render($user_profile) at the end in order to print all remaining items. If
 * the item is a category, it will contain all its profile items. By default,
 * $user_profile['summary'] is provided, which contains data on the user's
 * history. Other data can be included by modules. $user_profile['user_picture']
 * is available for showing the account picture.
 *
 * Available variables:
 *   - $user_profile: An array of profile items. Use render() to print them.
 *   - Field variables: for each field instance attached to the user a
 *     corresponding variable is defined; e.g., $account->field_example has a
 *     variable $field_example defined. When needing to access a field's raw
 *     values, developers/themers are strongly encouraged to use these
 *     variables. Otherwise they will have to explicitly specify the desired
 *     field language, e.g. $account->field_example['en'], thus overriding any
 *     language negotiation rule that was previously applied.
 *
 * @see user-profile-category.tpl.php
 *   Where the html is handled for the group.
 * @see user-profile-item.tpl.php
 *   Where the html is handled for each item in the group.
 * @see template_preprocess_user_profile()
 *
 * @ingroup themeable
 */

function mediacommons_base_process_user_profile(&$vars) {
  $vars['safe_user_profile'] = render($vars['user_profile']); 
}

function mediacommons_base_process_node(&$vars) {

  /** content array */
  $content = $vars['content'];

  $vars['classes_array'][] = 'yui3-cssbase';

  switch ($vars['type']) {

    case 'hub' :

      $vars['contributors'] = $vars['spokes'] = $vars['description'] = $vars['safe_tags'] = $vars['safe_tease'] = $vars['title_safe'] = null;

      switch ($vars['view_mode']) {

        case 'full' :

          $vars['theme_hook_suggestions'][] = 'node__hub__full';
          
          /** Remove the "Add new comment" if the comment form is being displayed on the same page. */
          if (!empty($content['comments']['comment_form'])) {
            unset($content['links']['comment']['#links']['comment-add']);
          }
          
          /** Display contributors. */
          $vars['contributors'] = render($content['field_contributors']);

          /** Display spokes. */
          $vars['spokes'] = render($content['field_spokes']);
          
          /** Display description. */
          $vars['description'] = render($content['field_description']);
          
          /** Display tags. */
          $vars['safe_tags'] = render($content['field_cluster_tags']);

          /** Display tags. */
          $vars['safe_tease'] = render($content['field_video_embed_link']);

          break;
      }
    
      break;

    case 'spoke' :
    
      $vars['links'] = $vars['authors'] = $vars['clusters'] = $vars['attached_images'] = $vars['comments'] = $vars['body'] = $vars['title_safe'] = null;
    
      switch ($vars['view_mode']) {
      
        case 'teaser' :
          
          //dpm($vars);
          //dpm($vars['content']['field_additional_authors']);
        
          $vars['theme_hook_suggestions'][] = 'node__spoke__teaser';
          
          $vars['title_safe'] = render($vars['title_prefix']) . '<h3 ' . $vars['title_attributes'] . '><a href="' . $vars['node_url'] . '">' . $vars['title'] .'</a></h3>' . render($vars['title_suffix']);
        
          /** Remove the "Add new comment" link on the teaser page */
          unset($vars['content']['links']['comment']['#links']['comment-add']);
      
          /** Display the wrapper div if there are links. */
          $vars['links'] = render($vars['content']['links']);
      
          /** Display the wrapper div if there are additional authors. */
          $vars['authors'] = render($vars['content']['field_additional_authors']);
      
          /** Display the wrapper div if there are clusters. */
          $vars['clusters'] = render($vars['content']['field_clusters']);
          
          /** Only display the representative image. */
          $vars['attached_images'] = render($vars['content']['field_attached_images']);
      
          /** Comments. */
          $vars['comments'] = render($vars['content']['comments']);
      
          /** Body. */
          $vars['body'] = render($vars['content']['field_document_textarea_body']);
      
          break;
          
        case 'full' :
        
          $vars['theme_hook_suggestions'][] = 'node__spoke__full';

          /** Remove the "Add new comment" if the comment form is being displayed on the same page. */
          if (!empty($vars['content']['comments']['comment_form'])) {
            unset($vars['content']['links']['comment']['#links']['comment-add']);
          }
          
          break;        
     
       }
       
     break;
   
   }
   
}

/*
 * Remove unnecessary white-space to improve DOM performance.
 * See: http://api.drupal.org/api/drupal/includes--theme.inc/function/theme_html_tag/7
 */
function mediacommons_base_html_tag($variables) {
  $element = $variables['element'];
  $attributes = isset($element['#attributes']) ? drupal_attributes($element['#attributes']) : '';
  if (!isset($element['#value'])) {
    return '<' . $element['#tag'] . $attributes . ' />';
  }
  else {
    $output = '<' . $element['#tag'] . $attributes . '>';
    if (isset($element['#value_prefix'])) {
      $output .= $element['#value_prefix'];
    }
    $output .= $element['#value'];
    if (isset($element['#value_suffix'])) {
      $output .= $element['#value_suffix'];
    }
    $output .= '</' . $element['#tag'] . '>';
    return $output;
  }
}