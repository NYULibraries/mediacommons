<?php

function mediacommons_altac_import_quickhack() {

	// http://mediacommons.futureofthebook.org/altac/pieces/remixing-outline-middle-state-moment-revision
	drush_print('Loading contributed pieces (onid: 14732)');
	
	$nodes = mediacommons_altac_import_d6_contributed_pieces(14732);
	
	foreach ($nodes as $node) {
	  mediacommons_altac_import_migrate_contributed_pieces(array( $node ));	
	}	

}