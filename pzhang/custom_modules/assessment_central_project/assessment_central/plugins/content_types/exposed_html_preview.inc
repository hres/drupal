<?php
### style sheets from NHPID
global $nhpid_format_header;
global $nhpid_format_footer;
global $base_url;
$path = drupal_get_path('module', 'assessment_central');
//drupal_set_message(nhpid_api_dump("$base_url/$path", 'red', TRUE));
$nhpid_format_header = <<<EOD
	<link href="$base_url/$path/css/nhpid/base.css" media="screen, print" rel="stylesheet" type="text/css" />
	<link href="$base_url/$path/css/nhpid/base-institution.css" media="screen, print" rel="stylesheet" type="text/css" />
	<link href="$base_url/$path/css/nhpid/dhp-mps.css" media="screen, print" rel="stylesheet" type="text/css" />
  <style>
  body{background-color:white;}
  #edit-export-preview a {text-decoration:underline;}
  #edit-assessment-central-html {background-color: rgb(221, 221, 221);}
  #edit-assessment-central-html legend {background:rgba(102, 51, 102, 0.3);}

  </style>
  <div class="page"><div class="core">

	<div class="center">
		<h1>Natural Health Products Ingredients Database</h1> 
EOD;
		
$nhpid_format_footer = <<<EOD
	</div>

	<div class="footer">
		<div class="footerline" ></div>	
 
	</div>
	
	</div>
EOD;
$plugin = array(
  'single' => TRUE,
  'title' => t('Exposed HTML format preview'),
  'description' => t('HTML format when exposed to NHPID.'),
  'category' => t('Custom Panes'),
  'edit form' => 'assessment_central_exposed_html_preview_edit_form',
  'render callback' => 'assessment_central_exposed_html_preview_render',
  'admin info' => 'assessment_central_exposed_html_preview_info',
  'defaults' => array('description' => t('To display preview of HTML format when exposed to NHPID'),
      								'title' => t('Exposed HTML format preview'),),
  'all contexts' => TRUE,
);
function assessment_central_exposed_html_preview_edit_form($form, &$form_state) {
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
function assessment_central_exposed_html_preview_edit_form_submit($form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

function assessment_central_exposed_html_preview_render($subtype, $conf, $args, $contexts) {
  $block = new stdClass();

  // initial content is blank
  $block->title = t('Exported HTML view'); // This will be overridden by the user within the panel options.
	$content = drupal_render(drupal_get_form('_assessment_central_exposed_html_preview_render_form'));
  //$content = 'example content';
	$block->content = $content;
  //drupal_set_message(nhpid_api_dump($content, 'red', TRUE));
  return $block;
}

function assessment_central_exposed_html_preview_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass;
    $block->title = $conf['override_title'] ? $conf['override_title_text'] : '';
    return $block;
  }
}

function _assessment_central_exposed_html_preview_render_form($form, &$form_state){
	global $user;
	global $nhpid_format_header;
  global $nhpid_format_footer;
	$currentnode_nid = currentnode_nid();
	$node = node_load($currentnode_nid);
	if ($currentnode_nid and ($node -> type == 'mi' or $node -> type == 'combo')){
		//$assessment_central_html = assessment_central_format_export_info($node, null, false);
		$assessment_central_html_e = assessment_central_format_export_info($node, null, true, $assessment_central_html_info, 'e');
		
		$form['export_preview'] = array(
      '#weight' => 1,
      '#type' =>'fieldset',
      //'#title' => '',
      '#collapsible' => FALSE, 
      '#collapsed' => FALSE,
    );
	  $form['export_preview']['export_preview_html'] = array(
      '#markup' => $nhpid_format_header . $assessment_central_html_e . $nhpid_format_footer,
      '#type' =>'item',
    );
	}
	return $form;
}
