<?php

function mediacommons_imr_import_embed_field () {

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
    foreach ( $html->find('embed') as $element) {
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

function mediacommons_imr_import_add_allowed_iframe_sources () {

  $d6_domains = mediacommons_imr_import_find_iframe_sources () ;

  $result = db_query("SELECT * FROM {mediacommons_filter}");

  $domains = array();

  $cache = array();

  foreach ( $result as $record ) {
  	$domains[$record->did] = $record->domain;
  }

  foreach ( $d6_domains as $d6_domain ) {
  	if ( ! in_array( $d6_domain, $domains ) ) {
  	  drush_log ( 'Adding ' . $d6_domain . ' to allowed iframe source list' , 'status' );
      db_insert('mediacommons_filter')->fields( array('domain' => $d6_domain, ) )->execute();
    }
  }

}

function mediacommons_imr_import_find_iframe_sources () {

  include_once ( libraries_get_path('simplehtmldom') . '/simple_html_dom.php' );

  $type = 'post';

  $query = "
    SELECT
      n.nid,
      MAX(n.vid) AS vid,
  	  nr.body,
      ct.*
    FROM {node} n
    LEFT JOIN {node_revisions} nr ON n.vid = nr.vid
    LEFT JOIN {content_type_post} ct ON n.vid = ct.vid
    WHERE n.type = :type
    AND n.status > 0
    GROUP BY n.nid
    ORDER BY n.nid";

  /** Create and return a database connection to the source (d6) database */
  $nodes = Database::getConnection('default', 'drupal6')->query($query, array(':type' => $type), array());

  $hosts = array ( 'iframe' => array ( ) , 'embed' => array ( ) , 'img' =>  array ( ) , 'sources' ) ;

  foreach ( $nodes as $node ) {

  	$html = str_get_html( $node->field_video_embed_link_value . ' ' . $node->body ) ;
  	// Find all iframe
  	foreach ( $html->find('iframe') as $element) {
  	  $parse_url = parse_url( $element->src ) ;
  	  if ( ! in_array( $parse_url['host'], $hosts['iframe'], true ) ) {
  	  	$hosts['iframe'][] = $parse_url['host'];
  	  }
  	}
  	// Find all embed
  	foreach ( $html->find('embed') as $element ) {
      $parse_url = parse_url( $element->src ) ;
  	  if ( ! in_array( $parse_url['host'], $hosts['embed'], true ) ) {
  	  	$hosts['embed'][] = $parse_url['host'];
  	  }
    }
    // Find all img
    foreach ( $html->find('img') as $element ) {
  	  $hosts['img'][] = $element->src ;
    }
  }

  $hosts['sources'] = array_merge( $hosts['iframe'] , $hosts['embed'] ) ;

  return $hosts['sources'] ;

}

/**
 * Migrate Theme Weeks.
 */
function mediacommons_imr_import_migrate_theme_weeks() {

  /**
   * Load data, this will take time; go get yourself a coffee
   */
  $data = mediacommons_imr_import_d6_theme_week_list();

  /**
   * If we have $data proceed
   */
  if (isset($data)) {

    foreach ($data as $key => $hub) {

      /**
       * Prepare node
       */
      $node = array();

      $node['title'] =  mediacommons_imr_import_theme_weeks_title($hub->title);

      $node['ouid'] = $hub->uid;

      $node['onid'] = $hub->nid;

      $node['language'] = (!empty($hub->language)) ? $hub->language : LANGUAGE_NONE;

      $node['date'] = date('Y-m-j H:i:s', $hub->created);

      $node['status'] = $hub->status;

      $node['promote'] = $hub->promote;

      $node['moderate'] = $hub->moderate;

      $node['sticky'] = $hub->sticky;

      if (isset($hub->body)) {
        $node['field_body'] = $hub->body;
      }

      $node['field_type'] = 1;

      /** representative image */
      if ( isset( $hub->field_theme_week_image_fid ) ) {
        $node['field_representative_image'] = (object) array( 'fid' => $hub->field_theme_week_image_fid, );
      }

      $node['field_pubstat'] = $hub->status;

      $node['field_tags'] = array();

      if ( isset ( $hub->terms ) ) foreach ( explode(', ', $hub->terms ) as $tid ) $node['field_tags'][] = $tid;

      /*
       * Post in Theme Weeks are a set of fields, we assume n match week day
       * D7 will use index as week date
       */
      $node['field_spokes'] = array();

      if (!empty($hub->field_featured_post_1_nid)) {
        $node['field_spokes'][] = $hub->field_featured_post_1_nid;
      }

      if (!empty($hub->field_featured_post_2_nid)) {
        $node['field_spokes'][] = $hub->field_featured_post_2_nid;
      }

      if (!empty($hub->field_featured_post_3_nid)) {
        $node['field_spokes'][] = $hub->field_featured_post_3_nid;
      }

      if (!empty($hub->field_featured_post_4_nid)) {
        $node['field_spokes'][] = $hub->field_featured_post_4_nid;
      }

      if (!empty($hub->field_featured_post_5_nid)) {
        $node['field_spokes'][] = $hub->field_featured_post_5_nid;
      }

      if (!empty($hub->field_featured_page_or_feed_nid)) {
        $node['field_spokes'][] = $hub->field_featured_page_or_feed_nid;
      }

      /**
       * Get the D6 uid from the all the Post that belong to this Theme Week
       * and add it to the D7 contributors field
       */

      $node['field_contributors'] = array();

      foreach ( $node['field_spokes'] as $onid ) {
        $node['field_contributors'][] = mediacommons_imr_import_d6_uid_by_node($onid);
      }

      /**
       * Subtract the dates from the title of the Theme Week
       */
      $dates = mediacommons_imr_import_find_hub_dates($hub->title);

      /**
       * Dates
       * If $dates is set proceed, no good comes from setting dates with wrong values
       */
       if (
         isset($dates['start']) &&
         isset($dates['end'])
       ) {
        $node['field_period_1'] = strtotime($dates['start']);
        $node['field_period_2'] = strtotime($dates['end']);
      }
      mediacommons_base_import_create_hub($node);
    }
  }

}

/**
 * Migrate IMR post.
 */
function mediacommons_imr_import_migrate_posts() {

  /**
   * Load data, this will take time; go get yourself a coffee
   */
  $data = mediacommons_imr_import_d6_post_list();

  /** If we have $data proceed */
  if ( isset( $data ) ) {

    foreach ( $data as $key => $spoke ) {

      $path_alias = mediacommons_base_import_find_url_alias('node/' . $spoke->nid);

      /** find out if node is already in the system */
      $node_exist = mediacommons_base_import_node_exists($spoke->nid);

      /** find out if user is already in the system */
      $author_uid = mediacommons_base_import_user_exists($spoke->uid);

      /** conditions to be meet before creating content */
      if ( $node_exist ) {
        drush_log ( t( 'Spoke "@title" exist.', array('@title' => $spoke->title ) ) , 'warning' );
        continue;
      }

      if ( !$author_uid ) {
        drush_log ( t("Can't import spoke @title author does not exist in system. Please make sure the author is created before creating content under his username", array('@title' => $spoke->title ) ) , 'warning');
        continue;
      }

      /** prepare node */
      $node = array();

      $node['title'] = $spoke->title;

      $node['ouid'] = $spoke->uid;

      $node['onid'] = $spoke->nid;

      $node['language'] = (!empty($spoke->language)) ? $spoke->language : LANGUAGE_NONE;

      $node['date'] = date('Y-m-j H:i:s', $spoke->created);

      $node['status'] = $spoke->status;

      $node['promote'] = $spoke->promote;

      $node['moderate'] = $spoke->moderate;

      $node['sticky'] = $spoke->sticky;

      if ( isset ( $spoke->body ) ) $node['field_body'] = $spoke->body ;

      $node['field_pubstat'] = $spoke->status;

      foreach ( explode(', ', $spoke->terms ) as $tid )  $node['field_tags'][] = $tid;

      /** representative image */
      if ( isset ( $spoke->field_tease_image_fid ) ) $node['field_attached_images_ri'] = (object) array( 'fid' => $spoke->field_tease_image_fid ) ;

      /**
       * Add embed content
       * Extend field must be explicitly added to mediacommons_imr_import_extend_fields()
       */
      if (isset($spoke->field_video_embed_link_value)) {
        $node['field_embed'] = array(
          $node['language'] => array(
            array(
              'value' => $spoke->field_video_embed_link_value,
              'format' => 'limited_html',
            ),
          ),
        );
      }
      mediacommons_base_import_create_spoke($node);
    }
  }

}

/**
 * Callback to set any non-standard MC distribution field
 */
function mediacommons_imr_import_extend_fields() {
  return array(
    'field_embed',
  );
}

/**
 * Title include Theme Week dates; remove the dates from the title
 */
function mediacommons_imr_import_theme_weeks_title($title_source) {

  $title_source = trim($title_source);

  preg_match('/\[(.*)\]/', $title_source, $match);

  if (isset($match[0])) {
    return str_replace($match[0], '', $title_source);
  }

  return trim($title_source);

}

/**
 * Crazy but the only realible source of dates is the title
 * of the Theme Week
 */
function mediacommons_imr_import_find_hub_dates($date_source) {

  $start_month = $end_month = $start_day = $end_day = NULL;

  $date_source = trim($date_source);
  $p = '/\[(.*)\]/';
  $year_pattern = '/[0-9]{4}$/';
  $months_pattern = '/[a-zA-Z]+/';
  $days_pattern = '/[0-9]{1,2}/';

  preg_match($p, $date_source, $match);

  if (isset($match[1])) {
    preg_match($year_pattern, $match[1], $years);
    preg_match_all($months_pattern, $match[1], $months);

    if (isset($months[0])) {
      if (isset($months[0][0])) {
        $start_month = $months[0][0];
      }
      if (isset($months[0][1])) {
        $end_month = $months[0][1];
      }
      if ($end_month == NULL && !empty($start_month)) {
        $end_month = $start_month;
      }
    }
    preg_match_all($days_pattern, str_replace($years, ' ', $match[1]), $days);

    if (isset($days[0])) {
      if (isset($days[0][0])) {
        $start_day = $days[0][0];
      }
      if (isset($days[0][1])) {
        $end_day = $days[0][1];
      }
    }
    return array(
      'start' => $start_month . ' ' . $start_day . ', ' . $years[0],
      'end' => $end_month . ' ' . $end_day . ', ' . $years[0],
    );
  }
}