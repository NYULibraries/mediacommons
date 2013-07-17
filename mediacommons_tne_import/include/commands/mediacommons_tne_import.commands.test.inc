<?php

function mediacommons_tne_import_quickhack() {

	// http://mediacommons.futureofthebook.org/tne/pieces/remixing-outline-middle-state-moment-revision
	drush_print('Loading contributed pieces (onid: 14732)');
	
	$nodes = mediacommons_tne_import_d6_contributed_pieces(14732);
	
	foreach ($nodes as $node) {
	  mediacommons_tne_import_migrate_contributed_pieces(array( $node ));	
	}	

}