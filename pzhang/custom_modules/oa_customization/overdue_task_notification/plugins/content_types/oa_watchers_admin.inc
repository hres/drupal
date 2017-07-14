<?php

$plugin = array(
  'single' => TRUE,
  'title' => t('Watchers (non-members only)'),
  'description' => t(''),
  'category' => t('Custom Panes'),
  'edit form' => 'oa_watchers_oa_watchers_admin_edit_form',
  'render callback' => 'oa_watchers_oa_watchers_admin_render',
  'admin info' => 'oa_watchers_oa_watchers_admin_info',
  'defaults' => array('description' => t('To set watchers for a task, etc'),
  'title' => 'Watchers (non-members only)',),
  'all contexts' => TRUE,
);
function oa_watchers_oa_watchers_admin_edit_form($form, &$form_state) {
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
function oa_watchers_oa_watchers_admin_edit_form_edit_form_submit($form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

function oa_watchers_oa_watchers_admin_render($subtype, $conf, $args, $contexts) {
	$block = new stdClass();

	// initial content is blank
	$block->title = t('Watchers setting'); // This will be overridden by the user within the panel options.
	$form = drupal_get_form('_oa_watchers_oa_watchers_admin_render_form');
	$content = drupal_render($form);
	//$content = 'example content';
	$block->content = $content;
	//drupal_set_message(nhpid_api_dump($content, 'red', TRUE));
	return $block;
}

function oa_watchers_oa_watchers_admin_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass;
    $block->title = $conf['override_title'] ? $conf['override_title_text'] : 'OA watchers setting';
    return $block;
  }
}

function _oa_watchers_oa_watchers_admin_render_form($form, &$form_state){
	
	$currentnode_nid = (arg(0) == 'node' && is_numeric(arg(1))) ? arg(1) : NULL;
	$current_space = oa_core_get_space_context();
	$watchers =oa_watchers_get_all_watchers_for_node($currentnode_nid);
	$notify = oa_watchers_get_notify_for_node($currentnode_nid);
	$notify = is_null($notify) ? 1 : $notify; // sets $notify to true if not set yet.
	
	$all_users = entity_load('user');
	$current_node = node_load($currentnode_nid);
	if (!$current_space){
		$query = new EntityFieldQuery();
		$query->entityCondition('entity_type', 'node')
		->entityCondition('bundle', 'oa_space')
		->propertyCondition('status', NODE_PUBLISHED)
		->range(0, 1);
		$result = $query->execute();
		$current_space = array_shift((array_keys($result['node'])));
	}
	$space_users = og_get_group_members_properties(node_load($current_space), array(), 'members', 'node');
	$candidates = array();
	foreach($all_users as $uid => $a_user){ // using variable $user may be troublesome
		if ($a_user -> status == 1 && !in_array($uid, $space_users)){
      $candidates[$uid] = $a_user->realname ? $a_user->realname : $a_user -> name;
		}
	}
	$default = isset($form_state['values']['watchers']) ? $form_state['values']['watchers'] : $watchers;
	$form['#access'] = og_user_access('node', $current_space, 'administer any oa task watchers', NULL, FALSE, TRUE) || og_user_access('node', $current_space, 'administer own oa task watchers', NULL, FALSE, TRUE);
	$form['watchers'] = array(
    '#type' => 'select',
		'#size' => 60,
    '#title' => t('Watchers(non-members only)'),
    '#default_value' => $default,
    '#options' => $candidates,
		'#multiple' => true,
    '#description' => t('Select watchers.'),
	);
	  
  $form['nid'] = array(
    '#type' => 'value',
		'#value' => $currentnode_nid,
	);
	$notify_default = isset($form_state['values']['notify']) ? $form_state['values']['notify'] : $notify;
  $form['notify'] = array(
    '#type' => 'checkbox',
		'#title' => t('Notify watchers'),
    '#default_value' => $notify_default,
	);
	  $form['save_setting'] = array(
	    '#type' => 'submit',
	    '#value' => t('Save setting'),
	    '#submit' => array('_oa_watchers_oa_watchers_admin_render_form_submit'),
	  );

	return $form;
}

function _oa_watchers_oa_watchers_admin_render_form_submit(&$form, &$form_state){
	$nid = $form_state['values']['nid'];
	$watchers = array_values($form_state['values']['watchers']);
  //drupal_set_message(nhpid_api_dump($watchers, 'red', TRUE));
	$notify = $form_state['values']['notify'];
  oa_watchers_update($nid, $watchers, $notify);
	
	$form_state['rebuild'] = TRUE;
}