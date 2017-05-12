<?php

function prepare_taxonomy_fields() {

  $country = 'a:247:{i:0;s:13:"United States";i:1;s:11:"Afghanistan";i:2;s:14:"Åland Islands";i:3;s:7:"Albania";i:4;s:7:"Algeria";i:5;s:14:"American Samoa";i:6;s:7:"Andorra";i:7;s:6:"Angola";i:8;s:8:"Anguilla";i:9;s:10:"Antarctica";i:10;s:19:"Antigua and Barbuda";i:11;s:9:"Argentina";i:12;s:7:"Armenia";i:13;s:5:"Aruba";i:14;s:9:"Australia";i:15;s:7:"Austria";i:16;s:10:"Azerbaijan";i:17;s:7:"Bahamas";i:18;s:7:"Bahrain";i:19;s:10:"Bangladesh";i:20;s:8:"Barbados";i:21;s:7:"Belarus";i:22;s:7:"Belgium";i:23;s:6:"Belize";i:24;s:5:"Benin";i:25;s:7:"Bermuda";i:26;s:6:"Bhutan";i:27;s:31:"Bolivia, Plurinational State of";i:28;s:22:"Bosnia and Herzegovina";i:29;s:8:"Botswana";i:30;s:13:"Bouvet Island";i:31;s:6:"Brazil";i:32;s:30:"British Indian Ocean Territory";i:33;s:17:"Brunei Darussalam";i:34;s:8:"Bulgaria";i:35;s:12:"Burkina Faso";i:36;s:7:"Burundi";i:37;s:8:"Cambodia";i:38;s:8:"Cameroon";i:39;s:6:"Canada";i:40;s:10:"Cape Verde";i:41;s:14:"Cayman Islands";i:42;s:24:"Central African Republic";i:43;s:4:"Chad";i:44;s:5:"Chile";i:45;s:5:"China";i:46;s:16:"Christmas Island";i:47;s:23:"Cocos (Keeling) Islands";i:48;s:8:"Colombia";i:49;s:7:"Comoros";i:50;s:5:"Congo";i:51;s:37:"Congo, the Democratic Republic of the";i:52;s:12:"Cook Islands";i:53;s:10:"Costa Rica";i:54;s:14:"Côte D\'ivoire";i:55;s:7:"Croatia";i:56;s:4:"Cuba";i:57;s:6:"Cyprus";i:58;s:14:"Czech Republic";i:59;s:7:"Denmark";i:60;s:8:"Djibouti";i:61;s:8:"Dominica";i:62;s:18:"Dominican Republic";i:63;s:7:"Ecuador";i:64;s:5:"Egypt";i:65;s:11:"El Salvador";i:66;s:17:"Equatorial Guinea";i:67;s:7:"Eritrea";i:68;s:7:"Estonia";i:69;s:8:"Ethiopia";i:70;s:27:"Falkland Islands (Malvinas)";i:71;s:13:"Faroe Islands";i:72;s:4:"Fiji";i:73;s:7:"Finland";i:74;s:6:"France";i:75;s:13:"French Guiana";i:76;s:16:"French Polynesia";i:77;s:27:"French Southern Territories";i:78;s:5:"Gabon";i:79;s:6:"Gambia";i:80;s:7:"Georgia";i:81;s:7:"Germany";i:82;s:5:"Ghana";i:83;s:9:"Gibraltar";i:84;s:6:"Greece";i:85;s:9:"Greenland";i:86;s:7:"Grenada";i:87;s:10:"Guadeloupe";i:88;s:4:"Guam";i:89;s:9:"Guatemala";i:90;s:8:"Guernsey";i:91;s:6:"Guinea";i:92;s:13:"Guinea-Bissau";i:93;s:6:"Guyana";i:94;s:5:"Haiti";i:95;s:33:"Heard Island and Mcdonald Islands";i:96;s:29:"Holy See (Vatican City State)";i:97;s:8:"Honduras";i:98;s:9:"Hong Kong";i:99;s:7:"Hungary";i:100;s:7:"Iceland";i:101;s:5:"India";i:102;s:9:"Indonesia";i:103;s:25:"Iran, Islamic Republic of";i:104;s:4:"Iraq";i:105;s:7:"Ireland";i:106;s:11:"Isle of Man";i:107;s:6:"Israel";i:108;s:5:"Italy";i:109;s:7:"Jamaica";i:110;s:5:"Japan";i:111;s:6:"Jersey";i:112;s:6:"Jordan";i:113;s:10:"Kazakhstan";i:114;s:5:"Kenya";i:115;s:8:"Kiribati";i:116;s:38:"Korea, Democratic People\'s Republic of";i:117;s:18:"Korea, Republic of";i:118;s:6:"Kuwait";i:119;s:10:"Kyrgyzstan";i:120;s:32:"Lao People\'s Democratic Republic";i:121;s:6:"Latvia";i:122;s:7:"Lebanon";i:123;s:7:"Lesotho";i:124;s:7:"Liberia";i:125;s:22:"Libyan Arab Jamahiriya";i:126;s:13:"Liechtenstein";i:127;s:9:"Lithuania";i:128;s:10:"Luxembourg";i:129;s:5:"Macao";i:130;s:42:"Macedonia, the Former Yugoslav Republic of";i:131;s:10:"Madagascar";i:132;s:6:"Malawi";i:133;s:8:"Malaysia";i:134;s:8:"Maldives";i:135;s:4:"Mali";i:136;s:5:"Malta";i:137;s:16:"Marshall Islands";i:138;s:10:"Martinique";i:139;s:10:"Mauritania";i:140;s:9:"Mauritius";i:141;s:7:"Mayotte";i:142;s:6:"Mexico";i:143;s:31:"Micronesia, Federated States of";i:144;s:20:"Moldova, Republic of";i:145;s:6:"Monaco";i:146;s:8:"Mongolia";i:147;s:10:"Montenegro";i:148;s:10:"Montserrat";i:149;s:7:"Morocco";i:150;s:10:"Mozambique";i:151;s:7:"Myanmar";i:152;s:7:"Namibia";i:153;s:5:"Nauru";i:154;s:5:"Nepal";i:155;s:11:"Netherlands";i:156;s:20:"Netherlands Antilles";i:157;s:13:"New Caledonia";i:158;s:11:"New Zealand";i:159;s:9:"Nicaragua";i:160;s:5:"Niger";i:161;s:7:"Nigeria";i:162;s:4:"Niue";i:163;s:14:"Norfolk Island";i:164;s:24:"Northern Mariana Islands";i:165;s:6:"Norway";i:166;s:4:"Oman";i:167;s:8:"Pakistan";i:168;s:5:"Palau";i:169;s:31:"Palestinian Territory, Occupied";i:170;s:6:"Panama";i:171;s:16:"Papua New Guinea";i:172;s:8:"Paraguay";i:173;s:4:"Peru";i:174;s:11:"Philippines";i:175;s:8:"Pitcairn";i:176;s:6:"Poland";i:177;s:8:"Portugal";i:178;s:11:"Puerto Rico";i:179;s:5:"Qatar";i:180;s:8:"Réunion";i:181;s:7:"Romania";i:182;s:18:"Russian Federation";i:183;s:6:"Rwanda";i:184;s:17:"Saint Barthélemy";i:185;s:12:"Saint Helena";i:186;s:21:"Saint Kitts and Nevis";i:187;s:11:"Saint Lucia";i:188;s:12:"Saint Martin";i:189;s:25:"Saint Pierre and Miquelon";i:190;s:32:"Saint Vincent and the Grenadines";i:191;s:5:"Samoa";i:192;s:10:"San Marino";i:193;s:21:"Sao Tome and Principe";i:194;s:12:"Saudi Arabia";i:195;s:7:"Senegal";i:196;s:6:"Serbia";i:197;s:10:"Seychelles";i:198;s:12:"Sierra Leone";i:199;s:9:"Singapore";i:200;s:8:"Slovakia";i:201;s:8:"Slovenia";i:202;s:15:"Solomon Islands";i:203;s:7:"Somalia";i:204;s:12:"South Africa";i:205;s:44:"South Georgia and the South Sandwich Islands";i:206;s:5:"Spain";i:207;s:9:"Sri Lanka";i:208;s:5:"Sudan";i:209;s:8:"Suriname";i:210;s:22:"Svalbard and Jan Mayen";i:211;s:9:"Swaziland";i:212;s:6:"Sweden";i:213;s:11:"Switzerland";i:214;s:20:"Syrian Arab Republic";i:215;s:25:"Taiwan, Province of China";i:216;s:10:"Tajikistan";i:217;s:28:"Tanzania, United Republic of";i:218;s:8:"Thailand";i:219;s:11:"Timor-Leste";i:220;s:4:"Togo";i:221;s:7:"Tokelau";i:222;s:5:"Tonga";i:223;s:19:"Trinidad and Tobago";i:224;s:7:"Tunisia";i:225;s:6:"Turkey";i:226;s:12:"Turkmenistan";i:227;s:24:"Turks and Caicos Islands";i:228;s:6:"Tuvalu";i:229;s:6:"Uganda";i:230;s:7:"Ukraine";i:231;s:20:"United Arab Emirates";i:232;s:14:"United Kingdom";i:233;s:36:"United States Minor Outlying Islands";i:234;s:7:"Uruguay";i:235;s:10:"Uzbekistan";i:236;s:7:"Vanuatu";i:237;s:18:"Vatican City State";i:238;s:33:"Venezuela, Bolivarian Republic of";i:239;s:8:"Viet Nam";i:240;s:23:"Virgin Islands, British";i:241;s:20:"Virgin Islands, U.S.";i:242;s:17:"Wallis and Futuna";i:243;s:14:"Western Sahara";i:244;s:5:"Yemen";i:245;s:6:"Zambia";i:246;s:8:"Zimbabwe";}';

  $state = 'a:56:{i:0;s:7:"Alabama";i:1;s:6:"Alaska";i:2;s:14:"American Samoa";i:3;s:7:"Arizona";i:4;s:8:"Arkansas";i:5;s:10:"California";i:6;s:8:"Colorado";i:7;s:11:"Connecticut";i:8;s:8:"Delaware";i:9;s:20:"District of Columbia";i:10;s:7:"Florida";i:11;s:7:"Georgia";i:12;s:4:"Guam";i:13;s:6:"Hawaii";i:14;s:5:"Idaho";i:15;s:8:"Illinois";i:16;s:7:"Indiana";i:17;s:4:"Iowa";i:18;s:6:"Kansas";i:19;s:8:"Kentucky";i:20;s:9:"Louisiana";i:21;s:5:"Maine";i:22;s:8:"Maryland";i:23;s:13:"Massachusetts";i:24;s:8:"Michigan";i:25;s:9:"Minnesota";i:26;s:11:"Mississippi";i:27;s:8:"Missouri";i:28;s:7:"Montana";i:29;s:8:"Nebraska";i:30;s:6:"Nevada";i:31;s:13:"New Hampshire";i:32;s:10:"New Jersey";i:33;s:10:"New Mexico";i:34;s:8:"New York";i:35;s:14:"North Carolina";i:36;s:12:"North Dakota";i:37;s:25:"Northern Marianas Islands";i:38;s:4:"Ohio";i:39;s:8:"Oklahoma";i:40;s:6:"Oregon";i:41;s:12:"Pennsylvania";i:42;s:11:"Puerto Rico";i:43;s:12:"Rhode Island";i:44;s:14:"South Carolina";i:45;s:12:"South Dakota";i:46;s:9:"Tennessee";i:47;s:5:"Texas";i:48;s:4:"Utah";i:49;s:7:"Vermont";i:50;s:8:"Virginia";i:51;s:14:"Virgin Islands";i:52;s:10:"Washington";i:53;s:13:"West Virginia";i:54;s:9:"Wisconsin";i:55;s:7:"Wyoming";}';

  /** Country */
  $vocabulary = taxonomy_vocabulary_machine_name_load ( 'country' );
  /** Does the vocabulary organizations exist */
  if ( $vocabulary ) {
  	foreach ( unserialize($country) as $value ) {
  	  $query = new EntityFieldQuery;
  	  $term = $query
  	    ->entityCondition('entity_type', 'taxonomy_term')
  	    ->propertyCondition('name', $value )
  	    ->propertyCondition('vid', $vocabulary->vid)
  	    ->execute();
  	  if ( $term ) {
  		$keys = array_keys( $term['taxonomy_term'] ) ;
  		$tid = array_pop ( $keys );
  	  }
  	  else {
  		$term = new stdClass();
  		$term->name = trim ( $value );
  		$term->vid = $vocabulary->vid;
  		taxonomy_term_save( $term );
  		$tid = $term->tid;
  	  }
  	  drush_log( $vocabulary->name . ': ' . $value , 'status') ;
  	}
  }

  /** State */
  $vocabulary = taxonomy_vocabulary_machine_name_load ( 'state' );
  /** Does the vocabulary organizations exist */
  if ( $vocabulary ) {
  	foreach ( unserialize($state) as $value ) {
  		$query = new EntityFieldQuery;
  		$term = $query
  		->entityCondition('entity_type', 'taxonomy_term')
  		->propertyCondition('name', $value )
  		->propertyCondition('vid', $vocabulary->vid)
  		->execute();
  		if ( $term ) {
  		  $keys = array_keys( $term['taxonomy_term'] ) ;
  		  $tid = array_pop ( $keys );
  		}
  		else {
  			$term = new stdClass();
  			$term->name = trim ( $value );
  			$term->vid = $vocabulary->vid;
  			taxonomy_term_save( $term );
  			$tid = $term->tid;
  		}
  		drush_log( $vocabulary->name . ': ' . $value , 'status') ;
  	}
  }
}

function find_tid_with_otid($otid) {
	return mediacommons_base_import_find_term_id($otid);
}

// See: https://jira.nyu.edu/browse/MC-156
function mediacommons_base_import_find_term_id($toid, $project = 'drupal6') {
	$query = db_query("SELECT tid FROM {mediacommons_base_import_term_map} WHERE toid = :toid AND project = :project", array(':toid' => $toid, ':project' => $project));
	if ($query->rowCount() > 0 && $result = $query->fetchObject()) {
    return $result->tid;
  }
	return FALSE;
}

function mediacommons_base_import_find_term($tid) {
	$term = array();
	$term_d7_tid = db_query("SELECT tid FROM {mediacommons_base_import_term_map} WHERE toid = :toid", array(':toid' => $tid));
	if (!$term_d7_tid->rowCount()) {
		$query = '
      SELECT term.tid, term.vid, term.name, term.description, term.weight, hierarchy.parent
      FROM {term_data} term
      LEFT JOIN {term_hierarchy} hierarchy
        ON term.tid = hierarchy.tid
      WHERE term.tid = :tid';
		/** Create a database connection to the source (d6) database */
		$result = Database::getConnection('default', 'drupal6')->query($query, array(':tid' => $tid), array());
		$term = mediacommons_base_import_create_taxonomy_term($result->fetchObject());
	}
	else {
		$t = $term_d7_tid->fetchObject();
		$term = taxonomy_term_load($t->tid);
	}
	return $term;
}


function mediacommons_base_import_find_vocabulary( $vid ) {

	$vocabulary = array();

	$vocabulary_d7_vid = db_query("SELECT vid FROM {mediacommons_base_import_vocabulary_map} WHERE vuid = :vuid;", array(':vuid' => $vid));

	if ( ! $vocabulary_d7_vid->rowCount() ) {
		$query = '
        SELECT
          vocabulary.vid,
          vocabulary.name,
          vocabulary.description,
          vocabulary.help,
          vocabulary.relations,
          vocabulary.hierarchy,
          vocabulary.multiple,
          vocabulary.required,
          vocabulary.tags,
          vocabulary.module,
          vocabulary.weight
        FROM {vocabulary} vocabulary
        WHERE vocabulary.vid = :vid';

		/** Create a database connection to the source (d6) database */
		$result = Database::getConnection('default', 'drupal6')->query($query, array(':vid' => $vid), array());

		if ($result->rowCount()) {
			$vocabulary = mediacommons_base_import_create_taxonomy_vocabulary($result->fetchObject());
		}

	}
	else {
		$v = $vocabulary_d7_vid->fetchObject();
		$vocabulary = taxonomy_vocabulary_load($v->vid);
	}

	return $vocabulary;

}

function mediacommons_base_import_d7_vocabulary_list() {
	return taxonomy_get_vocabularies();
}

function mediacommons_base_import_d7_term_list() {
	return taxonomy_get_vocabularies();
}

function mediacommons_base_import_d6_term_list() {
	$query = 'SELECT term.tid, term.vid, term.name, term.description, term.weight FROM {term_data} term;';
	/** Create a database connection to the source (d6) database */
	return Database::getConnection('default', 'drupal6')->query($query, array(), array()) ;

}

function mediacommons_base_import_d6_vocabulary_list() {
	$query = 'SELECT * FROM {vocabulary};';
	return Database::getConnection('default', 'drupal6')->query($query, array(), array()) ;
}

function d6_terms () {
	$terms = array();
	$query = 'SELECT tid, vid, name, description, weight FROM {term_data}';
	$target = 'default';
	foreach (array('drupal6', 'int6', 'imr6') as $project) {
	  /** Create a database connection to the source (d6) database */
	  foreach (Database::getConnection($target, $project)->query($query) as $result) {
      $result->project = $project;
		  $result->md5 = md5($result->name);
		  $terms[] = $result;
  	}
	}
	return $terms;
}

