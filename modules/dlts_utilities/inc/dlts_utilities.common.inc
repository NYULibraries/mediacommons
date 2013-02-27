<?php

/**
 * Package and send page callback as JSON/JSONP.
 * See: http://api.drupal.org/api/drupal/includes--common.inc/function/drupal_deliver_html_page/7
 *
 * @param $page_callback_result
 * A keyed array to be converted into JSON
 */
function dlts_utilities_deliver_json($page_callback_result) {  

  /** Send appropriate HTTP-Header for browsers and search engines. */
  global $language;
  drupal_add_http_header('Content-Language', $language->language);

  /** Menu status constants are integers; page content is a string or array. */ 
  if (is_int($page_callback_result)) {
    switch ($page_callback_result) {

      /*
       * Send a 404 response.
       */
      case MENU_NOT_FOUND:
        drupal_add_http_header('Status', 'HTTP/1.1 404 Not Found');
        watchdog('Request not found', check_plain($_GET['q']), NULL, WATCHDOG_WARNING);        
        
        $page_callback_result = array(
          'msg' => t('404 Not Found'),
          'code' => 404,
        );        
        break;
        
      /*
       * Send a 403 response.
       */        
      case MENU_ACCESS_DENIED:
        drupal_add_http_header('Status', 'HTTP/1.1 403 Forbidden');
        watchdog('dlts_utilities', check_plain($_GET['q']), NULL, WATCHDOG_WARNING);
        
        $page_callback_result = array(
          'msg' => t('403 Forbidden'),
          'code' => 403,
        );                
        break;
        
      /*
       * Send a 503 response.
       */        
      case MENU_SITE_OFFLINE:
        drupal_add_http_header('Status', 'HTTP/1.1 503 Service unavailable');
        $page_callback_result = array(
          'msg' => t('503 Service unavailable.') . filter_xss_admin(variable_get('site_offline_message', t(' @site is currently under maintenance. We should be back shortly. Thank you for your patience.', array('@site' => variable_get('site_name', 'Drupal'))))),
          'code' => 503,
        );
        break;
    }
  }

  // Only add Content-Length if we actually have any content. Some non-standard
  // servers get confused by Content-Length in at least HEAD/GET requests
  // $content_length = strlen($data);
  // if ($content_length > 0 || $method == 'POST' || $method == 'PUT') {
  //  $defaults['Content-Length'] = 'Content-Length: '. $content_length;
  // }
  
  if (isset($page_callback_result)) {
    if (array_key_exists('callback', $_GET)) {
      drupal_add_http_header('Content-Type', 'text/javascript; charset=utf-8');
      echo sprintf('%s(%s);', filter_xss($_GET['callback'], array()), drupal_json_encode($page_callback_result));
    } else {
      drupal_add_http_header('Content-Type', 'application/json; charset=utf-8');
      echo drupal_json_encode($page_callback_result);
    }
  }
}

/**
 * Returns the value of the field first item
 */
function dlts_utilities_field_get_first_item($entity_type, $entity, $field_name, $index = 0, $langcode = NULL) {
  $field = field_get_items($entity_type, $entity, $field_name, $langcode);
  return $field[$index];
}

/**
 * Returns a site-specific hash.
 * @Deprecated: Use dlts_utilities_unique_hash()
 */
function dlts_utilities_hash($name = '') {
  return dlts_utilities_unique_hash($name);
}

/**
 * Returns a unique hash.
 */
function dlts_utilities_unique_hash($name = '') {
  global $base_url;
  return substr(base_convert(sha1(uniqid($base_url . $name, TRUE)), 16, 36), 0, 6);
}

/** 
 * Returns a list of node ids of nodes that matche a field_label / field_value pair
 * 
 */
function dlts_utilities_get_nid_by_field($field_label, $field_value) {
	$query = new EntityFieldQuery();
	$query->entityCondition('entity_type', 'node', '=')->fieldCondition($field_label, 'value', $field_value, '=');
	$result = $query->execute();
	$nodeids = array();
	if ( array_key_exists('node', $result) && count( $result['node'] > 0 ) ){
		foreach ($result['node'] as $node) {
			$nodeids[] = $node->nid;
		}
	} else {
		watchdog('dlts_utilities', 'No results when trying to look up nodes by field value');
	}
	return $nodeids;
}	

function dlts_utilities_is_pjax() {
  $is_pjax = &drupal_static(__FUNCTION__);
  if (!isset($is_pjax) && function_exists('getallheaders')) {
    $headers = getallheaders();
    if (isset($headers['X-Pjax'])) {
      $is_pjax = TRUE;
    }
    else {
      $params = drupal_get_query_parameters();
      if (isset($params['pjax']) && $params['pjax'] == 1) {
        $is_pjax = TRUE;
      }
      else {
        $is_pjax = FALSE;
      }
    }
  }
  return $is_pjax;
}

/** Simple iOS and others mobile detection @TODO: Find a better solution. */
function dlts_utilities_mobile_detection() {
  if ( preg_match( "/iPod|iPhone|iPad|webOS|Android/i", $_SERVER['HTTP_USER_AGENT'], $matches ) ) {
    $device = strtolower($matches[0]);
    switch ( $device ) {
      case 'ipod' :
      case 'iphone' :
      case 'ipad' :
        return array(
          'os' => 'ios',
          'device' => $device
        );
        break;
      case 'android':
        return array(
          'os' => 'android',
          'device' => NULL
        );
        break;
      case 'webos':
        return array(
          'os' => 'webos',
          'device' => NULL
        );
        break;
      default:
        return FALSE;
    }
  } else {
    return FALSE;
  }
}

function dlts_utilities_browser_info($agent = null) {

	$known = array('msie', 'firefox', 'safari', 'webkit', 'opera', 'netscape', 'konqueror', 'gecko');

	// Clean up agent and build regex that matches phrases for known browsers
	// (e.g. "Firefox/2.0" or "MSIE 6.0" (This only matches the major and minor
	// version numbers.  E.g. "2.0.0.6" is parsed as simply "2.0"

	$agent = strtolower($agent ? $agent : $_SERVER['HTTP_USER_AGENT']);
	
	$pattern = '#(?<browser>' . join('|', $known) . ')[/ ]+(?<version>[0-9]+(?:\.[0-9]+)?)#';

	// Find all phrases (or return empty array if none found)
	if (!preg_match_all($pattern, $agent, $matches)) return array();

	// Since some UAs have more than one phrase (e.g Firefox has a Gecko phrase,
	// Opera 7,8 have a MSIE phrase), use the last one found (the right-most one
	// in the UA).  That's usually the most correct.
	$i = count($matches['browser']) - 1;
	
	return array($matches['browser'][$i] => $matches['version'][$i]);
	
}