<?php

function mediacommons_survey_import_test() {

  $data = mediacommons_survey_import_d6_survey_response_list();

  foreach ( $data as $node ) {
    drush_print_r($node);
  }

}
