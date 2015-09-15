<?php

/**
 * Migrate Theme Weeks.
 */
function migrate_theme_weeks () {
  foreach ( theme_weeks() as $key => $source ) migrate_theme_week ( $source ) ;
}

/**
 * Migrate Theme Week.
 */
function migrate_theme_week ( $old_node ) {

  /** Prepare node */
  $node = array() ;

  $node['title'] =  theme_weeks_title (  $old_node->title ) ;

  $node['ouid'] = $old_node->uid ;

  $node['onid'] = $old_node->nid ;

  $node['language'] = LANGUAGE_NONE ;

  $node['date'] = date ( 'Y-m-j H:i:s', $old_node->created ) ;

  $node['status'] = $old_node->status ;

  $node['promote'] = $old_node->promote ;

  $node['moderate'] = $old_node->moderate ;

  $node['sticky'] = $old_node->sticky ;

  $node['field_pubstat'] = $old_node->status ;

  $node['field_type'] = 1 ;

  $node['field_tags'] = array() ;

  $node['field_spokes'] = array() ;

  $node['field_contributors'] = array() ;

  /** representative image */
  if ( isset( $old_node->field_theme_week_image_fid ) ) $node['field_representative_image'] = (object) array ( 'fid' => $old_node->field_theme_week_image_fid  ) ;

  if ( isset ( $old_node->terms ) ) foreach ( explode(', ', $old_node->terms ) as $tid ) $node['field_tags'][] = $tid ;

  if ( isset ( $old_node->body ) ) {

  	extract_iframe_sources ( $old_node->body ) ;

  	$node['field_body'] = $old_node->body ;

  }

  /**
   * Post in Theme Weeks are a set of fields, we assume n match week day
   * D7 will use index as week date
   */
  if ( ! empty ( $old_node->field_featured_post_1_nid ) ) $node['field_spokes'][] = $old_node->field_featured_post_1_nid ;

  if ( ! empty ( $old_node->field_featured_post_2_nid ) ) $node['field_spokes'][] = $old_node->field_featured_post_2_nid ;

  if ( ! empty ( $old_node->field_featured_post_3_nid ) ) $node['field_spokes'][] = $old_node->field_featured_post_3_nid ;

  if ( ! empty ( $old_node->field_featured_post_4_nid ) ) $node['field_spokes'][] = $old_node->field_featured_post_4_nid ;

  if ( ! empty ( $old_node->field_featured_post_5_nid ) ) $node['field_spokes'][] = $old_node->field_featured_post_5_nid ;

  if ( ! empty ( $old_node->field_featured_page_or_feed_nid ) ) $node['field_spokes'][] = $old_node->field_featured_page_or_feed_nid ;

  /**
   * Get the D6 uid from the all the Post that belong to this Theme Week
   * and add it to the D7 contributors field
   */
  foreach ( $node['field_spokes'] as $onid ) $node['field_contributors'][] = ouid_by_onid ( $onid ) ;

  /**
   * Subtract the dates from the title of the Theme Week
   * If $dates is set proceed, no good comes from setting dates with wrong values
   */
  if (
      $dates = theme_week_dates ( $old_node->title ) &&
      isset ( $dates['start'] ) &&
      isset ( $dates['end'] )
  ) {

    $start = strtotime ( $dates['start'] ) ;

    $end = strtotime ( $dates['end'] ) ;

    if ( $start && $end  ) {

      $node['field_period_1'] = strtotime ( $dates['start'] ) ;

      $node['field_period_2'] = strtotime ( $dates['end'] ) ;

    }

  }

  return create_hub ( $node ) ;

}

/**
 * Title include Theme Week dates; remove the dates from the title
 */
function theme_weeks_title ( $title_source ) {

  $title_source = trim ( $title_source ) ;

  preg_match ('/\[(.*)\]/', $title_source, $match ) ;

  if ( isset ( $match[0] ) ) return str_replace ( $match[0] , '' , $title_source ) ;

  return trim ( $title_source ) ;

}

/**
 * Crazy but the only realible source of dates is the title
 * of the Theme Week
 */
function theme_week_dates ( $date_source ) {

  $start_month = $end_month = $start_day = $end_day = NULL ;

  $date_source = trim($date_source);

  $p = '/\[(.*)\]/';

  $year_pattern = '/[0-9]{4}$/';

  $months_pattern = '/[a-zA-Z]+/';

  $days_pattern = '/[0-9]{1,2}/';

  preg_match($p, $date_source, $match);

  if ( isset ( $match[1] ) ) {

    preg_match($year_pattern, $match[1], $years);

    preg_match_all($months_pattern, $match[1], $months);

    if ( isset ( $months[0] ) ) {
      if ( isset ( $months[0][0]) ) $start_month = $months[0][0] ;
      if ( isset ( $months[0][1]) ) $end_month = $months[0][1] ;
      if ( $end_month == NULL && ! empty ( $start_month ) ) $end_month = $start_month;
    }

    preg_match_all($days_pattern, str_replace($years, ' ', $match[1]), $days);

    if (isset($days[0])) {
      if ( isset( $days[0][0] ) ) $start_day = $days[0][0] ;
      if ( isset( $days[0][1] ) ) $end_day = $days[0][1] ;
    }

    return array (
      'start' => $start_month . ' ' . $start_day . ', ' . $years[0],
      'end' => $end_month . ' ' . $end_day . ', ' . $years[0],
    );
  }
}

function embed_field () {

  include_once ( libraries_get_path('simplehtmldom') . '/simple_html_dom.php' );

  $source = array ( ) ;

  foreach ( spokes () as $node ) {

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
