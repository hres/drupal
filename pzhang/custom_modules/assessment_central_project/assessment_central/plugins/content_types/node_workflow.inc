<?php

$plugin = array(
  'single' => TRUE,
  'title' => t('Node current workflow state and workflow change form'),
  'description' => t('Shows current node workflow state and workflow change form'),
  'category' => t('Custom Panes'),
  'edit form' => 'assessment_central_node_workflow_edit_form',
  'render callback' => 'assessment_central_node_workflow_render',
  'admin info' => 'assessment_central_node_workflow_info',
  'defaults' => array('description' => t('Shows current node workflow state and workflow change form'),
      								'title' => t('Node current workflow state and workflow change form'),),
  'all contexts' => TRUE,
);
function assessment_central_node_workflow_edit_form($form, &$form_state) {
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
function assessment_central_node_workflow_edit_form_submit($form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

function assessment_central_node_workflow_render($subtype, $conf, $args, $contexts) {
  $block = new stdClass();

  // initial content is blank
	$currentnode_nid = currentnode_nid();
	if (!$currentnode_nid) $currentnode_nid = $args[0];
  $block->title = t('Node current workflow state and workflow change form'); // This will be overridden by the user within the panel options.
	$content = drupal_render(drupal_get_form('_assessment_central_node_workflow_render_form', $currentnode_nid));
  //$content = 'example content';
	$block->content = $content;
  //drupal_set_message(nhpid_api_dump($content, 'red', TRUE));
  return $block;
}

function assessment_central_node_workflow_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass;
    $block->title = $conf['override_title'] ? $conf['override_title_text'] : '';
    return $block;
  }
}

function _assessment_central_node_workflow_render_form($form, &$form_state, $currentnode_nid){
	global $user;
	form_load_include($form_state, 'inc', 'workflow', 'workflow.pages');
	$node = node_load($currentnode_nid);
	$workflow = workflow_get_workflow_type_map_by_type($node->type);
  $states = array();
  $state_system_names = array();
  foreach (workflow_get_workflow_states() as $data) {
    $states[$data->sid] = check_plain(t($data->state));
    $state_system_names[$data->sid] = check_plain($data->state);
  }
	if ($currentnode_nid and $workflow){
		
		$current = workflow_node_current_state($node);
		$output = theme('workflow_current_state', array('state' => $states[$current], 'state_system_name' => $state_system_names[$current], 'sid' => $current));
		$form['current_state'] = array(
      '#type' =>'item',
      '#title' => t('Current state'),
			'#markup' => $output,
    );
		//drupal_set_message(nhpid_api_dump($node, 'red', TRUE));

		$edit_form = workflow_tab_form($form, $form_state, $node, $workflow->wid, $states, $current);
		$form['change_form'] = array(
			'#title' => t('Change workflow state'),
			'#type' => 'fieldset', 
			'#collapsible' => TRUE, 
			'#collapsed' => True,
			'#post_render' => array('_wolkflow_change_form_post_render'),
			'#access' => user_access('schedule workflow transitions'),
		);
	  $form['change_form']['change_form'] = $edit_form;
	}
	return $form;
}

function _wolkflow_change_form_post_render($content, $elemet){
	$dom = new DOMDocument;
	@$dom->loadHTML($content);
	$legends = &$dom->getElementsByTagName('legend');
	foreach($legends as $legend){
		$legend ->setAttribute("style", "background-color:white");
	}
	//drupal_set_message(nhpid_api_dump($legend, 'red', TRUE));
	return $dom->saveHTML();
}