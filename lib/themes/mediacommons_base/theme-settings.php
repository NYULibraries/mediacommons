<?php

function mediacommons_base_form_system_theme_settings_alter(&$form, &$form_state) {
   
   $form['mediacommons_base_settings']['mediacommons_base_toggle'] = array(
    '#type' => 'fieldset',
    '#title' => t('Toggle advanced elements'),
    '#description' => t('Enable or disable the display of certain page elements.'),
  );
  
  $form['mediacommons_base_settings']['mediacommons_base_toggle']['mediacommons_base_toggle_breadcrumb'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display the breadcrumb'),
    '#default_value' => theme_get_setting('mediacommons_base_toggle_breadcrumb'),
    '#description' => t('Breadcrumbs are disabled by default in MediaCommons Base theme. Enable the checkbox to show a trail of links from the homepage to the current page.'),  
    '#weight' => 1,
  );
  
   $form['mediacommons_base_settings']['mediacommons_base_toggle'] = array(
    '#type' => 'fieldset',
    '#title' => t('Toggle advanced elements'),
    '#description' => t('Enable or disable the display of certain page elements.'),
  );

}
