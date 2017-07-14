<?php

$plugin = array(
  'single' => TRUE,
  'title' => t('Task list visible custom fields and filters'),
  'description' => t(''),
  'category' => t('Custom Panes'),
  'edit form' => 'nhpd_oa2_task_list_visible_fields_filters_edit_form',
  'render callback' => 'nhpd_oa2_task_list_visible_fields_filters_render',
  'admin info' => 'nhpd_oa2_task_list_visible_fields_filters_info',
  'defaults' => array('description' => t('To set visible (dynamic) fields and filters for the views showing task list in spaces and sections'),
      								'title' => t('Task list visible custom fields and filters'),),
  'all contexts' => TRUE,
);
function nhpd_oa2_task_list_visible_fields_filters_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];
  $form['description'] = array(
    '#title' => t('Description'),
    '#description' => t('Description (administrative)'),
    '#type' => 'textfield',
    '#required' => FALSE,
    '#default_value' => $conf['description'],
  );

  return $form;
}
function nhpd_oa2_task_list_visible_fields_filters_edit_form_submit($form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

function nhpd_oa2_task_list_visible_fields_filters_render($subtype, $conf, $args, $contexts) {
	$block = new stdClass();

	// initial content is blank
	$block->title = t('Task list custom fields/filters setting'); // This will be overridden by the user within the panel options.
	$form = drupal_get_form('_nhpd_oa2_task_list_visible_fields_filters_render_form');
	$content = drupal_render($form);
	//$content = 'example content';
	$block->content = $content;
	//drupal_set_message(nhpid_api_dump($content, 'red', TRUE));
	return $block;
}

function nhpd_oa2_task_list_visible_fields_filters_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass;
    $block->title = $conf['override_title'] ? $conf['override_title_text'] : 'Task list custom fields/filters setting';
    return $block;
  }
}

function _nhpd_oa2_task_list_visible_fields_filters_render_form($form, &$form_state){
	global $user;
	$currentnode_nid = currentnode_nid();
	$user_data = property_exists($user, 'data')? $user->data : array();
	$task_list_setting = isset($user_data['task_list_setting'])? $user_data['task_list_setting'] : array();
	if ($currentnode_nid){
		$currentnode_type = $currentnode_nid ? node_load($currentnode_nid) -> type : null;
    //if ($currentnode_type != 'oa_section') return; //
    $customizable_fields = nhpd_oa2_task_list_customizable_fields_for_section($currentnode_nid);
  	$customizable_filters = nhpd_oa2_task_list_customizable_filters_for_section($currentnode_nid);
    $fields = isset($task_list_setting[$currentnode_nid]['fields']) ? $task_list_setting[$currentnode_nid]['fields'] : array();
		$filters = isset($task_list_setting[$currentnode_nid]['filters']) ? $task_list_setting[$currentnode_nid]['filters'] : array();
		$fields_options = array();
		$fields_default = empty($form_states['values']['fields'])? $fields : $form_states['values']['fields'];
	  $filters_default = empty($form_states['values']['filters'])? $filters : $form_states['values']['filters'];
		$form['fields'] = array(
	    '#type' => 'select',
			'#size' => 60,
	    '#title' => t('Visible fields for task list'),
	    '#default_value' => $fields_default,
	    '#options' => $customizable_fields,
			'#multiple' => true,
	    '#description' => t('Select fields you want to been shown in task report.'),
		);
	  $form['filters'] = array(
	    '#type' => 'select',
			'#size' => 60,
	    '#title' => t('Visible filters for task list'),
	    '#default_value' => $filters_default,
	    '#options' => $customizable_filters,
			'#multiple' => true,
	    '#description' => t('Select filters you want to been shown in task report.'),
		);
	  $form['nid'] = array(
	    '#type' => 'value',
			'#value' => $currentnode_nid,
		);
	  $form['type'] = array(
	    '#type' => 'value',
			'#value' => $currentnode_type,
		);
	  $form['save_setting'] = array(
	    '#type' => 'submit',
	    '#value' => t('Save setting'),
	    '#submit' => array('_nhpd_oa2_task_list_visible_fields_filters_render_form_submit'),
	  );
	}
	return $form;
}

function _nhpd_oa2_task_list_visible_fields_filters_render_form_submit(&$form, &$form_state){
  global $user;
  //drupal_set_message(nhpid_api_dump($form_state, 'purple', TRUE));
	$nid = $form_state['values']['nid'];
	$setting = array('fields' => $form_state['values']['fields'],
										'filters' => $form_state['values']['filters'],
										'nid' => $nid,
										'type' => $form_state['values']['type'],);
	$task_list_setting = isset($user -> data['task_list_setting'])? $user -> data['task_list_setting'] : array();
  $task_list_setting[$nid] = $setting;
	$user -> data['task_list_setting'] = $task_list_setting;
	user_save($user);
	$form_state['rebuild'] = TRUE;
}