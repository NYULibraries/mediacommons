<?php

function setup_project_environment () {

  variable_set('mediacommons_project', 'mediacommons');

  module_enable(array('mediacommons_twitter'));

  variable_set('mediacommons_twitter_label', 'Twitter Widget');

  variable_set('mediacommons_twitter_url', 'https://twitter.com/lhenze/lists/mediacommons');

  drupal_flush_all_caches();

  drupal_cron_run();

}
