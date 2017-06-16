<?php


// Implementation of hook_variable_group_info
function hci_md_export_variable_group_info() {
  $groups['hc_inspection'] = array(
    'title' => t('HC Inspection'), 
    'description' => t('HC inspection variables'), 
    'access' => 'administer site configuration', 
    'path' => array('admin/hc_inspection/hci_md_export/hc_inspection_variables', ),
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
		'title' => t('IID Publication Ready State'),
		'options' => $iid_workflow_states,
		'description' => t('The workflow state of IID which is ready to published to DHPID (hci_md_export_iid_publication_ready_state)' ),
		'group' => 'hc_inspection',
	);
	
	$variable['hci_md_export_iid_publication_published_state'] = array(
		'type' => 'select',
		'title' => t('IID Published State'),
		'options' => $iid_workflow_states,
		'description' => t('The workflow state of IID which published to DHPID (hci_md_export_iid_publication_published_state)' ),
		'group' => 'hc_inspection',
	);
	$variable['hci_md_export_irc_publication_ready_state'] = array(
		'type' => 'select',
		'title' => t('IRC Publication Ready State'),
		'options' => $irc_workflow_states,
		'description' => t('The workflow state of IRC which is ready to published to DHPID (hci_md_export_irc_publication_ready_state)' ),
		'group' => 'hc_inspection',
	);
	
	$variable['hci_md_export_irc_publication_published_state'] = array(
		'type' => 'select',
		'title' => t('IRC Published State'),
		'options' => $irc_workflow_states,
		'description' => t('The workflow state of IRC which is published to DHPID (hci_md_export_irc_publication_published_state)' ),
		'group' => 'hc_inspection',
	);
	$variable['hci_md_export_last_publication_time'] = array(
		'type' => 'number',
		'title' => t('Last data publication time'),
		'description' => t('The time (Unix timestamp) of last publication to DHPID (hci_md_export_last_publication_time)' ),
		'group' => 'hc_inspection',
	);
	//dpm($variable);
	return $variable;
}
