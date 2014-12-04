<?php

/**
 * Test callback
 */
function mediacommons_intransition_import_test() {
  drush_print('Running test command at ' . date('Y-m-j H:i:s', time()));
}

/**
 * Echo 5 random Hubs and generate the link for prodcution site and dev
 */
function mediacommons_intransition_import_test_content() {

  global $settings;
  
  $environment_url = $settings['settings'][$settings['environment']]['url'];
  $production_url = $settings['settings']['production']['url'];
  
  $query = "SELECT 
    m.*
    FROM {mediacommons_base_import_node_map} m
    LEFT JOIN node n
      ON n.nid = m.nid
    WHERE 
      type = 'hub' 
    ORDER BY RAND() 
    LIMIT 5";
    
  $result = db_query($query, array(), array());
    
  foreach ($result as $record) {
    drush_print(
      url($environment_url . 'node/' . $record->nid, array('absolute' => TRUE))
    );
    drush_print(
      url($production_url . 'node/' . $record->onid, array('absolute' => TRUE))
    );
  }

}

/**
 * Migrate Theme Weeks.
 */
function mediacommons_intransition_import_migrate_theme_weeks() {
  
  /**
   * Load data, this will take time; go get yourself a coffee 
   */
  $data = mediacommons_intransition_import_d6_theme_week_list();

  /**
   * If we have $data proceed 
   */
  if (isset($data)) {

    foreach ($data as $key => $hub) {
        
      /**
       * Prepare node
       */
      $node = array();

      $node['title'] =  mediacommons_intransition_import_theme_weeks_title($hub->title);

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
      
      /** 
       * Representative image
       */
      if (isset($hub->field_theme_week_image_fid)) {
        $node['field_representative_image'] = (object) array( 'fid' => $hub->field_theme_week_image_fid, );
      }
      
      $node['field_pubstat'] = $hub->status;
      
      $node['field_tags'] = array();
      
      if (isset($hub->terms)) {
        foreach (explode(', ', $hub->terms) as $tid) {
          $node['field_tags'][] = $tid;
        }
      }
      
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
             
      foreach ($node['field_spokes'] as $onid) {
        $node['field_contributors'][] = mediacommons_intransition_import_d6_uid_by_node($onid);
      }
      
      /**
       * Subtract the dates from the title of the Theme Week
       */
      $dates = mediacommons_intransition_import_find_hub_dates($hub->title);

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
  
  drush_print( t('Done') );
}

/**
 * Migrate IMR post.
 */
function mediacommons_intransition_import_migrate_posts() {

  /**
   * Load data, this will take time; go get yourself a coffee 
   */
  $data = mediacommons_intransition_import_d6_post_list();

  /** 
   * If we have $data proceed 
   */
  if (isset($data)) {

    foreach ($data as $key => $spoke) {

      $path_alias = mediacommons_base_import_find_url_alias('node/' . $spoke->nid);

      /** 
       * Find out if node is already in the system 
       */
      $node_exist = mediacommons_base_import_node_exists($spoke->nid);

      /**
       * Find out if user is already in the system 
       */
      $author_uid = mediacommons_base_import_user_exists($spoke->uid);

      /**
       * Conditions to be meet before creating content
       */
      if ($node_exist) {
        drush_print(
          t(
            'Spoke "@title" already exist.', 
            array('@title' => $spoke->title)
          )
        );
        continue;
      }

      if (!$author_uid) {
        drush_print(
          t("Can't import spoke @title author does not exist in system. Please make sure the author is created before creating content under his username", array('@title' => $spoke->title))
        );
        continue;
      }
      
      /**
       * Prepare node
       */
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

      if (isset($spoke->body)) {
        $node['field_body'] = $spoke->body;
      }

      $node['field_pubstat'] = $spoke->status;
      
      foreach (explode(', ', $spoke->terms) as $tid) {
        $node['field_tags'][] = $tid;
      }
      
      /** 
       * Representative image
       */
      if (isset($spoke->field_tease_image_fid)) {
        $node['field_attached_images_ri'] = (object) array( 'fid' => $spoke->field_tease_image_fid, );
      }      
      
      /** 
       * Add embed content
       * Extend field must be explicitly added to mediacommons_imr_import_extend_fields()
       */
      if (isset($spoke->field_video_embed_link_value)) { 
        $node['field_embed'] = array(
          $node['language'] => array(
            array(
              'value' => $spoke->field_video_embed_link_value,
              'format' => 'full_html',  // make this be filtered_html?
            ),
          ),
        );
      }      
      mediacommons_base_import_create_spoke($node);
    }
  }

  drush_print(
    t('Done.')
  );

}

/**
 * Callback to set any non-standard MC distribution field
 */
function mediacommons_intransition_import_extend_fields() {
  return array(
    'field_embed',
  );
}

/**
 * Title include Theme Week dates; remove the dates from the title
 */
function mediacommons_intransition_import_theme_weeks_title($title_source) {

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
function mediacommons_intransition_import_find_hub_dates($date_source) {

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