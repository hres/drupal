<?php
$url ='https://drupal.hres.ca/md/export';
// Fetches data
$post_fields = array('data_format'=>'json', 'name'=>'exporter', 'password' => 'exporter');
$curl = curl_init($url);
curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
curl_setopt($curl, CURLOPT_POST, true);
curl_setopt($curl, CURLOPT_POSTFIELDS, $post_fields);
curl_setopt($curl, CURLOPT_SSL_VERIFYPEER, false);
curl_setopt($curl, CURLOPT_TIMEOUT, 10);
curl_setopt($curl, CURLOPT_CONNECTTIMEOUT, 10);
$curl_response = curl_exec($curl);
dpm($curl_response);
//dpm(curl_error($curl));
// Feeds back data processing result
$data = drupal_json_decode($curl_response);
//dpm($data);
$dhpid_response = array();
foreach($data as $inspection){
	 $response = 'Data processing failed.';
	 if (isset($inspection['InitialInspections'])){
		$initialInspections = $inspection['InitialInspections'];
		$drupal_nid = $initialInspections['drupal_nid'];
		if (!isset($dhpid_response[$drupal_nid])){
			$dhpid_response[$drupal_nid] = array();
		}
		 //processes data

		if ($drupal_nid & 1){ // $drupal_nid is odd
			$dhpid_response[$drupal_nid][] = "Initial Report Regulation English can't be null";
			$dhpid_response[$drupal_nid][] = "Initial Report summary English can't be null";
			$dhpid_response[$drupal_nid][] = "Initial Report Regulation French can't be null";
			$dhpid_response[$drupal_nid][] = "Initial Report summary French can't be null";
		}
		
	 }
	 if (isset($inspection['Inspections'])){
		$initialInspections = $inspection['Inspections'];
		$drupal_nid = $initialInspections['drupal_nid'];
		if (!isset($dhpid_response[$drupal_nid])){
			$dhpid_response[$drupal_nid] = array();
		}
		 //processes data
		if ($drupal_nid & 1){ // $drupal_nid is odd
			$dhpid_response[$drupal_nid][] = "Activity French can't be null";
			$dhpid_response[$drupal_nid][] = "Street can't be null";
			$dhpid_response[$drupal_nid][] = "Rating can't be null";
			$dhpid_response[$drupal_nid][] = "Inspection End date can't be null since it is In progress";
		}
		
	 }
	 if (isset($inspection['ReportSummaryInspection'])){
		$initialInspections = $inspection['Inspections'];
		$drupal_nid = $initialInspections['drupal_nid'];
		if (!isset($dhpid_response[$drupal_nid])){
			$dhpid_response[$drupal_nid] = array();
		}

		 //processes data
		 if ($drupal_nid & 1){ // $drupal_nid is odd
			$dhpid_response[$drupal_nid][] = "Report Card Observations can't be null";
			$dhpid_response[$drupal_nid][] = "Report Card Regulation English can't be null";
			$dhpid_response[$drupal_nid][] = "Report Card summary English  can't be null";
			$dhpid_response[$drupal_nid][] = "Report Card measurestaken can't be null";
		}
		
	 }
}
dpm($dhpid_response);
$dhpid_response_json = drupal_json_encode($dhpid_response);
$post_fields = array( 'name'=>'exporter', 'password' => 'exporter', 'dhpid_response' => $dhpid_response_json, 'update_workflow_state' => 0);
curl_setopt($curl, CURLOPT_POSTFIELDS, $post_fields);
$curl_response = curl_exec($curl);
dpm($curl_response);
dpm(curl_error($curl));