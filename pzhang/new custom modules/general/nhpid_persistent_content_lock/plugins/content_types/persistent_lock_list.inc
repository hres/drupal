<?php

$plugin = array(
  'single' => TRUE,
  'title' => t('Persistent locked contents'),
  'description' => t(''),
  'category' => t('Custom Panes'),
  'edit form' => 'nhpid_persistent_content_lock_persistent_lock_list_edit_form',
  'render callback' => 'nhpid_persistent_content_lock_persistent_lock_list_render',
  'admin info' => 'nhpid_persistent_content_lock_persistent_lock_list_info',
  'defaults' => array('description' => t('To show persistent locked list by user'),
      								'title' => 'Persistent Locked Contents',),
  'all contexts' => TRUE,
);

function nhpid_persistent_content_lock_persistent_lock_list_edit_form($form, &$form_state) {
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
function nhpid_persistent_content_lock_persistent_lock_list_edit_form_submit($form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

function nhpid_persistent_content_lock_persistent_lock_list_render($subtype, $conf, $args, $contexts) {
  $block = new stdClass();
  $block->content = nhpid_persistent_content_lock_overview();
  return $block;
}

function nhpid_persistent_content_lock_persistent_lock_list_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass;
    $block->title = $conf['override_title'] ? $conf['override_title_text'] : '';
    return $block;
  }
}

