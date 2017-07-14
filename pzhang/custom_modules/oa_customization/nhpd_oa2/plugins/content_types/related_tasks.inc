<?php

$plugin = array(
  'single' => TRUE,
  'title' => t('Related tasks'),
  'description' => t(''),
  'category' => t('Custom Panes'),
  'edit form' => 'nhpd_oa2_related_tasks_edit_form',
  'render callback' => 'nhpd_oa2_related_tasks_render',
  'admin info' => 'nhpd_oa2_related_tasks_info',
  'defaults' => array('description' => t('To show related tasks information'),
      								'title' => 'Related tasks',),
  'all contexts' => TRUE,
);

function nhpd_oa2_related_tasks_edit_form($form, &$form_state) {
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
function nhpd_oa2_related_tasks_edit_form_submit($form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

function nhpd_oa2_related_tasks_render($subtype, $conf, $args, $contexts) {
  $block = new stdClass();

  $variable_name = 'oa_worktracker_allowed_values_related_task_relation';
	$related_task_relation=variable_get_value($variable_name);
  $block->title = $conf['title'];
	$related_tasks = nhpd_oa2_get_reverse_related_tasks() + nhpd_oa2_get_related_tasks();
	$relateds = array();
	foreach($related_tasks as $nid => $value){
		$relation = isset($value['relation']) ? $related_task_relation['options'][$value['relation']] : '';
		$title = '';
		$nid = '';
		if ($value['entity']){
			$title = $value['entity'] -> title;
			$nid = $value['entity'] -> nid;
		}
		$relateds[] = "$relation " .  l($title, 'node/' . $nid, array('attributes' => array('target' =>'_blank')));
	}
  if (!empty($relateds)){
    $block->content = theme_item_list(array('type' => 'ol', 'items' => $relateds, 'title' => null, 'attributes' => array()));
	}
	
  return $block;
}

function nhpd_oa2_related_tasks_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass;
    $block->title = $conf['override_title'] ? $conf['override_title_text'] : '';
    return $block;
  }
}

