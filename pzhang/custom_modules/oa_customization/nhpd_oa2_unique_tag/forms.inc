<?php

/***
 * 
 */ 
function nhpd_oa2_unique_tag_form($form, &$form_states){
	
	$form['#tree']=true;
	$form['desc'] = array(
		'#type' => 'item',
		'#markup' => 'This setting is to manage node unique tag. A uniqe tag for a oa task (or other activity) is the abbreviation of its corresponding space name and its nid. ',
	);
	$form['setting'] = array(
    '#type' => 'fieldset',
    '#title' => t('Setting'),
    '#collapsible' => True,
    '#collapsed' => False,
	);
	$setting = variable_get('nhpd_oa_unique_tag_setting', array());
	$form['setting']['is_on']=array(
		'#title' => t('Turn on the unique tag?'),
    '#type' => 'checkbox',
    '#default_value' => isset($setting['is_on']) ? $setting['is_on'] : TRUE,
    '#description' => t('If checked, the titles of newly added nodes and updated nodes of select types will be prefixed with tags.'),
	);
	$field = field_info_field('oa_section_ref');
	$node_types = node_type_get_types();
	$candidates = array();
  $node_types_default = isset($setting['node_types'])? $setting['node_types'] : array();
	foreach ($field['bundles']['node'] as $bundle){
    $candidates[$bundle] = $node_types[$bundle] -> name;
	}
	$form['setting']['node_types']=array(
    '#type' => 'select',
		'#size' => 60,
    '#title' => t('Node types'),
    '#default_value' => $node_types_default,
    '#options' => $candidates,
		'#multiple' => true,
    '#description' => t('Select node types which will use unique tag.'),
	);
  $form['setting']['save_setting'] = array(
    '#type' => 'submit',
    '#value' => t('Save setting')
  );
  $form['operation'] = array(
    '#type' => 'fieldset',
    '#title' => t('Operations'),
    '#collapsible' => True,
    '#collapsed' => True,
	);
  $form['operation']['desc'] = array(
		'#type' => 'item',
		'#markup' => 'Adding or removing tags to all nodes of selected content types. Please set the setting first. ',
	);
  $form['operation']['regenerate_tag'] = array(
    '#type' => 'submit',
    '#value' => t('Regenerate tags'),
		'#submit' => array('nhpd_oa2_unique_tag_regenerate_tags'),
    '#attributes' => array('onclick' => 'return confirm("This operation will add unique tags to nodes of all predefined types. Are you sure you wish to continue?");'),
  );

	$form['operation']['remove_tag'] = array(
    '#type' => 'submit',
    '#value' => t('Remove tags'),
    '#submit' => array('nhpd_oa2_unique_tag_remove_tags'),
    '#attributes' => array('onclick' => 'return confirm("This operation will remove unique tags to nodes of all predefined types.Are you sure you wish to continue?");'),
  );


	return $form;
}

function nhpd_oa2_unique_tag_form_submit($form, &$form_states){
	$setting = $form_states['values']['setting'];

  //drupal_set_message(nhpid_api_dump($setting, 'red', TRUE));
	variable_set('nhpd_oa_unique_tag_setting', $setting);
	drupal_set_message(t('Unique tag setting has been updated.'));
}

function nhpd_oa2_unique_tag_regenerate_tags(){
  $setting = variable_get('nhpd_oa_unique_tag_setting', array());
  $operations = array();
	//if($setting && $setting['is_on']){
		$node_types = $setting['node_types'];
		//$count = 0;
		foreach ($node_types as $node_type){
      $query = new EntityFieldQuery();
			$query->entityCondition('entity_type', 'node')
			  		->entityCondition('bundle', $node_type);
      $result = $query->execute();
      foreach($result['node'] as $nid => $info){
        $operations[] = array('_nhpd_oa2_unique_tag_regenerate_tag', array($nid));
				//$count ++;
				//if ($count > 100) break;
			}

		}
    $batch = array(
	    'operations' => $operations,
	    'finished' => '_nhpd_oa2_unique_tag_batch_op_finished',
	    // We can define custom messages instead of the default ones.
	    'title' => t('Regenerating task unique tags'),
	    'init_message' => t('Regeneration of unique tags is starting.'),
	    'progress_message' => t('Processed @current out of @total steps.'),
	    'error_message' => t('Regeneration of unique tags has encountered an error.'),
      'file' => drupal_get_path('module', 'nhpd_oa2_unique_tag'). '/forms.inc',
	  );

	  batch_set($batch);
	  batch_process();
	//}
}

function nhpd_oa2_unique_tag_remove_tags(){
  $setting = variable_get('nhpd_oa_unique_tag_setting', array());
  $operations = array();
	//if($setting && $setting['is_on']){
		$node_types = $setting['node_types'];
		foreach ($node_types as $node_type){
      $query = new EntityFieldQuery();
			$query->entityCondition('entity_type', 'node')
			  		->entityCondition('bundle', $node_type);
      $result = $query->execute();
      foreach($result['node'] as $nid => $info){
        $operations[] = array('_nhpd_oa2_unique_tag_remove_tag', array($nid));
			}

		}
    $batch = array(
	    'operations' => $operations,
	    'finished' => 'nhpd_oa2_unique_tag_batch_op_finished',
	    // We can define custom messages instead of the default ones.
	    'title' => t('Removing task unique tags'),
	    'init_message' => t('Removal of unique tags is starting.'),
	    'progress_message' => t('Processed @current out of @total steps.'),
	    'error_message' => t('Removal of unique tags has encountered an error.'),
      'file' => drupal_get_path('module', 'nhpd_oa2_unique_tag'). '/forms.inc',
	  );

	  batch_set($batch);
	  batch_process();
	//}
}

function nhpd_oa2_unique_tag_batch_op_finished($success, $results, $operations) {
  if ($success) {
    // Here we could do something meaningful with the results.
    // We just display the number of nodes we processed...
    $message = count($results) . ' processed.';
    //field_reference_update_all_cached_option_lists();
  }
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    $message = 'An error occurred while processing ' . $error_operation[0] . ' with arguments :' . print_r($error_operation[0], TRUE);
  }
  drupal_set_message($message);
}

function _nhpd_oa2_unique_tag_regenerate_tag($nid, $context) {
	global $user;
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = 1;
  }
	$node = node_load($nid);
	$node -> title = nhpd_oa2_unique_tag_regenerate_unique_tag($node);
	drupal_write_record('node', $node, 'nid');
  _node_save_revision($node, $user->uid, 'vid');
	entity_get_controller('node')->resetCache(array($node -> nid));
  // Update our progress information.
  $context['sandbox']['progress']++;
  $context['message'] = $node -> title;

  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
  //drupal_set_message($message);
}
function _nhpd_oa2_unique_tag_remove_tag($nid, $context) {
	global $user;
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = 1;
  }
	$node = node_load($nid);
	$node -> title = nhpd_oa2_unique_tag_remove_unique_tag($node);
	drupal_write_record('node', $node, 'nid');
  _node_save_revision($node, $user->uid, 'vid');
	entity_get_controller('node')->resetCache(array($node -> nid));
  // Update our progress information.
  $context['sandbox']['progress']++;
  $context['message'] = $node -> title;

  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }

}