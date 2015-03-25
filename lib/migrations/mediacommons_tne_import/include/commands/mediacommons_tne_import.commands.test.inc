<?php

function mediacommons_tne_import_test() {
  // mediacommons_tne_import_hub_iframe_sources
  foreach (mediacommons_tne_import_d6_response_list() as $response) {
    drush_print_r($response);
  }

}