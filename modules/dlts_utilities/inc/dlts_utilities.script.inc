<?php 

define('SCRIPT_LIBRARY', -100);
define('SCRIPT_DEFAULT', 0);
define('SCRIPT_THEME', 100);

function dlts_utilities_element_info() {
  $types['script'] = array(
    '#items' => array(), 
    '#pre_render' => array('dlts_utilities_pre_render_script'), 
    '#group_callback' => 'dlts_utilities_group_script',
    '#aggregate_callback' => 'dlts_utilities_aggregate_script',
  );
  return $types;
}

/*
 * Allow to including different types of <script>
 * See: http://api.drupal.org/api/drupal/core--includes--common.inc/function/drupal_add_js/8 
 */
function dlts_utilities_add_script($data = NULL, $options = NULL) {
  $script = &drupal_static(__FUNCTION__, array());

  // Construct the options, taking the defaults into consideration.
  if (isset($options)) {
    if (!is_array($options)) {
      $options = array('type' => $options);
    }
  }
  else {
    $options = array();
  }

  $options += dlts_utilities_script_defaults($data);

  /** Preprocess can only be set if caching is enabled. */
  $options['preprocess'] = $options['cache'] ? $options['preprocess'] : FALSE;

  /** Tweak the weight so that files of the same weight are included in the order of the calls to drupal_add_script(). */
  $options['weight'] += count($script) / 1000;

  if (isset($data)) {
    switch ($options['type']) {
      case 'setting':
        break;
      case 'inline':
        $script[] = $options;
        break;
      default:
        $script[$options['data']] = $options;
    }
  }
  return $script;
}

/**
 * Constructs an array of the defaults that are used for <script> items.
 *
 * @param $data
 *   (optional) The default data parameter for the <script> item array.
 * @see drupal_get_script()
 * @see drupal_add_script()
 */
function dlts_utilities_script_defaults($data = NULL) {
  return array(
    'type' => 'file',
    'group' => SCRIPT_DEFAULT,
    'every_page' => FALSE,
    'weight' => 0,
    'scope' => 'header',
    'cache' => TRUE,
    'defer' => FALSE,
    'preprocess' => TRUE,
    'version' => NULL,
    'data' => $data,
    'browsers' => array(),
    'script_type' => NULL,
  );
}

/*
 *
 * See: http://api.drupal.org/api/drupal/includes--common.inc/function/drupal_get_js/7
 * includes/common.inc, line 4079
 */
function dlts_utilities_get_script($scope = 'header', $script = NULL, $skip_alter = FALSE) {
  if (!isset($script)) {
    $script = dlts_utilities_add_script();
  }
  
  if (empty($script)) {
    return '';
  }

  // Allow modules to alter script.
  if (!$skip_alter) {
    drupal_alter('dlts_utilities_script', $script);
  }

  // Filter out elements of the given scope.
  $items = array();
  foreach ($script as $key => $item) {
    if ($item['scope'] == $scope) {
      $items[$key] = $item;
    }
  }

  // Sort the script so that it appears in the correct order.
  uasort($items, 'drupal_sort_css_js');
  
  // Render the HTML needed to load the script.
  $elements = array(
    '#type' => 'script',
    '#items' => $items,
  );

  return drupal_render($elements);
}

function dlts_utilities_aggregate_script(&$script_groups) {
  // Only aggregate when the site is configured to do so, and not during an
  // update.
  if (variable_get('preprocess_script', FALSE) && (!defined('MAINTENANCE_MODE') || MAINTENANCE_MODE != 'update')) {
    foreach ($script_groups as $key => $group) {
      if ($group['type'] == 'file' && $group['preprocess']) {
        $script_groups[$key]['data'] = dlts_utilities_build_script_cache($group['items']);
      }
    }
  }
}

function dlts_utilities_build_script_cache($files) {
  $contents = '';
  $uri = '';
  $map = variable_get('dlts_utilities_script_cache_files', array());
  $key = hash('sha256', serialize($files));
  if (isset($map[$key])) {
    $uri = $map[$key];
  }
  
  $script_type = 'text/x-template'; // for now

  if (empty($uri) || !file_exists($uri)) {
    // Build aggregate script file.
    foreach ($files as $path => $info) {
      if ($info['preprocess']) {
        // Append a ';' and a newline after each script file to prevent them from running together.
        $contents .= file_get_contents($path) . ";\n";
      }
    }
    // Prefix filename to prevent blocking by firewalls which reject files
    // starting with "ad*".
    $filename = 'script_' . drupal_hash_base64($contents) . '.' . $script_type;
    
    // Create the script/ within the files folder.
    $scriptpath = 'public://script';
    $uri = $script_path . '/' . $filename;
    // Create the script file.
    file_prepare_directory($script_path, FILE_CREATE_DIRECTORY);
    if (!file_exists($uri) && !file_unmanaged_save_data($contents, $uri, FILE_EXISTS_REPLACE)) {
      return FALSE;
    }
    // If script gzip compression is enabled, clean URLs are enabled (which means
    // that rewrite rules are working) and the zlib extension is available then
    // create a gzipped version of this file. This file is served conditionally
    // to browsers that accept gzip using .htaccess rules.
    if (variable_get('script_gzip_compression', TRUE) && variable_get('clean_url', 0) && extension_loaded('zlib')) {
      if (!file_exists($uri . '.gz') && !file_unmanaged_save_data(gzencode($contents, 9, FORCE_GZIP), $uri . '.gz', FILE_EXISTS_REPLACE)) {
        return FALSE;
      }
    }
    $map[$key] = $uri;
    variable_set('dlts_utilities_script_cache_files', $map);
  }
  return $uri;
}

/**
 * Default callback to group script items.
 *
 * This function arranges the script items that are in the #items property of the
 * script element into groups. Arranging the script items into groups serves two
 * purposes. When aggregation is enabled, files within a group are aggregated
 * into a single file, significantly improving page loading performance by
 * minimizing network traffic overhead.
 *
 * This function puts multiple items into the same group if they are groupable
 * and if they are for the same 'browsers'. Items of the 'file' type
 * are groupable if their 'preprocess' flag is TRUE, items of the 'inline' type
 * are never groupable, items of the 'settings' type are never groupable and
 * items of the 'external' type are never groupable.
 * This function also ensures that the process of grouping items does not change
 * their relative order. This requirement may result in multiple groups for the
 * same type and browsers, if needed to accommodate other items in
 * between.
 *
 * @param $scripts
 *   An array of script items, as returned by drupal_add_script(), but after
 *   alteration performed by drupal_get_script().
 *
 * @return
 *   An array of script groups. Each group contains the same keys (e.g., 
 *   'data', etc.) as a script item from the $scripts parameter, with the value of
 *   each key applying to the group as a whole. Each group also contains an
 *   'items' key, which is the subset of items from $scripts that are in the group.
 *
 * @see dlts_utilities_pre_render_scripts()
 */
function dlts_utilities_group_script($scripts) {
  $groups = array();
  // If a group can contain multiple items, we track the information that must
  // be the same for each item in the group, so that when we iterate the next
  // item, we can determine if it can be put into the current group, or if a
  // new group needs to be made for it.
  $current_group_keys = NULL;
  $index = -1;
  foreach ($scripts as $item) {
    ksort($item['browsers']);

    switch ($item['type']) {
      case 'file':
        // Group file items if their 'preprocess' flag is TRUE.
        // Help ensure maximum reuse of aggregate files by only grouping
        // together items that share the same 'group' value and 'every_page'
        // flag. See drupal_add_js() for details about that.
        $group_keys = $item['preprocess'] ? array($item['type'], $item['group'], $item['every_page'], $item['browsers']) : FALSE;
        break;
      case 'external':
      case 'setting':
      case 'inline':
        // Do not group external, settings, and inline items.
        $group_keys = FALSE;
        break;
    }

    // If the group keys don't match the most recent group we're working with,
    // then a new group must be made.
    if ($group_keys !== $current_group_keys) {
      $index++;
      // Initialize the new group with the same properties as the first item
      // being placed into it. The item's 'data' and 'weight' properties are
      // unique to the item and should not be carried over to the group.
      $groups[$index] = $item;
      unset($groups[$index]['data'], $groups[$index]['weight']);
      $groups[$index]['items'] = array();
      $current_group_keys = $group_keys ? $group_keys : NULL;
    }

    // Add the item to the current group.
    $groups[$index]['items'][] = $item;
  }
  return $groups;
}

function dlts_utilities_pre_render_script($elements) {
  // Group and aggregate the items.
  if (isset($elements['#group_callback'])) {
    $elements['#groups'] = $elements['#group_callback']($elements['#items']);
  }
  if (isset($elements['#aggregate_callback'])) {
    $elements['#aggregate_callback']($elements['#groups']);
  }

  // A dummy query-string is added to filenames, to gain control over
  // browser-caching. The string changes on every update or full cache
  // flush, forcing browsers to load a new copy of the files, as the
  // URL changed. Files that should not be cached (see drupal_add_js())
  // get REQUEST_TIME as query-string instead, to enforce reload on every
  // page request.
  $default_query_string = variable_get('script_css_query_string', '0');

  // For inline script to validate as XHTML, all script containing
  // XHTML needs to be wrapped in CDATA. To make that backwards compatible
  // with HTML 4, we need to comment out the CDATA-tag.
  
  // $embed_prefix = "\n<!--//--><![CDATA[//><!--\n";
  // $embed_suffix = "\n//--><!]]>\n";

  $embed_prefix = '';
  $embed_suffix = '';
  
  // Since sript may look for arguments in the URL and act on them, some
  // third-party code might require the use of a different query string.
  $script_version_string = variable_get('drupal_script_version_query_string', 'v=');

  // Defaults for each SCRIPT element.
  $element_defaults = array(
    '#type' => 'html_tag', 
    '#tag' => 'script', 
    '#value' => '',
    '#attributes' => array(),
  );

  // Loop through each group.
  foreach ($elements['#groups'] as $group) {
    // If a group of files has been aggregated into a single file,
    // $group['data'] contains the URI of the aggregate file. Add a single
    // script element for this file.
    if ($group['type'] == 'file' && isset($group['data'])) {
      $element = $element_defaults;
      $element['#attributes']['src'] = file_create_url($group['data']);
      $element['#browsers'] = $group['browsers'];
      $elements[] = $element;
    }
    // For non-file types, and non-aggregated files, add a script element per
    // item.
    else {
      foreach ($group['items'] as $item) {
        // Element properties that do not depend on item type.
        $element = $element_defaults;
        if (!empty($item['defer'])) {
          $element['#attributes']['defer'] = 'defer';
        }
        if (!empty($item['id'])) {
          $element['#attributes']['id'] = $item['id'];
        }
        if (!empty($item['script_type'])) {
          $element['#attributes']['type'] = $item['script_type'];
        }
        $element['#browsers'] = $item['browsers'];
        
        // Element properties that depend on item type.
        switch ($item['type']) {
          case 'inline':
            $element['#value_prefix'] = $embed_prefix;
            $element['#value'] = $item['data'];
            $element['#value_suffix'] = $embed_suffix;
            break;

          case 'file':
            $query_string = empty($item['version']) ? $default_query_string : $script_version_string . $item['version'];
            $query_string_separator = (strpos($item['data'], '?') !== FALSE) ? '&' : '?';
            $element['#attributes']['src'] = file_create_url($item['data']) . $query_string_separator . ($item['cache'] ? $query_string : REQUEST_TIME);
            break;

          case 'external':
            $element['#attributes']['src'] = $item['data'];
            break;
        }

        $elements[] = $element;
      }
    }
  }
  return $elements;
}

/*
 * Alter e.g.,
 */

function dlts_utilities_script_alter(&$script) {
}