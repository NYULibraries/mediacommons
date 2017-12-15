<?php

function config_apachesolr() {

    drupal_flush_all_caches();

    $theme = 'mediacommons';

    $hash = 'altac';

    // exclude content types
    $blacklisted = array(
      'front_page_post',
      'message',
      'page',
    );

    $bundles = array_keys(node_type_get_types());

    $bundles_to_index = array_diff($bundles, $blacklisted);

    $env_id = 'solr';

    $entity_types = array('node', 'comment');

    $blocks = array(
        // 'im_field_co_editor', // Co-editor. Filter by field field_co_editor of type user_reference.
        // 'im_field_contributors', // Contributors. Filter by field field_contributors of type user_reference.
        // 'im_field_curators_editors', // Curated by. Filter by field field_curators_editors of type user_reference.
        'im_field_taxonomy',  // Tags. Filter by field field_taxonomy of type taxonomy_term_reference.
        // 'im_field_reviewer',  // Reviewer. Filter by field field_reviewer of type user_reference.
        'dm_field_pubdate', // Publication date. Filter by field field_pubdate of type datestamp.
        'hash',  // Site. Filter by Site (Multisite).
        'sm_vid_tags', // Tags. Filter by Tags (Multisite).
    );

    module_load_include('inc', 'apachesolr', 'apachesolr.index');

    variable_set('apachesolr_site_hash', $hash);

    variable_set('search_active_modules', array('apachesolr_search'));

    variable_set('search_default_module', 'apachesolr_search');

    // Make this Solr search environment multisite capable
    apachesolr_environment_variable_set('solr', 'multisitesearch', TRUE);

    foreach ($entity_types as $entity_type) {
        try {
            db_delete('apachesolr_index_bundles')->condition('env_id', $env_id)->condition('entity_type', $entity_type)->execute();
            $insert = db_insert('apachesolr_index_bundles')->fields(array('env_id', 'entity_type', 'bundle'));
            foreach ($bundles_to_index as $bundle) {
                if ($entity_type == 'comment') {
                    $bundle = 'comment_node_' . $bundle;
                }
                $insert->values(array('env_id' => $env_id, 'entity_type' => $entity_type, 'bundle' => $bundle));
            }
            $insert->execute();
        }
        catch (Exception $e) {
            throw $e;
        }
    }

    $info = facetapi_get_facet_info('apachesolr@solr');

    $realm = facetapi_realm_load('block');

    $adapter = facetapi_adapter_load('apachesolr@solr');

    // Make sure we have the blocks we want to enable
    foreach ($info as $facet_name => $facet) {
        if (in_array($facet_name, $blocks)) {
            facetapi_save_facet_enabled($adapter, $realm, $facet);
        }
    }

    cache_clear_all();

    drupal_flush_all_caches();

    $theme = 'mediacommons';

    // Fetch and sort blocks. We need to do this to init our block
    mediacommons_prepare_blocks($theme);

    drupal_theme_initialize();

    $region = 'searchresults_sidebar';

    $module = 'facetapi';

    $args = array('weight' => '-1', 'status' => '1',' region' => $region, 'theme' => $theme);

    ctools_css_flush_caches();

    // if we don't clear statics facetapi_get_block_info won't return the list of all facets we just enabled
    drupal_static_reset();

    foreach (facetapi_get_block_info(array('block')) as $block => $data) {
        _block_rehash($theme);
        try {
            db_update('block')->fields($args)->condition('module', $module)->condition('theme', $theme)->condition('delta', $block)->execute();
        }
        catch (Exception $e) {
            throw $e;
        }
  }

  $filter = array($hash => $hash);

  // Multisite capable, but search only this site
  apachesolr_environment_variable_set('solr', 'multisitesearch_sites', $filter);

  drupal_flush_all_caches();

}

function setup_project_environment () {
  variable_set('mediacommons_project', 'altac');
}

function enable_twitter_block() {

  drush_log('Task: Enable Twitter block', 'ok');

  $enabled = module_enable(array('mediacommons_twitter'));

  if ($enabled) {

    // https://jira.nyu.edu/jira/browse/MC-319

    $region = 'homepage_sidebar';

    $theme = 'mediacommons';

    $block = 'mediacommons_twitter';

    variable_set('mediacommons_twitter_label', 'Twitter Widget');

    //$twitter_url = '';

    //variable_set('mediacommons_twitter_url', $twitter_url);

    $twitter_user = 'mediacommons';

    variable_set('mediacommons_twitter_user', $twitter_user);

    $args = array(
      'weight' => '-1',
      'status' => '1',
      'region' => $region,
      'theme' => $theme,
    );

    drupal_theme_initialize();

    // Fetch and sort blocks. We need to do this to init our block
    $blocks = mediacommons_prepare_blocks($theme);

    $transaction = db_transaction();

    try {
      db_update('block')
         ->fields($args)
        ->condition('module', $block)
        ->condition('delta', $block)
        ->condition('theme', 'mediacommons')
        ->execute();
    }
    catch (Exception $e) {
      $transaction->rollback();
      print_r($e);
      throw $e;
    }
  }
}
