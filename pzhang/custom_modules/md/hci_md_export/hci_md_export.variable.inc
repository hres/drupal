<?php


// Implementation of hook_variable_group_info
function hci_md_export_variable_group_info() {
  $groups['hc_inspection'] = array(
    'title' => t('HC Inspection'), 
    'description' => t('HC inspection variables'), 
    'access' => 'administer site configuration', 
    'path' => array('admin/hc_inspection/hci_md_export/hc_inspection_variables', ),
  );
  $groups['hc_inspection_export_log'] = array(
    'title' => t('HC Inspection Export Log'), 
    'description' => t('HC inspection export log variables'), 
    'access' => 'export hci_md data setting', 
    'path' => array('admin/hc_inspection/hci_md_export/hc_inspection_export_log_variables', 'hci_md_export/export_log_setting'),
  );
  return $groups;
}

// Implementation of hook_variable_info
function hci_md_export_variable_info($options){
	$description  = t('The possible values this field can contain.');
	$variable = array();
	$iid_workflow = workflow_get_workflows_by_type('iid');
	$irc_workflow = workflow_get_workflows_by_type('irc');
	$iid_workflow_states = workflow_get_workflow_state_names($iid_workflow->wid);
	$irc_workflow_states = workflow_get_workflow_state_names($irc_workflow->wid);
	
	$variable['hci_md_export_iid_publication_ready_state'] = array(
		'type' => 'select',
		'title' => t('IID Publication Ready State (hci_md_export_iid_publication_ready_state'),
		'options' => $iid_workflow_states,
		'description' => t('The workflow state of IID which is ready to published to DHPID )' ),
		'group' => 'hc_inspection',
	);
	
	$variable['hci_md_export_iid_publication_published_state'] = array(
		'type' => 'select',
		'title' => t('IID Published State (hci_md_export_iid_publication_published_state)'),
		'options' => $iid_workflow_states,
		'description' => t('The workflow state of IID which published to DHPID' ),
		'group' => 'hc_inspection',
	);
	$variable['hci_md_export_irc_publication_ready_state'] = array(
		'type' => 'select',
		'title' => t('IRC Publication Ready State (hci_md_export_irc_publication_ready_state)'),
		'options' => $irc_workflow_states,
		'description' => t('The workflow state of IRC which is ready to published to DHPID' ),
		'group' => 'hc_inspection',
	);
	
	$variable['hci_md_export_irc_publication_published_state'] = array(
		'type' => 'select',
		'title' => t('IRC Published State (hci_md_export_irc_publication_published_state)'),
		'options' => $irc_workflow_states,
		'description' => t('The workflow state of IRC which is published to DHPID' ),
		'group' => 'hc_inspection',
	);
	$variable['hci_md_export_last_publication_time'] = array(
		'type' => 'number',
		'title' => t('Last data publication time(hci_md_export_last_publication_time)'),
		'description' => t('The time (Unix timestamp) of last publication to DHPID' ),
		'group' => 'hc_inspection',
	);
	//export_log
	$variable['hci_md_export_log_max_records'] = array(
		'type' => 'select',
		'title' => t('The max number of records being kept (hci_md_export_log_max_records)'),
		'options' => array(1000=>1000, 2000=>2000, 5000=>5000, 10000=>10000, 50000=>50000, 0=>'unlimited'),
		'default' => 5000,
		'description' => t('The max number of records are kept. If the number of records exceeds the setting, older ones will be deleted automatically.'),
		'group' => 'hc_inspection_export_log',
	);
	$variable['hci_md_export_log_error_only'] = array(
		'type' => 'select',
		'title' => t('Log error only (hci_md_export_log_error_only)'),
		'options' => array(0 => t('No'), 1 => t('Yes')),
		'default' => 1,
		'description' => t('Yes- logs errors only; No - logs all' ),
		'group' => 'hc_inspection_export_log',
	);
	//dpm($variable);
	return $variable;
}
