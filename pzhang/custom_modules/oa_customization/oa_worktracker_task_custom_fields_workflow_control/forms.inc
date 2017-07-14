<?php
/**
* Dynamic settings for custom fields
* The visibility of custom fields depends on 1) section id, and 2) task type
* Since task type can be changed by the user through task form, this dependency must be ajax controlled.
*/
function oa_task_custom_fields_form($form, &$form_state){
	//drupal_set_message(nhpid_api_dump($form_state, 'red', TRUE));
	//drupal_set_message(nhpid_api_dump($form_state['complete form']['sections'], 'red', TRUE));
	drupal_set_title('Custom Fields');
	$task_type = variable_get('oa_worktracker_allowed_values_task_type', null);
	$task_type_options = $task_type['options'];
	$section_options = array();
	$oa_spaces = array();
	$tids = array();
	$taxonomy_term_query = new EntityFieldQuery;
	$result = $taxonomy_term_query
		->entityCondition('entity_type', 'taxonomy_term')
		->propertyCondition('name', array('Tasks Section', 'Task Section', 'IN')) // in case task section is renamed
		->execute();
	foreach($result['taxonomy_term'] as $key => $term){
		$tids[] = $term -> tid;
	}
	$space_query = db_select('node', 'n')
		-> fields('n', array('nid', 'title'))
		-> condition('type', 'oa_space', '=')
		-> condition('status', NODE_PUBLISHED,'=')
		-> orderBy('title')
		-> execute();
	while($rc = $space_query -> fetchObject()){
		$space_nid = $rc -> nid;
		$space_title = $rc -> title;
		//$oa_spaces[$space_nid] = $space_title;
		//$options[$space_title] = array();
		//$section_query = new EntityFieldQuery();
		$section_query = new EntityFieldQuery;
		$result = $section_query->entityCondition('entity_type', 'node')
			->entityCondition('bundle', 'oa_section')
			->propertyCondition('status', NODE_PUBLISHED)
			->fieldCondition('og_group_ref', 'target_id', $space_nid, '=')
			->fieldCondition('field_oa_section', 'tid', $tids, 'IN')
			->execute();
			if(!isset($result['node'])) continue;
			foreach($result['node'] as $key => $section){
				$section_nid = $section->nid;
				$section_node = node_load($section_nid);
				$section_options[$space_title][$section_nid] = $section_node  -> title;
			}
	}
	$all_task_fields = array();
	foreach(field_info_instances('node', 'oa_worktracker_task') as $field_name => $info){
		$all_task_fields[$field_name] = $info['label'] . " ($field_name)";
	}
	$fields_excluded = array('body',
														'oa_section_ref',
														'og_group_ref',
														'field_oa_media',
														'field_oa_worktracker_task_status',
														'field_oa_worktracker_assigned_to',
														'field_oa_worktracker_priority',
														'field_oa_worktracker_task_type',
														'field_oa_worktracker_duedate',
														'field_parent_task',
														'field_related_tasks',
														'field_chili_id',
														'field_issue_category',
													);
	foreach($fields_excluded as $field_excluded){
		unset($all_task_fields[$field_excluded]);
	}
	natsort($all_task_fields);
	$form['#tree']=true;
	$form['desc'] = array(
		'#type' => 'item',
		'#markup' => 'This setting is to define custom fields allowed to sections and task types.',
	);
	if (!empty($form_state['clicked_button']['#id'])){
		unset($form_state['input']);
	}
	$setting = variable_get('nhpd_oa_task_custom_fields_allowed_values_setting', array());
	$section = !empty($form_state['values']['sections']['section']) ? $form_state['values']['sections']['section'] : NULL;
	$custom_fields = isset($setting['sections'][$section]) ? $setting['sections'][$section] : array();
	//drupal_set_message(nhpid_api_dump($section, 'red', TRUE));
	//drupal_set_message(nhpid_api_dump($setting, 'red', TRUE));
	//drupal_set_message(nhpid_api_dump($custom_fields, 'red', TRUE));
	$default_tab = 'edit-sections';
	if (isset($form_state['triggering_element'])){
		$default_tab = $form_state['complete form'][$form_state['triggering_element']['#parents'][0]]['#id'];
	}

	$form['vertical_tabs'] = array(
    '#type' => 'vertical_tabs',
    '#default_tab' => $default_tab,
   );
	 if (!isset($setting['all_custom_fields'])){
		$setting['all_custom_fields']= array_keys($all_task_fields);
		variable_set('nhpd_oa_task_custom_fields_allowed_values_setting', $setting);
	 }
	 $all_custom_fields = $setting['all_custom_fields'];
	 $all_custom_fields = !empty($form_state['values']['custom_fields']['custom_fields']) ? $form_state['values']['custom_fields']['custom_fields'] : $all_custom_fields;
	 //drupal_set_message(nhpid_api_dump($all_custom_fields, 'red', TRUE));
	 $form['custom_fields']= array(
    '#type' => 'fieldset',
    '#title' => t('Custom fields'),
    '#collapsible' => True,
    '#collapsed' => False,
		'#group' => 'vertical_tabs',
		'#description' => t("It checked, the field will be a <em>custom field</em>. The visibility of custom fields are dynamically controlled by section ids and task types."),
	);
	$form['custom_fields']['custom_fields']= array(
    '#type' => 'select',
    '#title' => t('Please select fields'),
		'#multiple' => true,
    '#options' => $all_task_fields,
    '#default_value' => $all_custom_fields,
	);
	$form['custom_fields']['save']= array(
    '#type' => 'submit',
    '#value' => t('Save setting'),
		'#name' => 'custom-fields-save',
	);
	$form['sections']= array(
    '#type' => 'fieldset',
    '#title' => t('Sections'),
    '#collapsible' => True,
    '#collapsed' => False,
		'#group' => 'vertical_tabs',
	);
	$form['sections']['section']= array(
    '#type' => 'select',
    '#title' => t('Please select a section'),
    '#options' => $section_options,
    '#default_value' => $section,
		'#attributes' => array('onchange' => 'document.getElementById("edit-sections-edit").click();'),
	);
	$form['sections']['edit']= array(
    '#type' => 'submit',
    '#value' => t('Edit'),
		'#name' => 'sections-edit',
	);
	$form['sections']['custom_fields']= array(
    '#type' => 'select',
    '#title' => t('Please select custom fields'),
		'#multiple' => true,
    '#options' => $all_custom_fields,
    '#default_value' => $custom_fields,
		'#access' => !is_null($section),
	);
	$form['sections']['save']= array(
    '#type' => 'submit',
    '#value' => t('Save setting'),
		'#access' => !is_null($section),
		'#name' => 'sections-save',
	);
	$form['sections']['summary']= array(
    '#type' => 'fieldset',
    '#title' => t('Summary'),
    '#collapsible' => True,
    '#collapsed' => True,
	);
	$sections_summary = array('attributes' => Null, 'caption' => Null,'colgroups' => Null,'sticky' => True,'empty' => '',);
	$sections_summary['header'] = array('Section', 'Custom fields');
	if (isset($setting['sections'])){
		foreach($setting['sections'] as $section_id => $custom_fields){
			foreach($section_options as $space_title => $section_option){
				foreach($section_option as $section_option_id => $section_title){
					if ($section_id == $section_option_id){
						$custom_fields_str = '';
						foreach($custom_fields as $custom_field){
							$custom_fields_str = $custom_fields_str . $all_task_fields[$custom_field] . '<br>';
						}
						$sections_summary['rows'][] = array("$space_title: $section_title ($section_id)", $custom_fields_str);
					}
					break 1;
				}
			}
		}
	}
	//drupal_set_message(nhpid_api_dump(theme_table($sections_summary), 'red', TRUE));
	$form['sections']['summary']['summary']= array(
    '#type' => 'item',
    '#markup' => theme_table($sections_summary),
	);
	$task_type = !empty($form_state['values']['task_types']['task_type']) ? $form_state['values']['task_types']['task_type'] : NULL;
	$custom_fields = isset($setting['task_types'][$task_type]) ? $setting['task_types'][$task_type] : array();
	$form['task_types']= array(
    '#type' => 'fieldset',
    '#title' => t('Task types'),
    '#collapsible' => True,
    '#collapsed' => False,
		'#group' => 'vertical_tabs',
	);
	$form['task_types']['task_type']= array(
    '#type' => 'select',
    '#title' => t('Please select a section'),
    '#options' => $task_type_options,
    '#default_value' => $task_type,
		'#attributes' => array('onchange' => "document.getElementById('edit-task-types-edit').click();"),
	);
	$form['task_types']['edit']= array(
    '#type' => 'submit',
    '#value' => t('Edit'),
		'#name' => 'task-types-edit',
	);
	$form['task_types']['custom_fields']= array(
    '#type' => 'select',
    '#title' => t('Please select custom fields'),
		'#multiple' => true,
    '#options' => $all_custom_fields,
    '#default_value' => $custom_fields,
		'#access' => !is_null($task_type),
	);
	$form['task_types']['save']= array(
    '#type' => 'submit',
    '#value' => t('Save setting'),
		'#access' => !is_null($task_type),
		'#name' => 'task-types-save',
	);
	$form['task_types']['summary']= array(
    '#type' => 'fieldset',
    '#title' => t('Summary'),
    '#collapsible' => True,
    '#collapsed' => True,
	);
	$task_types_summary = array('attributes' => Null, 'caption' => Null,'colgroups' => Null,'sticky' => True,'empty' => '',);
	$task_types_summary ['header'] = array('Task type', 'Custom fields');
	if (isset($setting['task_types'])){
		foreach($setting['task_types'] as $task_type => $custom_fields){
			$custom_fields_str = '';
			foreach($custom_fields as $custom_field){
				$custom_fields_str = $custom_fields_str . $all_task_fields[$custom_field] . '<br>';
			}
			$task_types_summary['rows'][] = array($task_type_options[$task_type], $custom_fields_str);
		}
	}
	//drupal_set_message(nhpid_api_dump(theme_table($task_types_summary), 'red', TRUE));
	$form['task_types']['summary']['summary']= array(
    '#type' => 'item',
    '#markup' => theme_table($task_types_summary ),
	);
	return $form;

}
function oa_task_custom_fields_form_submit($form, &$form_state){
  if ($form_state['clicked_button']['#id'] == 'edit-sections-save'){

		$custom_fields= $form_state['values']['sections']['custom_fields'] ;
		$section = $form_state['values']['sections']['section'];
	  //drupal_set_message(nhpid_api_dump($setting, 'red', TRUE));
		$var = variable_get('nhpd_oa_task_custom_fields_allowed_values_setting', array());
		$var['sections'][$section]= $custom_fields;
		variable_set('nhpd_oa_task_custom_fields_allowed_values_setting', $var);
	}
	elseif($form_state['clicked_button']['#id'] == 'edit-task-types-save'){

		$custom_fields= $form_state['values']['task_types']['custom_fields'] ;
		$task_type = $form_state['values']['task_types']['task_type'];
	  //drupal_set_message(nhpid_api_dump($setting, 'red', TRUE));
		$var = variable_get('nhpd_oa_task_custom_fields_allowed_values_setting', array());
		$var['task_types'][$task_type]= $custom_fields;
		variable_set('nhpd_oa_task_custom_fields_allowed_values_setting', $var);
	}
	elseif($form_state['clicked_button']['#id'] == 'edit-custom-fields-save'){
		$custom_fields= $form_state['values']['custom_fields']['custom_fields'] ;
		$var = variable_get('nhpd_oa_task_custom_fields_allowed_values_setting', array());
		$var['all_custom_fields']= $custom_fields;
		variable_set('nhpd_oa_task_custom_fields_allowed_values_setting', $var);
	}
  $form_state['rebuild'] = TRUE;
	//drupal_set_message(nhpid_api_dump($form_state, 'red', TRUE));
}
/***
 * Dynamic task status and workflow setting
 * As with chili, workflow depends on task type (tracker in chili) and OG role (role in chiki)
 */ 
function oa_task_status_form($form, &$form_state){
  //drupal_set_message(nhpid_api_dump($form_state, 'red', TRUE));
 	$task_status = variable_get_value('oa_worktracker_allowed_values_task_status');
  $task_status_options = $task_status['options'];
  // drupal_set_message(nhpid_api_dump($task_status_options, 'red', TRUE));
  $task_type = variable_get('oa_worktracker_allowed_values_task_type', null);
  $task_type_options = $task_type['options'];
	$og_roles = og_roles('node', 'oa_space');
	$setting = variable_get('nhpd_oa_task_status_allowed_values_setting', array());
	foreach($og_roles as $rid => $role){
		$og_roles[$rid] = $role . " (Id: $rid)";
	}

	$form['#tree']=true;
	//$form['#theme'] = 'oa_workflow_form';
	$form['desc'] = array(
		'#type' => 'item',
		'#markup' => 'This setting is to define task statuses (workflow) available to a task section (nhpd_oa_task_status_allowed_values_setting).',
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
    '#title' => 'General setting',
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
	$form['summary'] = array(
    '#type' => 'fieldset',
    '#title' => t('Summary'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
	);
  //drupal_set_message(nhpid_api_dump($form, 'red', TRUE));
	$header = array(t('Current Status'), t('General Allowed Statuses'), t('Author Allowed Statuses'), t('Assignee Allowed Statuses'));
	$rows = array();
	foreach($setting as $task_type => $setting_task_type){
		$form['summary'][$task_type] = array(
			'#type' => 'fieldset',
			'#title' => t("Task Type: ") . $task_type,
			'#collapsible' => TRUE,
			'#collapsed' => TRUE,
		);
		foreach($setting_task_type as $rid => $setting_rid){
			$form['summary'][$task_type][$rid] = array(
				'#type' => 'fieldset',
				'#title' => t("OG Role: ") . $og_roles[$rid],
				'#collapsible' => TRUE,
				'#collapsed' => FALSE,
			);
			$rows = array();
			foreach($setting_rid['setting']['allowed_statuses'] as $current_status){
				$row = array($current_status);
				$general_allowed_statuses = array();
				if (isset($setting_rid['all_options']['general_setting'][$current_status])){
					foreach($setting_rid['all_options']['general_setting'][$current_status] as $allowed_status => $allowed){
						if ($allowed) $general_allowed_statuses[] = $allowed_status;
					}
				}
				$row[] = implode(", ", $general_allowed_statuses);
				$author_allowed_statuses = array();
				if (isset($setting_rid['all_options']['author_setting'][$current_status])){
					foreach($setting_rid['all_options']['author_setting'][$current_status] as $allowed_status => $allowed){
						if ($allowed) $author_allowed_statuses[] = $allowed_status;
					}
				}
				$row[] = implode(", ", $author_allowed_statuses);
				$assignee_allowed_statuses = array();
				if (isset($setting_rid['all_options']['assignee_setting'][$current_status])){
					foreach($setting_rid['all_options']['assignee_setting'][$current_status] as $allowed_status => $allowed){
						if ($allowed) $assignee_allowed_statuses[] = $allowed_status;
					}
				}
				$row[] = implode(", ", $assignee_allowed_statuses);
				$rows[] = $row;
				$output = theme_table(array('header' => $header, 'rows' => $rows, 'attributes' => Null, 'caption' => Null,'colgroups' => Null,'sticky' => True,'empty' => '',));
				$form['summary'][$task_type][$rid]['summary'] = array(
					'#type' => 'item',
					'#markup' => $output,
				);
			}
		}
	}
	
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
	$setting = variable_get('nhpd_oa_task_type_allowed_values_setting', array());
	$setting = isset($setting['setting'])? $setting['setting'] : array();
	
	$form['#tree']=true;
	$form['desc'] = array(
		'#type' => 'item',
		'#markup' => 'This setting is to define task types available to a task section (nhpd_oa_task_type_allowed_values_setting).',
	);
	$form['setting'] = array(
    '#type' => 'fieldset',
    '#title' => t('Setting'),
    '#collapsible' => False,
    '#collapsed' => False,
	);
	$tids = array();
	$taxonomy_term_query = new EntityFieldQuery;
	$result = $taxonomy_term_query
		->entityCondition('entity_type', 'taxonomy_term')
		->propertyCondition('name', array('Tasks Section', 'Task Section', 'IN')) // in case task section is renamed
		->execute();
	foreach($result['taxonomy_term'] as $key => $term){
		$tids[] = $term -> tid;
	}
	$space_query = db_select('node', 'n')
		-> fields('n', array('nid', 'title'))
		-> condition('type', 'oa_space', '=')
		-> condition('status', NODE_PUBLISHED,'=')
		-> orderBy('title')
		-> execute();
	while($rc = $space_query -> fetchObject()){
		$space_nid = $rc -> nid;
		$space_title = $rc -> title;
		
		$section_query = new EntityFieldQuery;
		$result = $section_query->entityCondition('entity_type', 'node')
			->entityCondition('bundle', 'oa_section')
			->propertyCondition('status', NODE_PUBLISHED)
			->fieldCondition('og_group_ref', 'target_id', $space_nid, '=')
			->fieldCondition('field_oa_section', 'tid', $tids, 'IN')
			->execute();
		
		if(!isset($result['node'])) continue;
		$form['setting'][$space_nid] = array(
			'#type' => 'fieldset',
			'#title' => t('!space_title (space ID: !space_id)', array('!space_title' => $space_title, '!space_id' => $space_nid)),
			'#collapsible' => TRUE,
			'#collapsed' => False,
		);
		foreach($result['node'] as $key => $section){
			$section_nid = $section->nid;
			$section_node = node_load($section_nid);
			$default=isset($setting[$space_nid][$section_nid]) ?  $setting[$space_nid][$section_nid] : array();
			$form['setting'][$space_nid][$section_nid]=array(
				'#type' => 'select',
				'#size' => 60,
				'#title' => t('!section_title (section ID: !section_id)', array('!section_title' => $section_node -> title, '!section_id' => $section_nid)),
				'#default_value' => $default,
				'#options' => $task_type_options,
				'#multiple' => true,
				'#description' => t('Select one or more types'),
			);
		}
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
	$setting = variable_get('nhpd_oa_task_category_allowed_values_setting', array());
	$setting = isset($setting['setting'])? $setting['setting'] : array();
	$form['#tree']=true;
	$form['desc'] = array(
		'#type' => 'item',
		'#markup' => 'This setting is to define task categories available to a task section (nhpd_oa_task_category_allowed_values_setting).',
	);
	$form['setting'] = array(
    '#type' => 'fieldset',
    '#title' => t('Setting'),
    '#collapsible' => False,
    '#collapsed' => False,
	);
	$tids = array();
	$taxonomy_term_query = new EntityFieldQuery;
	$result = $taxonomy_term_query
		->entityCondition('entity_type', 'taxonomy_term')
		->propertyCondition('name', array('Tasks Section', 'Task Section', 'IN')) // in case task section is renamed
		->execute();
	foreach($result['taxonomy_term'] as $key => $term){
		$tids[] = $term -> tid;
	}
	$space_query = db_select('node', 'n')
		-> fields('n', array('nid', 'title'))
		-> condition('type', 'oa_space', '=')
		-> condition('status', NODE_PUBLISHED,'=')
		-> orderBy('title')
		-> execute();
	while($rc = $space_query -> fetchObject()){
		$space_nid = $rc -> nid;
		$space_title = $rc -> title;
		
		$section_query = new EntityFieldQuery;
		$result = $section_query->entityCondition('entity_type', 'node')
			->entityCondition('bundle', 'oa_section')
			->propertyCondition('status', NODE_PUBLISHED)
			->fieldCondition('og_group_ref', 'target_id', $space_nid, '=')
			->fieldCondition('field_oa_section', 'tid', $tids, 'IN')
			->execute();
		
		if(!isset($result['node'])) continue;
		$form['setting'][$space_nid] = array(
			'#type' => 'fieldset',
			'#title' => t('!space_title (space ID: !space_id)', array('!space_title' => $space_title, '!space_id' => $space_nid)),
			'#collapsible' => TRUE,
			'#collapsed' => False,
		);
		foreach($result['node'] as $key => $section){
			$section_nid = $section->nid;
			$section_node = node_load($section_nid);
			$default=isset($setting[$space_nid][$section_nid]) ?  $setting[$space_nid][$section_nid] : array();
			$form['setting'][$space_nid][$section_nid]=array(
				'#type' => 'select',
				'#size' => 60,
				'#title' => t('!section_title (section ID: !section_id)', array('!section_title' => $section_node -> title, '!section_id' => $section_nid)),
				'#default_value' => $default,
				'#options' => $task_category_options,
				'#multiple' => true,
				'#description' => t('Select one or more types'),
			);
		}
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
