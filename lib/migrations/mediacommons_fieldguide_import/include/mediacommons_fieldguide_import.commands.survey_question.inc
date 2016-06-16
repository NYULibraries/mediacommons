<?php

function migrate_survey_questions() {
  foreach (survey_questions() as $old_node) migrate_survey_question($old_node);
}

function migrate_survey_question($old_node) {
  $node = array();
  $node['title'] =  $old_node->title;
  $node['ouid'] = $old_node->uid;
  $node['onid'] = $old_node->nid;
  $node['language'] = LANGUAGE_NONE;
  $node['date'] = date( 'Y-m-j H:i:s', $old_node->created);
  $node['status'] = $old_node->status;
  $node['promote'] = $old_node->promote;
  $node['moderate'] = $old_node->moderate;
  $node['sticky'] = $old_node->sticky;
  $node['field_pubstat'] = $old_node->status;
  if (isset($old_node->pieces)) $node['field_spokes'] = explode (', ', $old_node->pieces);
  create_hub($node);
}

function test_this() {
  $nodes = mediacommons_base_import_find_nodes('hub');
  foreach ($nodes as $node) {
    // array to hold contributors user ids
    $contributors = array();
    // load the node by NID
    $entity = node_load($node->nid);
    // wrap it with Entity API
    $wrapper = entity_metadata_wrapper('node', $entity);
    // find spokes, load them and get the user id
    foreach ($entity->field_spokes[$entity->language] as $value) {
      $spoke = node_load($value['nid']);
      $contributors[] = $spoke->uid;
    }
    // assign value
    $wrapper->field_contributors->set(array_unique($contributors));
    // save the node
    $wrapper->save();
  }
}
