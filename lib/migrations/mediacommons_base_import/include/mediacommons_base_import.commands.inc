<?php

function mediacommons_base_import_add_contributed_pieces_allowed_iframe_sources () {

	$d6_domains = mediacommons_base_import_contributed_pieces_find_iframe_sources () ;

	$result = db_query("SELECT * FROM {mediacommons_filter}");

	$domains = array();

	foreach ( $result as $record ) $domains[$record->did] = $record->domain ;

	foreach ( $d6_domains as $d6_domain ) {
		if ( ! in_array( $d6_domain, $domains ) ) {
			drush_log ( 'Adding ' . $d6_domain . ' to allowed iframe source list' , 'status' );
			db_insert('mediacommons_filter')->fields( array('domain' => $d6_domain, ) )->execute();
		}
	}

}

function mediacommons_base_import_add_allowed_iframe_sources () {

  $d6_domains = mediacommons_base_import_find_iframe_sources () ;

  $result = db_query("SELECT * FROM {mediacommons_filter}");

  $cache = $domains = array();

  foreach ( $result as $record ) $domains[$record->did] = $record->domain ;

  foreach ( $d6_domains as $d6_domain ) {
    if ( ! in_array( $d6_domain, $domains ) ) {
      drush_log ( 'Adding ' . $d6_domain . ' to allowed iframe source list' , 'status' );
      db_insert('mediacommons_filter')->fields( array('domain' => $d6_domain, ) )->execute();
    }
  }

}

function mediacommons_base_import_spoke_add_allowed_iframe_sources () {

  $d6_domains = mediacommons_base_import_contributed_pieces_find_iframe_sources () ;

  $result = db_query("SELECT * FROM {mediacommons_filter}");

  $cache = $domains = array();

  foreach ( $result as $record ) $domains[$record->did] = $record->domain ;

  foreach ( $d6_domains as $d6_domain ) {
    if ( ! in_array( $d6_domain, $domains ) ) {
      drush_log ( 'Adding ' . $d6_domain . ' to allowed iframe source list' , 'status' );
      db_insert('mediacommons_filter')->fields( array('domain' => $d6_domain, ) )->execute();
    }
  }

}

function mediacommons_base_import_embed_field () {

	include_once ( libraries_get_path('simplehtmldom') . '/simple_html_dom.php' );

	$source = array ( ) ;

	$nodes = mediacommons_base_import_d7_spoke_list () ;

	foreach ( $nodes as $node ) {

		$alter = FALSE;

		/** load node */
		$entity = node_load ( $node->nid ) ;

		$elemets = array ( 'nid' => $node->nid , 'object' => array ( ) , 'embed' => array ( ) , 'div' =>  array ( ), 'source' =>  '' ) ;

		/** Wrap it with Entity API */
		$ewrapper = entity_metadata_wrapper('node', $entity);

		$embed = $ewrapper->field_embed->value();

		$html = str_get_html( $embed['value'] ) ;

		// Find all object
		foreach ( $html->find('object') as $element) {
			foreach ( $html->find('embed') as $sub_element ) {
				if ( ! in_array ( $sub_element->src, $elemets['object'] ) ) {
					$alter = TRUE;
					$elemets['object'][] = $sub_element->src;
				}
			}
		}

		// Find all embed
		foreach ( $html->find('embed') as $element ) {
			if ( ! in_array ( $element->src, $elemets['object'] ) ) {
				$alter = TRUE;
				$elemets['embed'][] = $element->src;
			}
		}

		// Find all div
		if ( empty ( $elemets['embed'] ) && empty ( $elemets['object'] ) ) {
			foreach ( $html->find('div') as $element ) {
				foreach ( $element->find('param[name=movie]') as $sub_element ) {
					$alter = TRUE;
					$elemets['div'][] = $sub_element->value ;
				}
			}
		}

		if ( $alter ) {

			$elemets['source'] = $embed['value'];

			$source[] = $elemets ;

		}

	}

	foreach ( $source as $node_data ) {

		$html = '';

		$srcs = array_unique ( array_merge( $node_data['object'], $node_data['embed'], $node_data['div'] ) );

		foreach ( $srcs as $src ) {
			if ( $src ) {
				$html .= theme('html_tag', array( 'element' => array( '#tag' => 'iframe', '#attributes' => array( 'class' => array( 'altered' ), 'src' => $src, 'data-onid' => $node_data['nid'], 'data-onid' => $node_data['nid'] ) ) ) ) ;
			}
		}

		/** load node */
		$entity = node_load ( $node_data['nid'] ) ;

		/** Wrap it with Entity API */
		$ewrapper = entity_metadata_wrapper('node', $entity);

		$ewrapper->field_embed->set( array ( 'value' => $html ) );

		drush_log ( url('node/' . $entity->nid , array( 'absolute'=> TRUE ) ) , 'status' ) ;

		$ewrapper->save();

	}

}

function mediacommons_base_import_body_field () {

	include_once ( libraries_get_path('simplehtmldom') . '/simple_html_dom.php' );

	$source = array ( ) ;

	$nodes = mediacommons_base_import_d7_spoke_list () ;

	foreach ( $nodes as $node ) {

		$alter = FALSE;

		/** load node */
		$entity = node_load ( $node->nid ) ;

		$elemets = array ( 'nid' => $node->nid , 'object' => array ( ) , 'embed' => array ( ) , 'div' =>  array ( ), 'source' =>  '' ) ;

		/** Wrap it with Entity API */
		$ewrapper = entity_metadata_wrapper('node', $entity);

		$embed = $ewrapper->field_body->value();


		continue ;

		$html = str_get_html( $embed['value'] ) ;

		// Find all object
		foreach ( $html->find('object') as $element) {
			foreach ( $html->find('embed') as $sub_element ) {
				if ( ! in_array ( $sub_element->src, $elemets['object'] ) ) {
					$alter = TRUE;
					$elemets['object'][] = $sub_element->src;
				}
			}
		}

		// Find all embed
		foreach ( $html->find('embed') as $element ) {
			if ( ! in_array ( $element->src, $elemets['object'] ) ) {
				$alter = TRUE;
				$elemets['embed'][] = $element->src;
			}
		}

		// Find all div
		if ( empty ( $elemets['embed'] ) && empty ( $elemets['object'] ) ) {
			foreach ( $html->find('div') as $element ) {
				foreach ( $element->find('param[name=movie]') as $sub_element ) {
					$alter = TRUE;
					$elemets['div'][] = $sub_element->value ;
				}
			}
		}

		if ( $alter ) {

			$elemets['source'] = $embed['value'];

			$source[] = $elemets ;

		}

	}

	foreach ( $source as $node_data ) {

		$html = '';

		$srcs = array_unique ( array_merge( $node_data['object'], $node_data['embed'], $node_data['div'] ) );

		foreach ( $srcs as $src ) {
			if ( $src ) {
				$html .= theme('html_tag', array( 'element' => array( '#tag' => 'iframe', '#attributes' => array( 'class' => array( 'altered' ), 'src' => $src, 'data-onid' => $node_data['nid'], 'data-onid' => $node_data['nid'] ) ) ) ) ;
			}
		}

		/** load node */
		$entity = node_load ( $node_data['nid'] ) ;

		/** Wrap it with Entity API */
		$ewrapper = entity_metadata_wrapper('node', $entity);

		$ewrapper->field_body->set( array ( 'value' => $html ) );

		drush_log ( url('node/' . $entity->nid , array( 'absolute'=> TRUE ) ) , 'status' ) ;

		$ewrapper->save();

	}

}
