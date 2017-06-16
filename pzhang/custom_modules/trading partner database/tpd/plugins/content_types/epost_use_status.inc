<?php

$plugin = array(
  'single' => TRUE,
  'title' => t('ePost use status'),
  'description' => t('Shows field field_epost_application value'),
  'category' => t('Custom Panes'),
  'edit form' => 'tpd_epost_use_status_edit_form',
  'render callback' => 'tpd_epost_use_status_render',
  'admin info' => 'tpd_epost_use_status_info',
  'defaults' => array('description' => t('Shows field field_epost_application value'),
      								'title' => t('ePost use status'),
											'active_text_color' => 'green',
											'inactive_text_color' => 'red',
											'size' => 'x-large',
											'weight' => 'bold',
											'style' => 'normal',
											'alignment' => 'center',
											
											),
  'all contexts' => TRUE,
);
function tpd_epost_use_status_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];
  $form['description'] = array(
    '#title' => t('Description'),
    '#description' => t('Description (administrative)'),
    '#type' => 'textfield',
    '#required' => FALSE,
    '#default_value' => $conf['description'],
  );
	$form['active_text_color'] = array(
    '#title' => t('Active text color'),
    '#description' => t('Active text color'),
    '#type' => 'textfield',
		'#default_value' => isset($conf['active_text_color']) ? $conf['active_text_color'] : 'green',
    '#required' => FALSE,
  );
	$form['inactive_text_color'] = array(
    '#title' => t('Inactive text color'),
    '#description' => t('Inactive text color'),
    '#type' => 'textfield',
		'#default_value' => isset($conf['inactive_text_color']) ? $conf['inactive_text_color'] : 'red',
    '#required' => FALSE,
  );
	$form['size'] = array(
    '#title' => t('Size'),
    '#type' => 'select',
    '#default_value' => isset($conf['size']) ? $conf['size'] : 'x-large',
    '#options' => array(
      '' => '',
      'xx-small' => t('XX-Small'),
      'x-small' => t('X-Small'),
      'small' => t('Small'),
      'medium' => t('Medium'),
      'large' => t('Large'),
      'x-large' => t('X-Large'),
      'xx-large' => t('XX-Large'),
    ),
  );
	$form['weight'] = array(
    '#title' => t('Weight'),
    '#type' => 'select',
    '#default_value' => isset($conf['weight']) ? $conf['weight'] : 'bold',
    '#options' => array(
      '' => '',
      'normal' => t('Normal'),
      'bold' => t('Bold'),
      'bolder' => t('Bolder'),
      'lighter' => t('Lighter'),
    ),
  );

  // style
  $form['style'] = array(
    '#title' => t('Style'),
    '#type' => 'select',
    '#default_value' => isset($conf['style']) ? $conf['style'] : 'normal',
    '#options' => array(
      '' => '',
      'normal' => t('Normal'),
      'italic' => t('Italic'),
      'oblique' => t('Oblique'),
    ),
  );
	$form['alignment'] = array(
    '#title' => t('Align'),
    '#type' => 'select',
    '#default_value' => isset($conf['alignment']) ? $conf['alignment'] : 'center',
    '#options' => array(
      '' => '',
      'justify' => t('Justify'),
      'left' => t('Left'),
      'right' => t('Right'),
      'center' => t('Center'),
    ),
  );

  return $form;
}
function tpd_epost_use_status_edit_form_submit($form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

function tpd_epost_use_status_render($subtype, $conf, $args, $contexts) {
  $block = new stdClass();

  // initial content is blank
  $block->title = t('ePost use statue'); // This will be overridden by the user within the panel options.
	$currentnode_nid = currentnode_nid();
	if (!$currentnode_nid) $currentnode_nid = $args[0];
	$node = node_load($currentnode_nid);
	//drupal_set_message(nhpid_api_dump($node, 'red', TRUE));
	
	$node_wrapper = entity_metadata_wrapper('node', $node);
	$value = $node_wrapper -> field_epost_application -> value();
		
	$css = '';
	if ($conf['size']) $css .= 'font-size:' . $conf['size'] .';';
	if ($conf['weight']) $css .= 'font-weight:'. $conf['weight'] .';';
	if ($conf['style']) $css .= 'font-style:' . $conf['style'] .';';
	if ($conf['alignment']) $css .= 'text-align:'. $conf['alignment'] .';';
	if ($value == 'Active'){
		$css .= 'color:' . $conf['active_text_color'] .';';
	}
	elseif ($value == 'Not active'){
		$css .= 'color:' . $conf['inactive_text_color'] .';';
	}
	
	$form['value'] = array(
		'#type' => 'item',
		'#markup' => "<div style='$css'>  $value </div>",
	);
	$content = render($form);
  //$content = 'example content';
	$block->content = $content;
  
  return $block;
}

function tpd_epost_use_status_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass;
    $block->title = $conf['override_title'] ? $conf['override_title_text'] : '';
    return $block;
  }
}

function _tpd_epost_use_status_render_form($form, &$form_state){
	global $user;
	$currentnode_nid = currentnode_nid();
	$node = node_load($currentnode_nid);
	
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
		
	}
	return $form;
}
