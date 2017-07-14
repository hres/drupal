<?php

$plugin = array(
  'single' => TRUE,
  'title' => t('Revision list'),
  'description' => t('Shows revision list of nodes'),
  'category' => t('Custom Panes'),
  'edit form' => 'tpd_revision_list_edit_form',
  'render callback' => 'tpd_revision_list_render',
  'admin info' => 'tpd_revision_list_info',
  'defaults' => array('description' => t('Shows revision list of nodes'),
      								'title' => t('Revision list'),
										),
  'all contexts' => TRUE,
);
function tpd_revision_list_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];
  $form['description'] = array(
    '#title' => t('Description'),
    '#description' => t('Shows revision list of nodes (administrative)'),
    '#type' => 'textfield',
    '#required' => FALSE,
    '#default_value' => $conf['description'],
  );

  return $form;
}
function tpd_revision_list_edit_form_submit($form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

function tpd_revision_list_render($subtype, $conf, $args, $contexts) {
  $block = new stdClass();
	
  // initial content is blank
  $block->title = t('Revision list'); // This will be overridden by the user within the panel options.
	if (module_exists('diff')){
		module_load_include('inc', 'diff', 'diff.pages');
		$currentnode_nid = currentnode_nid();
		if (!$currentnode_nid) $currentnode_nid = $args[0];
		$node = node_load($currentnode_nid);
		$form = drupal_get_form('diff_node_revisions', $node);
		$content = drupal_render($form);
		//$content = 'example content';
		$block->content = $content;
		//drupal_set_message(nhpid_api_dump($node, 'red', TRUE));
		
	}
	
  
  return $block;
}

function tpd_revision_list_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass;
    $block->title = $conf['override_title'] ? $conf['override_title_text'] : '';
    return $block;
  }
}

