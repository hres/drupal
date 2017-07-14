<?php

$plugin = array(
  'single' => TRUE,
  'title' => t('Subtasks'),
  'description' => t(''),
  'category' => t('Custom Panes'),
  'edit form' => 'nhpd_oa2_subtasks_edit_form',
  'render callback' => 'nhpd_oa2_subtasks_render',
  'admin info' => 'nhpd_oa2_subtasks_info',
  'defaults' => array('description' => t('To show subtasks information'),
      								'title' => 'Subtasks',),
  'all contexts' => TRUE,
);
function nhpd_oa2_subtasks_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];
  $form['description'] = array(
    '#title' => t('Description'),
    '#description' => t(''),
    '#type' => 'textarea',
    '#required' => FALSE,
    '#default_value' => $conf['description'],
  );
  $form['title'] = array(
    '#title' => t('title'),
    '#type' => 'value',
    '#default_value' => $conf['title'],
  );
  return $form;
}
function nhpd_oa2_subtasks_edit_form_submit($form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

function nhpd_oa2_subtasks_render($subtype, $conf, $args, $contexts) {
  $block = new stdClass();
	//drupal_set_message(nhpid_api_dump($args, 'purple', TRUE));
	$subtasks =  isset($args[0]) ? nhpd_oa2_get_all_child_tasks(node_load($args[0])) : array();
	$links = array();
	foreach($subtasks as $nid => $value){
		$links[] =  l($value-> title, 'node/' . $nid, array('attributes' => array('target' =>'_blank')));
	}
	if (!empty($links)){
		$block->title = $conf['title'];
		$block->content = theme_item_list(array('type' => 'ol', 'items' => $links, 'title' => null, 'attributes' => array()));
	}
	
	//$block->content = 'This is a test';
  return $block;
}

function nhpd_oa2_subtasks_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass;
    $block->title = $conf['override_title'] ? $conf['override_title_text'] : '';
    return $block;
  }
}

