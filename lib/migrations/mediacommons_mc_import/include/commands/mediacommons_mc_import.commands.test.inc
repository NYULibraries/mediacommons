<?php

function mediacommons_mc_import_test() {

  mediacommons_mc_import_migrate_all_frontpage();
  
  return;

  drush_print('Loading frontpage nodes');
	
  foreach (mediacommons_mc_import_d6_frontpage_list() as $response) {
    drush_print_r($response);
    if (isset($response->field_fpp_image_data)) {
      drush_print_r(unserialize($response->field_fpp_image_data));
    }
  }

}