<?php

function mediacommons_survey_import_migrate_all_survey_question() {

  mediacommons_survey_import_migrate_survey_question(
    mediacommons_survey_import_d6_survey_question_list()
  );

}

function mediacommons_survey_import_migrate_survey_question ( $data = array() ) {

  foreach ( $data as $key => $hub ) {

    /** Prepare node */

  	$node = array();

    $node['title'] =  $hub->title;

    $node['ouid'] = $hub->uid;

    $node['onid'] = $hub->nid;

    $node['language'] = ( !empty( $hub->language ) ) ? $hub->language : LANGUAGE_NONE;

    $node['date'] = date('Y-m-j H:i:s', $hub->created);

    $node['status'] = $hub->status;

    $node['promote'] = $hub->promote;

    $node['moderate'] = $hub->moderate;

    $node['sticky'] = $hub->sticky;

    $node['field_pubstat'] = $hub->status;

    if ( isset( $hub->pieces ) ) $node['field_spokes'] = explode(", ", $hub->pieces);

    mediacommons_base_import_create_hub( $node );

  }
}