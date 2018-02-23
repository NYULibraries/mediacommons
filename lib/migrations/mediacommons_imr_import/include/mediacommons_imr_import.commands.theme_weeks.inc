<?php

/**
 * Migrate Theme Weeks.
 */
function migrate_theme_weeks () {
  foreach (theme_weeks() as $key => $old_node) migrate_theme_week($old_node);
}

/**
 * Migrate Theme Week.
 */
function migrate_theme_week ( $old_node ) {

  $node = array ( ) ;

  $node['title'] =  theme_weeks_title($old_node->title);

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

  if ( isset ( $old_node->body ) ) $node['field_body'] = $old_node->body ;

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

  $dates = theme_week_dates ( $old_node->title,  $old_node) ;

  /**
   * Subtract the dates from the title of the Theme Week
   * If $dates is set proceed, no good comes from setting dates with wrong values
   */
  if ( $dates ) {

  	if ( isset ( $dates['start'] ) && $start = strtotime ( $dates['start'] ) ) $node['field_period_1'] = $start ;

    if ( isset ( $dates['end'] ) && $end = strtotime ( $dates['end'] ) ) $node['field_period_2'] = $end ;

  }

  return create_hub ( $node );

}

/**
 * Title include Theme Week dates; remove the dates from the title
 */
function theme_weeks_title ( $title_source ) {

  $title_source = trim ( $title_source ) ;

  preg_match ('/\[(.*)\]/', $title_source, $match ) ;

  if ( isset ( $match[0] ) ) return str_replace ( $match[0] , '' , $title_source ) ;

  return trim($title_source);

}

/**
 * Crazy but the only realible source of dates is the title
 * of the Theme Week
 */
function theme_week_dates($date_source, $old_node) {  
  $listMonths = array(
    'January',
    'February',
    'March', 
    'April', 
    'May', 
    'June', 
    'July', 
    'August', 
    'September', 
    'October', 
    'November', 
    'December'
  );
  $created_year = date('Y', $old_node->created);
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
      if (isset($months[0][0])) $start_month = $months[0][0];      
      if (isset($months[0][1])) {
        $end_month = $months[0][1];
        if (!in_array($end_month, $listMonths)) {
          drush_log($end_month, 'ok');
          switch ($i) {
            case "Sept":
              $end_month = "September";
              break;
            case "Jan":
              $end_month = "January";
              break;
            case "th":
            case "rd":
            case "st":
            case "nd":
              $end_month = $start_month;
              break;
          }
        }
      }
      if ($end_month == NULL && !empty ($start_month)) $end_month = $start_month;
    }
    preg_match_all($days_pattern, str_replace($years, ' ', $match[1]), $days);    
    if (isset($days[0])) {
      if (isset($days[0][0])) $start_day = $days[0][0];
      if (isset($days[0][1])) $end_day = $days[0][1];
    }
    // https://jira.nyu.edu/jira/browse/MC-381
    if (empty($years)) {
      $year = $created_year;
    }
    else {
      $year = $years[0];
    }
    return array(
      'start' => $start_month . ' ' . $start_day . ', ' . $year,
      'end' => $end_month . ' ' . $end_day . ', ' . $year,
    );
  }
}
