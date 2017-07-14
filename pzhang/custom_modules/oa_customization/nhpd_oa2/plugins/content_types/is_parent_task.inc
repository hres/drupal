<?php

$plugin = array(
  'single' => TRUE,
  'title' => t('Is parent task'),
  'description' => t(''),
  'category' => t('Custom Panes'),
  'edit form' => 'nhpd_oa2_is_parent_task_edit_form',
  'render callback' => 'nhpd_oa2_is_parent_task_render',
  'admin info' => 'nhpd_oa2_is_parent_task_info',
  'defaults' => array('description' => t('To show info "This is a parent task"'),
      								'title' => 'Is parent task',),
  'all contexts' => TRUE,
);

function nhpd_oa2_is_parent_task_edit_form($form, &$form_state) {
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
function nhpd_oa2_is_parent_task_edit_form_submit($form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

function nhpd_oa2_is_parent_task_render($subtype, $conf, $args, $contexts) {
  $block = new stdClass();

  // initial content is blank
	$is_a_parent_task = nhpd_oa2_is_parent_task();
  if ($is_a_parent_task){
    $block->content = t('This is a parent task');
	}
	
  return $block;
}

function nhpd_oa2_is_parent_task_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass;
    $block->title = $conf['override_title'] ? $conf['override_title_text'] : '';
    return $block;
  }
}

