<?php

function mediacommons_imr_import_test() {
  drush_print('Running test command at ' . date('Y-m-j H:i:s', time()));
}

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
        $node['field_description'] = $hub->body;
      }      

      $node['field_type'] = 1;
      
      /** 
       * Representative image
       */
      if (isset($hub->field_theme_week_image_fid)) {
        $node['field_representative_image'] = $hub->field_theme_week_image_fid;
      }
      
      $node['field_pubstat'] = $hub->status;
      
      $node['field_cluster_tags'] = array();
      
      if (isset($hub->terms)) {
        foreach (explode(', ', $hub->terms) as $tid) {
          $node['field_cluster_tags'][] = $tid;
        }
      }
      
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
      
      $node['field_contributors'] = array();
      
      foreach ($node['field_spokes'] as $onid) {
        $node['field_contributors'][] = mediacommons_imr_import_d6_uid_by_node($onid);
      }
      
      $dates = mediacommons_imr_import_find_hub_dates($hub->title);

      /** 
       * Dates
       * http://drupal.stackexchange.com/questions/26948/programmatically-set-date-fields-in-drupal-7-date-date-iso-format-and-date
       */
      $node['field_field_period_1'] = date('Y-m-d H:i:s', strtotime($dates['start']));

      $node['field_field_period_2'] = date('Y-m-d H:i:s', strtotime($dates['end']));

      mediacommons_base_import_create_hub($node);

    }
  }
  
  drush_print(
    t('Done')
  );
}

/**
 * Migrate IMR post to spoke.
 */
function mediacommons_imr_import_migrate_posts() {

  /**
   * Load data, this will take time; go get yourself a coffee 
   */
  $data = mediacommons_imr_import_d6_post_list();

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
        // drush_print('Spoke "' . $spoke->title . '" already exist.');
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
        $node['field_attached_images_ri'] = $spoke->field_tease_image_fid;
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

function mediacommons_imr_import_extend_fields() {
  return array(
    'field_embed',
  );
}

/**
 * Migrate Theme Weeks into hubs.
 */
function mediacommons_imr_import_migrate_theme_weeks_old() {
  
  
  $settings = mediacommons_imr_import_settings();
  
  $production_url = $settings['settings']['production']['url'];

  $data = mediacommons_imr_import_d6_theme_week_list();
  
  /** Get a list of all the D6 users */
  /** $d6_users = mediacommons_imr_import_d6_users_list(); */
  
  /** If we have $data proceed */
  if ($data->rowCount()) {

    foreach ($data as $key => $value) {
    
      //drush_print_r($value);
      
      $old_author_uid = (int) $value->uid;
      
      /** Find out if user is already in the system */
      $author_exist = db_query('SELECT uid FROM {mediacommons_base_import_user_map} u WHERE ouid = :ouid', array(':ouid' => $old_author_uid));
      
      $authorCount = (int)$author_exist->rowCount();
      
      /** Find out if node is already in the system */
      $node_exist = db_query('SELECT nid FROM {mediacommons_base_import_node_map} u WHERE onid = :onid', array(':onid' => $value->nid));
      
      $rowCount = (int)$node_exist->rowCount();
      
      if (
          $rowCount == 0 &&
          $authorCount > 0 &&
          $old_author_uid > 0
      ) {
        
        drush_print('Importing ' . $value->title . "\n");        
        
        /** This node defualt configuration */
        $node = _mediacommons_imr_import_setup_node('hub');
        
        /** Node title */
        $node->title = $value->title;

        /** Node status */
        $node->status = $value->status;
        
        /** Node created and changed */
        $node->date = date("Y-m-j H:i:s", $value->created);

        /** Node comment */
        $node->comment = $value->comment;

        /** Node author record */
        $author_record = $author_exist->fetchObject();
        
        /** Node author new id */
        $node->uid = (int)$author_record->uid;
        
        $contributor_role_id = mediacommons_imr_import_get_role_by_name('contributor');
        
        /** Node author */
        $node->uid = (int)$author_record->uid;
        
        /** Load user object */
        $account = user_load(array('uid' => $node->uid));
        
        /** Save the user object with the new roles. */
        if ($account !== FALSE && !isset($account->roles[$contributor_role_id])) {
          $roles = $account->roles + array($contributor_role_id => 'contibutor');
          user_save($account, array('roles' => $roles));
        }
        
        /** Add description */
        $node->field_description[$node->language][0] = array(
          'value' => $value->body,
          'format' => 'full_html',
        );
        
        /** Media type */
        $node->field_type[$node->language][0]['value'] = 1;
      
        /** Hub representative image */
        if (!empty($value->field_theme_week_image_fid)) {
          $dupal_file_path = drupal_realpath('public://');
          $data = mediacommons_imr_import_find_image($value->field_theme_week_image_fid);
          $url_image_path = url($production_url . $data->filepath);
          $source_image_path = $dupal_file_path . '/' . $data->filename;
          file_put_contents($source_image_path, file_get_contents($url_image_path));
          $file = (object) array(
            'uid' => 1,
            'uri' => $source_image_path,
            'filemime' => file_get_mimetype($source_image_path),
            'status' => 1,
          );
          $file = file_copy($file, 'public://');
          $node->field_representative_image[$node->language][0] = (array)$file;
        }

        $contributors = array();
        $posts = array();        
        
        if (!empty($value->field_featured_post_1_nid)) {
          $posts[] = $value->field_featured_post_1_nid;
        }
        
        if (!empty($value->field_featured_post_2_nid)) {
          $posts[] = $value->field_featured_post_2_nid;
        }

        if (!empty($value->field_featured_post_3_nid)) {
          $posts[] = $value->field_featured_post_3_nid;
        }

        if (!empty($value->field_featured_post_4_nid)) {
          $posts[] = $value->field_featured_post_4_nid;
        }

        if (!empty($value->field_featured_page_or_feed_nid)) {
          $posts[] = $value->field_featured_page_or_feed_nid;
        }
    
        if (!empty($value->field_featured_post_5_nid)) {
          $posts[] = $value->field_featured_post_5_nid;
        }
        
        
        foreach ($posts as $key => $post) {
          $post_exist = db_query("SELECT nid FROM {mediacommons_base_import_node_map} WHERE onid = :onid", array(':onid' => $post));
          if ($post_exist->rowCount()) {
            $post_record = $post_exist->fetchObject();
            $c = db_query("SELECT uid FROM {node} WHERE nid = :nid", array(':nid' => $post_record->nid));
            $contributor_record = $c->fetchObject();
            $contributors[] = $contributor_record->uid;
            $node->field_spokes[LANGUAGE_NONE][$key]['nid'] = (int)$post_record->nid;
          }
        }

        foreach ($contributors as $key => $contributor) {
          $node->field_contributors[LANGUAGE_NONE][$key] = array(
            'uid' => $contributor,
            '_weight' => $key, 
          );        
        }        
        
        unset($contributors);
      
        /** dates */
        //$node->field_field_period[$node->language][0]['value'] = $value->field_period_value;
        //$node->field_field_period[$node->language][0]['value2'] = $value->field_period_value2;
      
        /** taxonomy */
        foreach (explode(', ', $value->terms) as $tid) {
          $tid_result = db_query("SELECT tid FROM {mediacommons_base_import_term_map} WHERE toid = :toid", array(':toid' => $tid));
          $tiCount = (int)$tid_result->rowCount();
          if ($tiCount > 0) {
            $term_object = $tid_result->fetchObject();
            $node->field_cluster_tags[$node->language][] = array('tid' => $term_object->tid);
          }
        }
        
        /** Prepare node for saving */
        if ($node = node_submit($node)) {
          node_save($node);
          db_insert('mediacommons_imr_import_node_map')
            ->fields(array(
              'onid' => $value->nid,
              'nid' => $node->nid,
            )
          )->execute();
          drush_print('Node saved. ' . url('node/' . $node->nid, array('absolute'=>TRUE)) . "\n");
        }
      }
      else {
        drush_print('Node already exist');
      }
    }
  }  
}

function mediacommons_imr_import_theme_weeks_title($title) {
  
  preg_match('/\[(.*)\]$/', $title, $match);
  
  if (isset($match[0])) {
    return str_replace($match[0], '', $title);
  }

  return $title;  
  
}

function mediacommons_imr_import_find_hub_dates($date_source) {
  
  $p = '/\[(.*)\]$/';
  $year_pattern = '/[0-9]{4}$/';
  $months_pattern = '/[a-zA-Z]+/';
  $days_pattern = '/[0-9]{1,2}/';
  
  $start_month = $end_month = $start_day = $end_day = NULL;

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