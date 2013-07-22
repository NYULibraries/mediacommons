<?php

function mediacommons_tne_import_test() {

	// http://mediacommons.futureofthebook.org/tne/pieces/remixing-outline-middle-state-moment-revision
	drush_print('Loading contributed pieces (onid: 14732)');
	
  foreach (mediacommons_tne_import_d6_response_list() as $response) {
    drush_print_r($response);
  }

}