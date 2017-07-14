<?php

$plugin = array(
  'single' => TRUE,
  'title' => t('Sibing tasks'),
  'description' => t(''),
  'category' => t('Custom Panes'),
  'edit form' => 'nhpd_oa2_sibling_tasks_edit_form',
  'render callback' => 'nhpd_oa2_sibling_tasks_render',
  'admin info' => 'nhpd_oa2_sibling_tasks_info',
  'defaults' => array('description' => t('To show info sibling tasks'),
      								'title' => 'Sibling tasks',),
  'all contexts' => TRUE,
);

function nhpd_oa2_sibling_tasks_edit_form($form, &$form_state) {
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
function nhpd_oa2_sibling_tasks_edit_form_submit($form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

function nhpd_oa2_sibling_tasks_render($subtype, $conf, $args, $contexts) {
  $block = new stdClass();

  // initial content is blank
  $block->title = $conf['title'];
	$sibling_tasks = nhpd_oa2_get_all_sibing_tasks();
	$siblings = array();
	foreach($sibling_tasks as $nid => $task_node){
		$siblings[] = l($task_node -> title, 'node/' . $task_node ->nid, array('attributes' => array('target' =>'_blank')));
	}
  if (!empty($siblings)){
    $block->content = theme_item_list(array('type' => 'ol', 'items' => $siblings, 'title' => null, 'attributes' => array()));
	}
	
  return $block;
}

function nhpd_oa2_sibling_tasks_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass;
    $block->title = $conf['override_title'] ? $conf['override_title_text'] : '';
    return $block;
  }
}

