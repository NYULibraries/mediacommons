<?php


function mediacommons_editorialworkflow_contribute_page() {

  global $user;

  $output = NULL;
   
  $pattern = $replacement = $items = array(); 
  
  $types = node_type_get_types();
  
  $output .= theme('html_tag', array(
    'element' => array(
      '#tag' => 'h2',
      '#attributes' => array(
        'class' => 'title',
        ),
      '#value' => t('Create New Content'),
    ),
  ));

  foreach( (array)$types as $type ) {   
    if (user_access('create ' . $type->type . ' content')) {
      
      $type_url_str = str_replace('_', '-', $type->type);
      
      switch ($type->type) {
      
        case 'cluster' :
          $type->name = t('Create a New Cluster');
          break;      

        case 'contributed_pieces' :
          $type->name = t('Contribute a New Piece');
          break;

        case 'response' :
          $type->name = t("Respond to Cluster or Pieces, or to items you've bookmarked");
          break;
          
      }

      $items[] = t(l( $type->name, 'node/add/' . $type_url_str, array('query' => array('destination' => 'contribute'))) . '<div class="description">'. $type->description .   '</div>');

    }
  }
  
  $output .= theme('item_list', array('items' => $items));


  if (user_access('contribute content cluster')) {

    /**
    $query = "SELECT
                  node.nid AS nid,
                  node.language AS node_language,
                  node.title AS node_title,
                  node.created AS node_created
                FROM {node} node
                LEFT JOIN {content_field_contributors} node_data_field_contributors ON node.vid = node_data_field_contributors.vid
                INNER JOIN {users} users_node_data_field_contributors ON node_data_field_contributors.field_contributors_uid = users_node_data_field_contributors.uid
                WHERE (node.type in ('cluster'))
                AND (users_node_data_field_contributors.uid = %s)
                ORDER BY node_created ASC";

    $result = db_query($query, $user->uid);

    while ($data = db_fetch_object($result)) {
      $items[] = l($data->node_title, 'node/add/contributed-pieces/cluster/' . $data->nid);
    }
    $output .= theme('item_list', $items);
    **/

  }

  return $output;

}

function mediacommons_editorialworkflow_page_set_upcoming_hub() {
  return;
}

function mediacommons_editorialworkflow_set_current_hub() {
  return;
}

function mediacommons_editorialworkflow_admin_page() {
  
  $output = '';
  
  $output .= '<h2 class="title">' . t('Administer site' . '</h2>');

  if (user_access('administer users')) {
    $items[] = l('Administer users', 'admin/people');
  }
  
  if (user_access('set current hub')) {
    $items[] = l('Set current hub', 'user/admin/set-current-cluster', array('query' => array('destination' => 'contribute/dashboard')));
  }

  if (user_access('set upcoming hub')) {
    $items[] = l('Set upcoming hub', 'user/admin/set-upcoming-hub', array('query' => array('destination' => 'contribute/dashboard')));
  }

  $output .= theme('item_list', array('items' => $items));
  
  return $output;
  
}