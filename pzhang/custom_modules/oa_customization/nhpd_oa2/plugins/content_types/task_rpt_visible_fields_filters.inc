<?php

$plugin = array(
  'single' => TRUE,
  'title' => t('Task reporting visible custom fields and filters'),
  'description' => t(''),
  'category' => t('Custom Panes'),
  'edit form' => 'nhpd_oa2_task_rpt_visible_fields_filters_edit_form',
  'render callback' => 'nhpd_oa2_task_rpt_visible_fields_filters_render',
  'admin info' => 'nhpd_oa2_task_rpt_visible_fields_filters_info',
  'defaults' => array('description' => t('To set visible (dynamic) fields and filters for task reporting view'),
      								'title' => 'Task reporting visible custom fields and filters',),
  'all contexts' => TRUE,
);

function nhpd_oa2_task_rpt_visible_fields_filters_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];
  $form['description'] = array(
    '#title' => t('Description'),
    '#description' => t('Description (administrative)'),
    '#type' => 'textarea',
    '#required' => FALSE,
    '#default_value' => $conf['description'],
  );
  $form['title'] = array(
    '#title' => t('Description'),
    '#type' => 'value',
    '#default_value' => $conf['description'],
  );

  return $form;
}
function nhpd_oa2_task_rpt_visible_fields_filters_edit_form_submit($form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

function nhpd_oa2_task_rpt_visible_fields_filters_render($subtype, $conf, $args, $contexts) {
	$block = new stdClass();

	// initial content is blank
	$block->title = t('Task reporting custom fields/filters setting'); // This will be overridden by the user within the panel options.
	$form = drupal_get_form('_nhpd_oa2_task_reporting_visible_fields_filters_render_form');
	$content = drupal_render($form);
	//$content = 'example content';
	$block->content = $content;
	//drupal_set_message(nhpid_api_dump($content, 'red', TRUE));
	return $block;
}

function nhpd_oa2_task_rpt_visible_fields_filters_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass;
    $block->title = $conf['override_title'] ? $conf['override_title_text'] : 'Task reporting custom fields/filters setting';
    return $block;
  }
}

function _nhpd_oa2_task_reporting_visible_fields_filters_render_form($form, &$form_state){
	global $user;
  //$customizable_fields = variable_get('oa_worktracker_customizable_fields', array());
  /*foreach($customizable_fields as $customizable_field => $value){
		if (!$value){
			unset($customizable_fields[$customizable_field]);
		}
	}*/
  $view = views_get_view(TASK_LIST_VIEW_NAME, true);
  $view->set_display(TASK_REPORT_DISPLAY_ID);
	$view->init_handlers();
  $customizable_fields = variable_get('oa_worktracker_customizable_fields', array());
  $customizable_filters = variable_get('oa_worktracker_customizable_filters', array());
	$field_options = array();
	$filter_options = array();
	$view_fields = isset($view->display[$view->current_display]->display_options['fields']) ? $view->display[$view->current_display]->display_options['fields'] : $view->display['default']->display_options['fields'];
  $view_filters = isset($view->display[$view->current_display]->display_options['filters']) ? $view->display[$view->current_display]->display_options['filters'] : $view->display['default']->display_options['filters'];
 	foreach($customizable_fields as $field){
    if ($field) $field_options[$view_fields[$field]['id']] = isset($view_fields[$field]['label'])?$view_fields[$field]['label']:$view_fields[$field]['id'];
	}
  foreach($customizable_filters as $filter){
    if ($filter)$filter_options[$view_filters[$filter]['id']] = isset($view_filters[$filter]['expose']['label'])?$view_filters[$filter]['expose']['label']:$view_filters[$filter]['id'];
	}

	$form['#tree']=true;
	drupal_set_title('Reporting Setting');
	$user_data = $user ->data;
	$setting = isset($user_data['task_reporting_setting']) ? $user_data['task_reporting_setting'] : array();

	$fields = isset($setting['task_reporting']['fields']) ? $setting['task_reporting']['fields'] : array();
	$filters = isset($setting['task_reporting']['filters']) ? $setting['task_reporting']['filters'] : array();
	$fields_options = array();
	$fields_default = empty($form_states['values']['task_reporting']['fields'])? $fields : $form_states['values']['task_reporting']['fields'];
  $filters_default = empty($form_states['values']['task_reporting']['filters'])? $filters : $form_states['values']['task_reporting']['filters'];
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
    '#title' => t('Visible fields for task reporting'),
    '#default_value' => $fields_default,
    '#options' => $field_options,
		'#multiple' => true,
    '#description' => t('Select fields you want to been shown in task report.'),
	);
  $form['task_reporting']['filters'] = array(
    '#type' => 'select',
		'#size' => 60,
    '#title' => t('Visible filters for task reporting'),
    '#default_value' => $filters_default,
    '#options' => $filter_options,
		'#multiple' => true,
    '#description' => t('Select filters you want to been shown in task report.'),
	);

  $form['save_setting'] = array(
    '#type' => 'submit',
    '#value' => t('Save setting')
  );

	return $form;
}

function _nhpd_oa2_task_reporting_visible_fields_filters_render_form_submit(&$form, &$form_state){
  global $user;
  //drupal_set_message(nhpid_api_dump($form_states, 'purple', TRUE));
	$setting = array('fields' => $form_state['values']['task_reporting']['fields'], 'filters' => $form_state['values']['task_reporting']['filters']);
	$user -> data['task_reporting_setting']['task_reporting'] = $setting;
	user_save($user);
	$form_state['rebuild'] = TRUE;
}