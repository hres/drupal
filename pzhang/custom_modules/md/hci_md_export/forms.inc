<?php
function hc_irc_export_manual_export_form($form, &$form_state){
	$workflow_setting = variable_get(hci_md_WORKFLOW_TRANSITION, array());
	
	$form['#tree']=TRUE;
	$form['iid'] = array(
		'#type' => 'fieldset',
		'#title' => t('IID'),
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
	);
	$to_be_published_status = $workflow_setting['gmp_iid']['to_be_published_status'];
	$query = new EntityFieldQuery();
	$workflow_field = hci_md_WORKFLOW_FIELD;
	$query->entityCondition('entity_type', 'node')
		  ->entityCondition('bundle', 'gmp_iid')
		  ->propertyCondition('status', NODE_PUBLISHED)
		  ->fieldCondition($workflow_field, 'value', $to_be_published_status, '=');
	$result = $query->execute();
	$header = array('Establishment Name', 'Inspection End Date', 'IID Post Due Date', 'Inspector');
	$rows = array();
	if (isset($result['node'])) {
		$inspection_nids = array_keys($result['node']);
		foreach($inspection_nids as $inspection_nid){
			$inspection_data = hci_md_irc_inpection_node_data(node_load($inspection_nid));
			$row = array($inspection_data['establishment_name'], $inspection_data['inspection_end_date'],$inspection_data['iid_post_due_date'],$inspection_data['author'],);
			$rows[$inspection_nid] = $row;
		}
		$form['iid']['selected'] = array(
			'#type' => 'tableselect',
			'#header' => $header,
			'#options' => $rows,
			'#required' => TRUE,
			'#multiple' => True,
			'#empty' => t('No content available.'),
		);
		$form['iid']['server'] = array(
			'#title' => 'Server',
			'#type' => 'radios',
			'#options' => array('staging'=>'Staging','production' => 'Production'),
			'#required' => TRUE,
			//'#multiple' => FALSE,
	
		);
		$form['iid']['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Submit'),
			'#name' => 'iid-submit',
			'#submit' => array('hci_md_export_manual_export_form_submit'),
			'#limit_validation_errors' => array(array('iid')),
		);
	}
	$form['irc'] = array(
		'#type' => 'fieldset',
		'#title' => t('IRC'),
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
	);
	$to_be_published_status = $workflow_setting['gmp_irc']['to_be_published_status'];
	$query = new EntityFieldQuery();
	$workflow_field = hci_md_WORKFLOW_FIELD;
	$query->entityCondition('entity_type', 'node')
		  ->entityCondition('bundle', 'gmp_irc')
		  ->propertyCondition('status', NODE_PUBLISHED)
		  ->fieldCondition($workflow_field, 'value', $to_be_published_status, '=');
	$result = $query->execute();
	$header = array('establishment_name' =>'Establishment Name', 'inspection_end_date' => 'Inspection End Date', 'final_post_date' => 'Final Post Due Date', 'inspector' =>'Inspector');
	$rows = array();
	if (isset($result['node'])) {
		$inspection_nids = array_keys($result['node']);
		foreach($inspection_nids as $inspection_nid){
			$inspection_data = hci_md_irc_inpection_node_data(node_load($inspection_nid));
			$row = array('establishment_name' =>$inspection_data['establishment_name'], 'inspection_end_date'=>$inspection_data['inspection_end_date'],'final_post_date' => $inspection_data['final_post_due_date'],'inspector' =>$inspection_data['author'],);
			$rows[$inspection_nid] = $row;
		}
		$form['irc']['selected'] = array(
			'#type' => 'tableselect',
			'#header' => $header,
			'#options' => $rows,
			'#required' => TRUE,
			'#multiple' => True,
			'#empty' => t('No content available.'),
		);
		$form['irc']['server'] = array(
			'#title' => 'Server',
			'#type' => 'radios',
			'#options' => array('staging'=>'Staging','production' => 'Production'),
			'#required' => TRUE,
			//'#multiple' => FALSE,
	
		);
		$form['irc']['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Submit'),
			'#name' => 'irc-submit',
			'#submit' => array('hci_md_export_manual_export_form_submit'),
			'#limit_validation_errors' => array(array('irc')),
		);
	}
	return $form;
}

function hci_md_export_manual_export_form_submit(&$form, &$form_state){
	//drupal_set_message(hci_md_dump($form_state, 'red', TRUE));
	
	if (isset($form_state['values']['iid'])){
		$server = $form_state['values']['iid']['server'];
		$nids = $form_state['values']['iid']['selected'];
	}
	elseif (isset($form_state['values']['irc'])){
		$server = $form_state['values']['irc']['server'];
		$nids = $form_state['values']['irc']['selected'];
	}
	hci_md_export_export($server, $nids);
	$form_state['rebuild'] = TRUE;
}

function hci_md_export_setting_form($form, &$form_state){
	$setting = variable_get('hci_md_export_setting', array('cron_is_on' => 0, 'server'=>'production', 'rest_url_staging' => null, 'rest_url_production' => null,));
	$form['rest_url'] = array(
		'#type' => 'fieldset',
		'#title' => t('REST URL'),
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
	);
	$form['rest_url']['staging'] = array(
		'#type' => 'textfield',
		'#title' => t('HC Inspection Staging Server Rest URL'),
		'#default_value' => $setting['rest_url_staging'],
	);
	$form['rest_url']['production'] = array(
		'#type' => 'textfield',
		'#title' => t('HC Inspection Staging Server Rest URL'),
		'#default_value' => $setting['rest_url_production'],
	);
	$form['cron'] = array(
		'#type' => 'fieldset',
		'#title' => t('Cron Setting'),
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
	);
	$form['cron']['cron_is_on'] = array(
		'#type' => 'radios',
		'#title' => t('Turn on cron'),
		'#options' => array(0 => t('Off'), 1 => t('On')),
		'#default_value' => $setting['cron_is_on'],
	);
	$form['cron']['server'] = array(
			'#title' => 'Server',
			'#type' => 'radios',
			'#options' => array('staging'=>'Staging','production' => 'Production', 'both' => 'Both'),
			'#default_value' => $setting['server'],
			'#required' => TRUE,
			//'#multiple' => FALSE,
	
		);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit'),
		'#name' => 'query',
		'#submit' => array('hci_md_export_setting_form_submit'),
	);
	return $form;
}
function hci_md_export_log_clear_form($form, &$form_state){

	$older_than = 365;
	
	$form['older_than']=array(
		'#title' => t('Remove records older than'),
		'#type' => 'select',
		'#options' => array(1=>'1 day', 2=>'2 days', 7=>'A week', 31=>'A month', 365 => 'A year', 0=>'All'),
		'#default_value' => $older_than,
	);
	$form['clear_log'] = array(
		'#type' => 'submit',
		'#value' => t('Clear log'),
		'#name' => 'clear_log',
		'#submit' => array('hci_md_export_log_clear_log'),
		'#attributes' => array('onclick' => "return confirm('You are going to remove data migration logs. Continue?');"),
	);
	return $form;
}

function hci_md_export_log_form($form, &$form_state){
	//dpm($form_state);
	$form['#method'] = 'get';
	$days_ago = isset($_GET['days_ago']) ? $_GET['days_ago'] : (isset($form_state['values']['days_ago']) ? $form_state['values']['days_ago'] : 7);
	$form['days_ago']=array(
		'#title' => t('Days ago'),
		'#type' => 'select',
		'#options' => array(1=>'1 day', 2=>'2 days', 7=>'A week', 31=>'A month', 0=>'No limit'),
		'#default_value' => $days_ago,
	);
	$error_only = isset($_GET['error_only']) ? $_GET['error_only'] : (isset($form_state['values']['error_only']) ? $form_state['values']['error_only'] : FALSE);
	//$error_only = isset($_GET['error_only']) ? $_GET['error_only'] : TRUE;
	$form['error_only']=array(
		'#title' => t('Error log only'),
		'#type' => 'checkbox',
		'#default_value' => $error_only,
	);
	$lines_per_page = isset($_GET['lines_per_page']) ? $_GET['lines_per_page'] : (isset($form_state['values']['lines_per_page']) ? $form_state['values']['lines_per_page'] : 20);
	//dpm($lines_per_page);
	$form['lines_per_page']=array(
		'#title' => t('Records per page'),
		'#type' => 'select',
		'#options' => array(10=>10, 20=>20, 50=>50, 100=>100, 500=>500),
		'#default_value' => $lines_per_page,
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Refresh'),
		'#name' => 'query',
		'#submit' => array('hci_md_export_log_form_submit'),
	);
	$summary_header = array(
		'export_date' =>  t('Date'), 'successful_inspections' =>t('Exported Inspections'), 'failed_inspections' => t('Failed Inspections'),
	);
	//dpm($summary_header);
	
	$summary_query = db_select('hci_md_export_log', 'l')
			-> fields('l')
			->orderBy('l.export_date', 'DESC');
	$summary_result = $summary_query->execute();
	$summary_rows = array();
	if ($summary_result){
		while($rc =$summary_result -> fetchObject()){
			$summary_date = format_date($rc -> export_date, 'custom', 'Y-m-d');
			if (!array_key_exists($summary_date, $summary_rows)){
				$summary_rows[$summary_date]['export_date']=$summary_date;
				$summary_rows[$summary_date]['successful_inspections']=0;
				$summary_rows[$summary_date]['failed_inspections']=0;
				
			}
			//$summary_rows[$summary_date]['export_date']['data']=$summary_date;
			$error = $rc -> error;
			if ($error){
				$summary_rows[$summary_date]['failed_inspections']++;
			}
			else{
				$summary_rows[$summary_date]['successful_inspections']++;
			}
		}
		
	}
	//dpm($summary_rows);
	$header = array(
		'export_date' => array('data' => t('Date'), 'field' => 'l.export_date', 'sort' =>'desc'), 'inspection_nid' => array('data' =>t('Inspection Nid'), 'field' => 'l.inspection_nid'), 'inspection_type' => array('data' =>t('Inspection Type'), 'field' => 'l.inspection_type'),
		'log' => array('data' => t('Log'), 'field' => 'log'),
	);
	$query = db_select('hci_md_export_log', 'l')
			-> fields('l');
	if ($days_ago > 0){
		$time = time() - $days_ago * 24 * 3600;
		$query -> condition('export_date', $time, '>=');
	}
	if ($error_only){
		$query -> condition('error', $error_only, '=');
	}
	
	$query = $query->extend('TableSort')->orderByHeader($header)->extend('PagerDefault')->limit($lines_per_page);
	$result = $query->execute();
	$rows = array();
	
	if ($result){
		while($rc =$result -> fetchObject()){
			$date = format_date($rc -> export_date, 'custom', 'Y-m-d H:i:s');
			$error = $rc -> error;
			$log = $rc -> log;
			$inspection_nid = $rc -> inspection_nid;
			$inspection_type = $rc -> inspection_type;
			$row = array(
				'export_date' => array('data' => $date),
				'inspection_nid' => array('data' => l($inspection_nid, "node/$inspection_nid")),
				'inspection_type' => array('data' => $inspection_type),
				'log' => array('data' => $log, 'style'=>$error? 'color:red;':'color:green',),
			);
			$rows[] = $row;
		}
	}
	//dpm($rows);
	
	$last_export_date = format_date(variable_get('hci_md_export_last_publication_time', null), 'custom', 'Y-m-d H:i:s');
	$form['last_export_date']=array(
		'#type' => 'item',
		'#title' => t('The date of last export'),
		'#markup' => $last_export_date,
	);
	$form['summary_table'] = array(
      '#theme' => 'table',
      '#header' => $summary_header,
      '#rows' => $summary_rows,
      '#empty' => t('No Result.'),
	  '#caption' => t('Summary'),
    );
	
	$form['table'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#empty' => t('No Result.'),
	  '#caption' => t('Log'),
    );

    // ADD PAGER
	
    $form['pager'] = array('#markup' => theme('pager',  array( 'parameters'=>array('error_only' => $error_only, 'lines_per_page' => $lines_per_page, 'days_ago' =>$days_ago),)));
	return $form;
}
function hci_md_export_log_form_submit($form, &$form_state){
	$form_state['lines_per_page'] = $form_state['values']['lines_per_page'];
	$form_state['days_ago'] = $form_state['values']['days_ago'];
	$form_state['error_only'] = $form_state['values']['error_only'];
	//$form_state['cache'] = FALSE;
	$form_state['rebuild'] = TRUE;
}

function hci_md_export_log_clear_log($form, &$form_state){
	$older_than = isset($_GET['older_than']) ? $_GET['older_than'] : (isset($form_state['values']['older_than']) ? $form_state['values']['older_than'] : 365);
	$time = time() - $older_than * 24 * 365;
	$num = db_delete('hci_md_export_log')->condition('export_date', $time, '<')->execute();
	$form_state['cache'] = FALSE;
	$form_state['rebuild'] = TRUE;
	drupal_set_message(t('!num records have been removed', array('!num'=> $num)));
}
function hci_md_export_setting_form_submit($form, &$form_state){
	//drupal_set_message(hci_md_dump($form_state, 'red', TRUE));
	$cron_on = $form_state['values']['cron_is_on'];
	$server = $form_state['values']['server'];
	$rest_url_staging = $form_state['values']['rest_url_staging'];
	$rest_url_production = $form_state['values']['rest_url_production'];
	variable_set('hci_md_export_setting', array('cron_is_on' => $cron_on, 'server'=>$server, 'rest_url_staging' => $rest_url_staging, 'rest_url_production'=>$rest_url_production));
	$form_state['rebuild'] = TRUE;
}
