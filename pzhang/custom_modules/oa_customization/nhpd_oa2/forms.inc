<?php

/***
 * Dynamic task status and workflow setting
 * As with chili, workflow depends on task type (tracker in chili) and OG role (role in chiki)
 */ 
 
 /*
function oa_task_status_form($form, &$form_state){
  //drupal_set_message(nhpid_api_dump($form_state, 'red', TRUE));
 	$task_status = variable_get_value('oa_worktracker_allowed_values_task_status');
  $task_status_options = $task_status['options'];
  // drupal_set_message(nhpid_api_dump($task_status_options, 'red', TRUE));
  $task_type = variable_get('oa_worktracker_allowed_values_task_type', null);
  $task_type_options = $task_type['options'];
	$og_roles = og_roles('node', 'oa_space');

	$form['#tree']=true;
	//$form['#theme'] = 'oa_workflow_form';
	$form['desc'] = array(
		'#type' => 'item',
		'#markup' => t('This setting is to define task statuses available to a task section.'),
	);
	$form['step1']= array(
    '#type' => 'fieldset',
    '#title' => t('Select a OG role and a task type to edit the workflow'),
    '#collapsible' => True,
    '#collapsed' => False,
	);
	$og_role = !empty($form_state['values']['step1']['og_role']) ? $form_state['values']['step1']['og_role'] : null;
  $task_type = !empty($form_state['values']['step1']['task_type']) ? $form_state['values']['step1']['task_type'] : null;
  $statuses_mode = isset($form_state['values']['step1']['statuses_mode']) ? $form_state['values']['step1']['statuses_mode'] : 1;
  //drupal_set_message(nhpid_api_dump($statuses_mode, 'red', TRUE));
  $form['step1']['og_role']= array(
    '#type' => 'select',
    '#title' => t('OG role'),
    '#options' => $og_roles,
		'#multiple' => false,
    '#default_value' => $og_role,
		'#required' => true,
	);
  $form['step1']['task_type']= array(
    '#type' => 'select',
    '#title' => t('Task type'),
    '#options' => $task_type_options,
		'#multiple' => false,
    '#default_value' => $task_type,
    '#required' => true,
	);
  $form['step1']['statuses_mode']= array(
    '#type' => 'checkbox',
    '#title' => t('Display only statuses used by this type'),
    '#default_value' => $statuses_mode,
	);
  $form['step1']['edit']= array(
    '#type' => 'submit',
    '#value' => t('Edit'),
	);
	$allow_statuses = array();
	$workflow_setting = array();
	if (!empty($form_state['clicked_button']['#id'])){
    $setting = variable_get('nhpd_oa_task_status_allowed_values_setting', array());
		$allowed_statuses = isset($setting[$task_type][$og_role]['setting'])? $setting[$task_type][$og_role]['setting']['allowed_statuses'] : array();
		$workflow_setting =isset($setting[$task_type][$og_role]['all_options'])? $setting[$task_type][$og_role]['all_options'] : array();
		unset($form_state['input']);
	}

	$setting_form = array();
  //first row
  $setting_form['allowed_statuses_header']['0']=array(
			'#type' => 'item',
      '#title' => null,
	);
	if ($statuses_mode == 1 && !empty($allowed_statuses)){
  	$task_status_options = array_intersect_key($task_status_options, array_flip($allowed_statuses));
	}
  //$task_status_options = array_slice($task_status_options, 0, 20);
  foreach ($task_status_options as $status_key => $status_value) {
    $setting_form['allowed_statuses_header'][$status_key]= array(
			'#type' => 'item',
      '#title' => $status_value,
		);
  }
  $setting_form['allowed_statuses'] = array(
    '#type' => 'value',
    '#value' => array_keys($setting_form['allowed_statuses_header']),
	);
  foreach ($task_status_options as $status_current_key => $status_current_value) {
    $setting_form[$status_current_key]['0'] = array(
			'#type' => 'item',
      '#title' => $status_current_value,
		);
		foreach ($task_status_options as $status_allowed_key => $status_allowed_value) {
		 	$setting_form[$status_current_key][$status_allowed_key] = array(
        '#type' => 'checkbox',
       );
		}
  }
  //drupal_set_message(nhpid_api_dump($workflow_setting, 'red', TRUE));
  $form['setting'] = array(
    '#type' => 'fieldset',
    //'#title' => '  ',
    '#collapsible' => false,
    '#collapsed' => False,
		'#access' => $task_type && $og_role,
    '#attributes' => array('style' => array('overflow:auto')),
	);
  $form['setting']['general_setting'] = array(
    '#type' => 'fieldset',
    '#title' => t('General setting'),
    '#collapsible' => true,
    '#collapsed' => False,
		//'#theme' => 'oa_workflow_form',
		'#attributes' => array('style' => array('overflow:auto')),
	);
  $form['setting']['general_setting']['workflow'] = array(
    '#type' => 'fieldset',
    //'#title' => t('Additional transitions allowed when the user is the assignee'),
    '#collapsible' => false,
    '#collapsed' => false,
		'#theme' => 'oa_workflow_form',
    '#attributes' => array('style' => array('overflow:auto')),
	);
  foreach ($task_status_options as $status_current_key => $status_current_value) {
    foreach ($task_status_options as $status_allowed_key => $status_allowed_value) {
			$default = isset($workflow_setting['general_setting'][$status_current_key][$status_allowed_key])? $workflow_setting['general_setting'][$status_current_key][$status_allowed_key] : 0;
			$default = isset($setting['general_setting']['workflow'][$status_current_key][$status_allowed_key]) ? $setting['general_setting']['workflow'][$status_current_key][$status_allowed_key] : $default;
		 	$setting_form[$status_current_key][$status_allowed_key]['#default_value'] = $default;
      //drupal_set_message(nhpid_api_dump("$status_current_key-$status_allowed_key: $default", 'red', TRUE));
		}
  }
  $form['setting']['general_setting']['workflow'] = array_merge($form['setting']['general_setting']['workflow'], $setting_form);
  $form['setting']['author_setting'] = array(
    '#type' => 'fieldset',
    '#title' => t('Additional transitions allowed when the user is the author'),
    '#collapsible' => true,
    '#collapsed' => true,
		//'#theme' => 'oa_workflow_form',
    '#attributes' => array('style' => array('overflow:auto')),
	);
  $form['setting']['author_setting']['workflow'] = array(
    '#type' => 'fieldset',
    //'#title' => t('Additional transitions allowed when the user is the assignee'),
    '#collapsible' => false,
    '#collapsed' => false,
		'#theme' => 'oa_workflow_form',
    '#attributes' => array('style' => array('overflow:auto')),
	);
  foreach ($task_status_options as $status_current_key => $status_current_value) {
    foreach ($task_status_options as $status_allowed_key => $status_allowed_value) {
      $default = isset($workflow_setting['author_setting'][$status_current_key][$status_allowed_key])? $workflow_setting['author_setting'][$status_current_key][$status_allowed_key] : 0;
			$default = isset($setting['author_setting']['workflow'][$status_current_key][$status_allowed_key]) ? $setting['author_setting']['workflow'][$status_current_key][$status_allowed_key] : $default;
		 	$setting_form[$status_current_key][$status_allowed_key]['#default_value'] = $default;
		}
  }
  $form['setting']['author_setting']['workflow'] = array_merge($form['setting']['author_setting']['workflow'], $setting_form);
  $form['setting']['assignee_setting'] = array(
    '#type' => 'fieldset',
    '#title' => t('Additional transitions allowed when the user is the assignee'),
    '#collapsible' => true,
    '#collapsed' => true,
		//'#theme' => 'oa_workflow_form',
    '#attributes' => array('style' => array('overflow:auto')),
	);
  $form['setting']['assignee_setting']['workflow'] = array(
    '#type' => 'fieldset',
    //'#title' => t('Additional transitions allowed when the user is the assignee'),
    '#collapsible' => false,
    '#collapsed' => false,
		'#theme' => 'oa_workflow_form',
    '#attributes' => array('style' => array('overflow:auto')),
	);
  foreach ($task_status_options as $status_current_key => $status_current_value) {
    foreach ($task_status_options as $status_allowed_key => $status_allowed_value) {
      $default = isset($workflow_setting['assignee_setting'][$status_current_key][$status_allowed_key])? $workflow_setting['assignee_setting'][$status_current_key][$status_allowed_key] : 0;
			$default = isset($setting['assignee_setting']['workflow'][$status_current_key][$status_allowed_key]) ? $setting['assignee_setting']['workflow'][$status_current_key][$status_allowed_key] : $default;
		 	$setting_form[$status_current_key][$status_allowed_key]['#default_value'] = $default;
		}
  }
  $form['setting']['assignee_setting']['workflow'] = array_merge($form['setting']['assignee_setting']['workflow'], $setting_form);

  $form['setting']['save_setting'] = array(
    '#type' => 'submit',
    '#value' => t('Save setting')
  );
  //drupal_set_message(nhpid_api_dump($form, 'red', TRUE));
	return $form;
}

function oa_task_status_form_submit($form, &$form_state){
  if ($form_state['clicked_button']['#id'] == 'edit-setting-save-setting'){
		$setting = array();
		$setting['allowed_statuses'] = array();
		$all_options = array();
		$all_options['general_setting'] = $form_state['values']['setting']['general_setting']['workflow'] ;
	  $all_options['author_setting'] = $form_state['values']['setting']['author_setting']['workflow'] ;
	  $all_options['assignee_setting'] = $form_state['values']['setting']['assignee_setting']['workflow'] ;
		unset($all_options['general_setting']['allowed_statuses'] );
	  unset($all_options['author_setting']['allowed_statuses'] );
	  unset($all_options['assignee_setting']['allowed_statuses'] );
		foreach(array($all_options['general_setting'], $all_options['author_setting'],$all_options['assignee_setting'] ) as $workflow_setting){
			foreach($workflow_setting as $current_status => $allowed_statuses){
				foreach ($allowed_statuses as $status => $allowed){
					if ($allowed){
            $setting['allowed_statuses'][] = $status;
						$setting['allowed_statuses'][] = $current_status;
					} 
				}
			}
		}
    $setting['allowed_statuses'] = array_unique($setting['allowed_statuses']);
		$og_role = $form_state['values']['step1']['og_role'];
	  $task_type = $form_state['values']['step1']['task_type'];

	  //drupal_set_message(nhpid_api_dump($setting, 'red', TRUE));
		$var = variable_get('nhpd_oa_task_status_allowed_values_setting', array());
		$var[$task_type][$og_role]= array('setting' =>$setting, 'all_options' => $all_options);
		variable_set('nhpd_oa_task_status_allowed_values_setting', $var);
	}
  $form_state['rebuild'] = TRUE;
}

function oa_task_type_form($form, &$form_state){
	$task_type = variable_get('oa_worktracker_allowed_values_task_type', null);
	$task_type_options = $task_type['options'];
	$tid = array_shift(taxonomy_get_term_by_name('Tasks Section', 'section_type')) -> tid;
	$query = new EntityFieldQuery();
	$result = $query->entityCondition('entity_type', 'node')
		->entityCondition('bundle', 'oa_section')
		->fieldCondition('field_oa_section', 'tid', $tid)
		->execute();
	$nodes = node_load_multiple(array_keys($result['node']));
	$section_id_space_names = array();
	foreach ($nodes as $nid => $node){
		$node_wrapper = entity_metadata_wrapper('node', $node);
		$space = $node_wrapper->og_group_ref->value();
		if($space) $space_name = $space -> title;
    	$section_id_space_names[$nid] = $space_name;

	}
	$form['#tree']=true;
	$form['desc'] = array(
		'#type' => 'item',
		'#markup' => 'This setting is to define task types available to a task section.',
	);
	$form['setting'] = array(
    '#type' => 'fieldset',
    '#title' => t('Setting'),
    '#collapsible' => False,
    '#collapsed' => False,
	);
	$setting = variable_get('nhpd_oa_task_type_allowed_values_setting', array());
	$setting = isset($setting['setting'])? $setting['setting'] : array();
	foreach($section_id_space_names as $nid => $space_name){
		$default=empty($setting) ? array() : $setting[$nid];
		$form['setting'][$nid]=array(
      '#type' => 'select',
			'#size' => 60,
      '#title' => $space_name,
      '#default_value' => $default,
      '#options' => $task_type_options,
			'#multiple' => true,
      '#description' => t('Select one or more types'),
		);
	}
	$form['all_options']=array(
    '#type' => 'value',
    '#value' => $task_type_options,
	);
  $form['save_setting'] = array(
    '#type' => 'submit',
    '#value' => t('Save setting')
  );

	return $form;
}

function oa_task_type_form_submit($form, &$form_state){
	$setting = $form_state['values']['setting'];
	$all_options = $form_state['values']['all_options'];

  //drupal_set_message(nhpid_api_dump($setting, 'red', TRUE));
	variable_set('nhpd_oa_task_type_allowed_values_setting', array('setting' =>$setting, 'all_options' => $all_options));
}

function oa_task_category_form($form, &$form_state){
	$task_category = variable_get('oa_worktracker_allowed_values_task_category', null);
	$task_category_options = $task_category['options'];
	$tid = array_shift(taxonomy_get_term_by_name('Tasks Section', 'section_type')) -> tid;
	$query = new EntityFieldQuery();
	$result = $query->entityCondition('entity_type', 'node')
		->entityCondition('bundle', 'oa_section')
		->fieldCondition('field_oa_section', 'tid', $tid)
		->execute();
	$nodes = node_load_multiple(array_keys($result['node']));
	$section_id_space_names = array();
	foreach ($nodes as $nid => $node){
		$node_wrapper = entity_metadata_wrapper('node', $node);
		$space = $node_wrapper->og_group_ref->value();
		if($space) $space_name = $space -> title;
    	$section_id_space_names[$nid] = $space_name;
	}

	$form['#tree']=true;
	$form['desc'] = array(
		'#type' => 'item',
		'#markup' => 'This setting is to define task types available to a task section.',
	);
	$form['setting'] = array(
    '#type' => 'fieldset',
    '#title' => t('Setting'),
    '#collapsible' => False,
    '#collapsed' => False,
	);
	$setting = variable_get('nhpd_oa_task_category_allowed_values_setting', array());
	$setting = isset($setting['setting'])? $setting['setting'] : array();
	foreach($section_id_space_names as $nid => $space_name){
		$default=isset($setting[$nid]) ?  $setting[$nid] : array();
		$form['setting'][$nid]=array(
      '#type' => 'select',
			'#size' => 60,
      '#title' => $space_name,
      '#default_value' => $default,
      '#options' => $task_category_options,
			'#multiple' => true,
      '#description' => t('Select one or more types'),
		);
	}
	$form['all_options']=array(
    '#type' => 'value',
    '#value' => $task_category_options,
	);
  $form['save_setting'] = array(
    '#type' => 'submit',
    '#value' => t('Save setting')
  );

	return $form;
}

function oa_task_category_form_submit($form, &$form_state){
	$setting = $form_state['values']['setting'];
	$all_options = $form_state['values']['all_options'];

  //drupal_set_message(nhpid_api_dump($setting, 'red', TRUE));
	variable_set('nhpd_oa_task_category_allowed_values_setting', array('setting' =>$setting, 'all_options' => $all_options));
}
*/

function oa_reporting_setting_form($form, &$form_state){
	global $user;
  //$customizable_fields = variable_get('oa_worktracker_customizable_fields', array());
  /*foreach($customizable_fields as $customizable_field => $value){
		if (!$value){
			unset($customizable_fields[$customizable_field]);
		}
	}*/
  $customizable_fields = variable_get('oa_worktracker_customizable_fields', array());
	foreach($customizable_fields as $customizable_field => $value){
		if (!$value) unset($customizable_fields[$customizable_field]);
	}
  $customizable_filters = variable_get('oa_worktracker_customizable_filters', array());
  foreach($customizable_filters as $customizable_filter => $value){
		if (!$value) unset($customizable_filters[$customizable_filter]);
	}
	$form['#tree']=true;
	drupal_set_title('Reporting Setting');
	$user_data = $user ->data;
	$setting = isset($user_data['task_reporting_setting']) ? $user_data['task_reporting_setting'] : array();

	$fields = isset($setting['task_reporting']['fields']) ? $setting['task_reporting']['fields'] : array();
	$filters = isset($setting['task_reporting']['filters']) ? $setting['task_reporting']['filters'] : array();
	$fields_options = array();
	$fields_default = empty($form_state['values']['task_reporting']['fields'])? $fields : $form_state['values']['task_reporting']['fields'];
  $filters_default = empty($form_state['values']['task_reporting']['filters'])? $filters : $form_state['values']['task_reporting']['filters'];
	$form['description'] = array(
    '#type' => 'item',
		'#markup' => 'This setting is to define extra fields and filters to been shown in task reporting. The setting is saved to your user account. So the setting is user specific.',
		);
	$form['task_reporting']=array(
    '#type' => 'fieldset',
		'#title' => t('Task reporting settings'),
  	'#collapsible' => TRUE,
  	'#collapsed' => FALSE,
	);
	$form['task_reporting']['fields'] = array(
    '#type' => 'select',
		'#size' => 60,
    '#title' => t('Custom fields for task reporting'),
    '#default_value' => $fields_default,
    '#options' => $customizable_fields,
		'#multiple' => true,
    '#description' => t('Select fields you want to been shown in task report.'),
	);
  $form['task_reporting']['filters'] = array(
    '#type' => 'select',
		'#size' => 60,
    '#title' => t('Custom filters for task reporting'),
    '#default_value' => $filters_default,
    '#options' => $customizable_filters,
		'#multiple' => true,
    '#description' => t('Select filters you want to been shown in task report.'),
	);
  
  $form['save_setting'] = array(
    '#type' => 'submit',
    '#value' => t('Save setting')
  );

	return $form;
}

function oa_reporting_setting_form_submit($form, &$form_state){
	global $user;
  //drupal_set_message(nhpid_api_dump($form_state, 'purple', TRUE));
	$setting = array('fields' => $form_state['values']['task_reporting']['fields'], 'filters' => $form_state['values']['task_reporting']['filters']);
	$user -> data['task_reporting_setting']['task_reporting'] = $setting;
	user_save($user);
	$form_state['rebuild'] = TRUE;

}

function theme_oa_workflow_form($variables){
	//$header = array();
	$header = array(t('Current status'), array('data' => t('New ststuses allowed'), 'colspan' => count($variables['element']['allowed_statuses']['#value'])-1));
  //$form = $variables['form'];
  //drupal_set_message(nhpid_api_dump($variables, 'purple', TRUE));
  $rows = array();
  $task_status = variable_get_value('oa_worktracker_allowed_values_task_status');
  $task_status_options = $task_status['options'];
	//first row
  foreach ($variables['element']['allowed_statuses']['#value'] as $status_key) {
			$rows['allowed_statuses_header'][$status_key] = drupal_render($variables['element']['allowed_statuses_header'][$status_key]);
  }
  foreach ($variables['element']['allowed_statuses']['#value'] as $status_current_key) {
		if ($status_current_key == '0') continue;
		foreach ($variables['element']['allowed_statuses']['#value'] as $status_allowed_key) {
			$rows[$status_current_key][$status_allowed_key] = array('data' => drupal_render($variables['element'][$status_current_key][$status_allowed_key]), 'align' => 'left');
  	}
	}
  //drupal_set_message(nhpid_api_dump($rows, 'purple', TRUE));
  $output = theme_table(array('header' => $header, 'rows' => $rows, 'attributes' => Null, 'caption' => Null,'colgroups' => Null,'sticky' => True,'empty' => '',));
	return $output;
}
