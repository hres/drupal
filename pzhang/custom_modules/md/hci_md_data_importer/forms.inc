<?php

function hci_md_data_importer_form($form, &$form_state, $bundle, $sheet_name){
	//drupal_set_message(hci_md_dump($bundle, 'red', TRUE));
	//drupal_set_message(hci_md_dump($sheet_name, 'red', TRUE));
	$form['#tree']=TRUE;
	$form['#attributes'] = array('enctype' => "multipart/form-data");
	/*
	$inspection_cat_nodes = node_load_multiple(array(), array('type' => 'hci_inspection_category'));
	$inspection_cats = array();
	foreach($inspection_cat_nodes as $inspection_cat_node){
		$wrapper = entity_metadata_wrapper('node', $inspection_cat_node);
		$code = $wrapper -> field_code -> value();
		$desc_en = $wrapper -> field_description_english -> value();
		$inspection_cats[$code]="$desc_en ($code)";
	}
	
	$form['inspection_category'] = array(
		'#type' => 'select',
		'#title' => t('Inspection Category'),
		'#options' => $inspection_cats,
		'#required' => TRUE,
		'#multiple' => FALSE,
		'#default_value' => 'GMP',
	);
	*/
	$form['data_file'] = array (
		'#type' => 'file', 
		'#title' => t('Establishment Excel file'),
		'#size' => 100,
		//'#required' => TRUE,
	);
	$form['sheet_name'] = array (
		'#type' => 'textfield', 
		'#title' => t('The name of the datasheet'),
		'#default_value' => $sheet_name,
		'#required' => TRUE,
	);
	$form['header_row'] = array (
		'#type' => 'textfield', 
		'#title' => t('The row number of sheet hearder'),
		'#default_value' => 2,
		'#required' => TRUE,
	);
	$form['first_data_row'] = array (
		'#type' => 'textfield', 
		'#title' => t('The row number of the first data row'),
		'#default_value' => 3,
		'#required' => TRUE,
	);
	$form['test'] = array (
		'#type' => 'checkbox', 
		'#title' => t('Testing?'),
		'#default_value' => TRUE,
	);
	$form['bundle'] = array (
		'#type' => 'value', 
		'#value' => $bundle,
	);
		
	$form['data_file_submit']=array(
    '#type' => 'submit', 
    '#value' => t('Upload'), 
    '#name' => 'data-file-submit',
    '#attributes' => array('onclick' => "return confirm('You are going to upload new $bundle data. Existing datasets will replaced. Continue?');"),
	); 
	
	return $form;
}
function hci_md_data_importer_form_submit(&$form, &$form_state){
	//drupal_set_message(hci_md_dump($form_state, 'red', TRUE));
	$form_state['rebuild'] = TRUE;
	drupal_set_message('uploading file...');
	$bundle = $form_state['values']['bundle'];
	$sheet_name = $form_state['values']['sheet_name'];
	$header_row = $form_state['values']['header_row'];
	$first_data_row = $form_state['values']['first_data_row'];
	$test = $form_state['values']['test'];
    
	$validators = array('file_validate_extensions' => array('xls xlsx'), 'file_validate_size' => array(1024*1024*10),);
    $uploaded_data_file = 'data_file';
    $uploaded_data_file_path = file_directory_temp() . "/uploaded_data_file";
    $uploaded_data_file_path =  "public://uploaded_data_file/";
     //if (!is_dir($uploaded_data_file_path)) mkdir($uploaded_data_file_path, 0777, true);
    //if (!is_dir($uploaded_data_file_split_path)) mkdir($uploaded_data_file_split_path, 0777, true);
    
    file_prepare_directory($uploaded_data_file_path, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);
    if ($file = file_save_upload($uploaded_data_file, $validators, "$uploaded_data_file_path", FILE_EXISTS_REPLACE)) {
      $file_path = $file -> uri;
      //drupal_set_message(hc_irc_dump($file, 'red', TRUE));
	  //drupal_set_message(hc_irc_dump(parse_url($file->url), 'red', TRUE));
      //drupal_set_message(hc_irc_dump(pathinfo($file->filename), 'red', TRUE));
	  $function = 'hci_md_' . $bundle . '_data_load_batch';
	  //drupal_set_message(hc_irc_dump("$function, $file, $uploaded_data_file, $uploaded_data_file_path, $file_path", 'red', TRUE));
      $function($file,$sheet_name,$header_row,$first_data_row, $test);
      
    }
    
    else {
      $msg = "File $uploaded_data_file cannot be uploaded.";
      form_set_error ('', $msg);
    }
    $form_state['storage']['values'] = $form_state['values'];
}

function hci_md_establishment_data_load_batch($file,$sheet_name,$header_row,$first_data_row, $test){
	ini_set('memory_limit', "2000M");
	include_once 'sites/all/libraries/PHPExcel/Classes/PHPExcel.php';
	$regulation_section_name = 'Establishments';

	$section = hci_md_data_importer_node_load_by_title($regulation_section_name, 'section');
	$section_id = null;
	if ($section){
		$section_id = $section -> nid;
	}
	else{
		drupal_set_message("Regulation Standard Line section $regulation_section_name cannot be loaded. Exits.");
		return;
	}
	// other fields
	
	$file_path = drupal_realpath($file -> uri);
	$pathinfo = pathinfo($file->filename);
	$extension = $pathinfo['extension'];
	$filename = $pathinfo['filename'];
	//drupal_set_message(hci_md_dump($extension, 'red', TRUE));
	//drupal_set_message(hci_md_dump($file_path, 'red', TRUE));
	$queue = DrupalQueue::get('hci_md_establishment_data_load');
	$queue -> deleteQueue(); // delete all un-done items
	if($extension == 'xls'){
		$file_format = 'Excel5';
	}
	elseif($extension == 'xlsx'){
		$file_format = 'Excel2007';
	}
	else{
		drupal_set_message("I cannot parse $filename.$extension. File must be xls or xlsx.");
		return;
	}
	//drupal_set_message(hci_md_dump($file_format, 'red', TRUE));
	//$cacheMethod = PHPExcel_CachedObjectStorageFactory::cache_in_memory_gzip;
	$cacheMethod = PHPExcel_CachedObjectStorageFactory::cache_to_phpTemp;
	$cacheSettings = array( 'memoryCacheSize' => '2GB', 'cacheTime' => 6000);
	PHPExcel_Settings::setCacheStorageMethod($cacheMethod, $cacheSettings);
	PHPExcel_Cell::setValueBinder(new PHPExcel_Cell_AdvancedValueBinder());
	$objReader =PHPExcel_IOFactory::createReader($file_format);
	$objReader->setReadDataOnly(TRUE);
	$objPHPExcel = $objReader->load($file_path);
	
	$sheet = $objPHPExcel -> getSheetByName($sheet_name);
	//drupal_set_message(hci_md_dump($sheet, 'red', TRUE));
	if (!$sheet){
		drupal_set_message("Sheet $sheet_name cannot be found.");
		return;
	}
    $highest_row = $sheet -> getHighestRow();
    $highest_col = PHPExcel_Cell::columnIndexFromString($sheet -> getHighestColumn());
	$column_index = array();
	//parses header
    for ($col = 0; $col < $highest_col; $col ++){ // 
      $col_name = trim($sheet-> getCellByColumnAndRow($col, $header_row) -> getCalculatedValue());
      $column_index[$col] = $col_name;
    }
	//drupal_set_message(hci_md_dump($column_index, 'red', TRUE));
	//parses data
	$data = array();
	$unique_datasets = array();
	
	for ($row = $first_data_row; $row <= $highest_row; $row ++){
		for ($col = 0; $col < $highest_col; $col ++){ // 
			$value = trim($sheet-> getCellByColumnAndRow($col, $row) -> getCalculatedValue());
			$col_name = $column_index[$col];
			$data[$col_name]=$value;
		}
		$data['section_id']=$section_id;
		//drupal_set_message(hci_md_dump($data, 'red', TRUE));
		$unique_dataset_key = trim($data['License Number']);
		$unique_site_id = trim($data['Reference Number']);
		if (empty($unique_dataset_key) || $unique_dataset_key == 'none'){
			drupal_set_message(t("There is no Licence Number at !row. The row is skipped.", array('!row'=> $row)));
			continue;
		}
		if (empty($unique_site_id)){
			drupal_set_message(t("There is no Reference Number at !row. The row is skipped.", array('!row'=> $row)));
			continue;
		}
		$inspection_start_date = $data['Inspection Start Date'];
		
		$unique_datasets[$unique_dataset_key]['section_id']=$data['section_id'];
	
		$unique_datasets[$unique_dataset_key]['License Number']=trim($data['License Number']);
		$unique_datasets[$unique_dataset_key]['COMPANY NAME']=trim($data['COMPANY NAME']);
		$unique_datasets[$unique_dataset_key]['Licensed']=trim($data['MDEL ACTIVE']);
		if (!empty($unique_datasets[$unique_dataset_key]['sites'][$unique_site_id]) && $unique_datasets[$unique_dataset_key]['sites'][$unique_site_id]['Inspection Start Date'] < $inspection_start_date){
			drupal_set_message(t("site $inspection_start_date is duplicated. Most recent one is used."));
			continue;
		}
		

		$unique_datasets[$unique_dataset_key]['sites'][$unique_site_id]['row_number']=$row;
		$unique_datasets[$unique_dataset_key]['sites'][$unique_site_id]['Reference Number']=$unique_site_id;
		$unique_datasets[$unique_dataset_key]['sites'][$unique_site_id]['Inspection Start Date']=trim($data['Inspection Start Date']);
		$unique_datasets[$unique_dataset_key]['sites'][$unique_site_id]['STREET']=trim($data['STREET']);
		$unique_datasets[$unique_dataset_key]['sites'][$unique_site_id]['CITY']=trim($data['CITY']);
		$unique_datasets[$unique_dataset_key]['sites'][$unique_site_id]['PROVINCE']=trim($data['PROVINCE']);
		$unique_datasets[$unique_dataset_key]['sites'][$unique_site_id]['COUNTRY']=trim($data['COUNTRY']);
		$unique_datasets[$unique_dataset_key]['sites'][$unique_site_id]['Postal Code']=trim($data['Postal Code']);
		$unique_datasets[$unique_dataset_key]['sites'][$unique_site_id]["T&C's"]=trim($data["T&C's"]);
		$unique_datasets[$unique_dataset_key]['sites'][$unique_site_id]['ACTIVITY']=trim($data['Activity_En']);

		//drupal_set_message(hci_md_dump($unique_datasets[$unique_dataset_key], 'red', TRUE));
	}
	foreach($unique_datasets as $key => $unique_dataset){
		$establishment_active = TRUE;
		/*
		foreach($unique_dataset['sites'] as $site){
			$establishment_active = $establishment_active & strtoupper($site['Active'][0])=='Y' ? TRUE : FALSE;
		}
		*/
		//$unique_dataset['Active']=$establishment_active;
		
		$item = array('data' => $unique_dataset, );
		$queue->createItem($item);
	}
	//drupal_set_message(hci_md_dump($unique_datasets, 'red', TRUE));
	$operations = array();
	$operations[] = array('hci_md_establishment_data_load', array($queue->numberOfItems(), $test));
	$batch = array(
		'operations' => $operations,
		'finished' => 'hci_md_batch_op_finished',
		// We can define custom messages instead of the default ones.
		'title' => t('Migrating regulation data'),
		'init_message' => t('Migrating starting...'),
		'progress_message' => t('Processed @current out of @total steps.'),
		'error_message' => t('Migrating has encountered an error.'),
		'file' => drupal_get_path('module', 'hci_md_data_importer'). '/forms.inc',
	);
	//drupal_set_message(hci_md_dump($batch, 'purple', TRUE));
	batch_set($batch);
	batch_process();
}
function hci_md_establishment_data_load($total_items, $test, &$context){
	$queue = DrupalQueue::get('hci_md_establishment_data_load');
	if (!isset($context['sandbox']['progress'])) {
		$context['sandbox']['progress'] = 0;
		$context['sandbox']['max'] = $total_items;
		$context['results']['log'] = array();
	}
	$item = $queue->claimItem();
	//drupal_set_message(_dump($item, 'purple', TRUE));
	if ($item) {
		//drupal_set_message(hci_md_dump($item, 'purple', TRUE));
		hci_md_save_establishment($item->data['data'], $test);
		$queue->deleteItem($item);
	}
	

	// Provide an estimation of the completion level we've reached.
	$context['sandbox']['progress'] += 1;
	$context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
	$context['message'] = t( 'Processed @current out of @total', array('@current' => $context['sandbox']['progress'], '@total' => $context['sandbox']['max']));
}
function hci_md_save_establishment($data, $test){
	
	global $user;
	$establishment_type = 'hci_establishment';
//drupal_set_message(hci_md_dump($data, 'red', TRUE));
	$establishment_id = $data['License Number'];
	if (!$establishment_id){
		return;
	}
		
	$company_name = $data['COMPANY NAME'];
	
	$section_id = $data['section_id'];
	$establishment_active=isset($data['Active'])? $data['Active'] : 1;
	$sites = $data['sites'];
	$sql = new EntityFieldQuery();
	$sql->entityCondition('entity_type', 'node')
	->entityCondition('bundle', $establishment_type)
	->fieldCondition('field_license_number', 'value', $establishment_id, '=');
	$result = $sql->execute();
	//drupal_set_message(hci_md_dump($result, 'red', TRUE));
	//return;
	$establishment_node = null;
	$is_new = TRUE;
	if ($result){
		$establishment_node = node_load(array_shift((array_keys($result['node']))));
		$is_new = FALSE;
	}
	else{
		$establishment_node =  entity_create('node', array('type' => $establishment_type));
		$is_new = TRUE;
	}
	$establishment_node -> revision = 1;
	$establishment_wrapper = entity_metadata_wrapper('node',$establishment_node);
	//return;
	// oa_section_ref
	$establishment_wrapper -> field_section = $section_id;
	
	// other fields
	
	$establishment_wrapper -> title = $company_name;
	//drupal_set_message(hci_md_dump($establishment_obsolete, 'red', TRUE));
	if ($establishment_active){
		$establishment_wrapper -> status = 1;
		//drupal_set_message(hci_md_dump($establishment_node, 'red', TRUE));
	}
	else{
		$establishment_wrapper -> status = 0;
	}

	$establishment_wrapper -> field_license_number = $establishment_id;
	$establishment_wrapper -> field_establishment_name = $company_name;
	$establishment_wrapper -> field_license_expiry_date = NULL;
	$currently_licenced = $data['Licensed'];

	$establishment_wrapper -> field_currently_licensed = $currently_licenced[0] == 'Y' ? TRUE : FALSE;

	$time = time();
	
	if($is_new){
		$establishment_wrapper -> author = $user->uid;
		$establishment_wrapper -> created = $time;
	}

	$site_type = 'hci_site';
	$site_nids = array();
	$country_list = country_get_list(); // from module local
	foreach($sites as $key => $site){
		//drupal_set_message(hci_md_dump($site, 'red', TRUE));
		$row_number = $site['row_number'];
		$site_active = isset($site['Active']) && strtoupper($site['Active'][0])=='N' ? FALSE : TRUE;
		$site_number = $site['Reference Number'];
		$street = $site['STREET'];
		$city = $site['CITY'];
		$province = $site['PROVINCE'];
		$country = $site['COUNTRY'];
		$postal_code = $site['Postal Code'];
		$activities = explode(',', $site['ACTIVITY']);
		foreach($activities as $key =>$activity){
			$activities[$key]=trim($activity);
		}
		
		$activity_nids = array();
		$t_c_stdlin_nids=array();
		$country_code = country_is_a_valid_country($country);
		if ($country_code){

			foreach($activities as $activity){
				
				$activity_sql = new EntityFieldQuery();
				$activity_sql->entityCondition('entity_type', 'node')
				->entityCondition('bundle', 'hci_activity_standard_line')
				->fieldCondition('field_description_english', 'value', $activity, '=');
				$activity_result = $activity_sql->execute();
				//drupal_set_message(hci_md_dump($result, 'red', TRUE));
				//return;
				$site_node = null;
				$activity_node = null;
				if ($activity_result){
					//$activity_node = node_load(array_shift((array_keys($activity_result['node']))));
					$activity_nids[]=array_shift((array_keys($activity_result['node'])));
				}
				else{
					drupal_set_message(t('!activity cannot be mapped at line !row_number. Please refer to HCI Activity Standard Line for correct activity standard line', array('!activity' => $activity,'!row_number' =>$row_number,)));
				}
			}
			$site_sql = new EntityFieldQuery();
			$site_sql->entityCondition('entity_type', 'node')
			->entityCondition('bundle', $site_type)
			->fieldCondition('field_site_number', 'value', $site_number, '=');
			$site_result = $site_sql->execute();
			//drupal_set_message(hci_md_dump($result, 'red', TRUE));
			//return;
			$site_node = null;
			$site_is_new = TRUE;
			if ($site_result){
				$site_node = node_load(array_shift((array_keys($site_result['node']))));
				$site_is_new = FALSE;
			}
			else{
				$site_node =  entity_create('node', array('type' => $site_type));
				$site_is_new = TRUE;
			}
			
			$site_node -> revision = 1;
			$site_wrapper = entity_metadata_wrapper('node',$site_node);
			$province_code = location_province_code($country_code, $province);
			//drupal_set_message(hci_md_dump($province_code, 'red', TRUE));
			if ($province && !$province_code){
				$province_list = location_get_provinces($country_code);
				drupal_set_message(t("!province cannot be mapped at line !row_number. Please refer to following list for correct province name", array('!province' => $province,'!row_number' =>$row_number,)));
				dpm($province_list);
			}
			
			$location_query = db_select('location', 'l');
			$location_query->fields('l')
				->condition('street', $street,'=')
				->condition('city', $city,'=')
				->condition('country', $country_code,'=');
			if ($province_code){
				$location_query->condition('province', $province_code,'=');
			}
						
			$location_result = $location_query->execute();
			$location = array();
			if ($location_result->rowCount() > 0){
				$lid = $location_result->fetchObject()->lid;
				$location = location_load_location($lid);
			}
			
			$location['country']=$country_code;
			if ($province_code){
				$location['province']=$province_code;
			}
			$location['street'] =$street;
			$location['city'] = $city;
			if ($postal_code){
				$location['postal_cod'] = $postal_cod;
			}
			$location_id = location_save($location);

			$site_wrapper -> field_site_number = $site_number;
			//continue;
			$site_wrapper -> field_activity_std_ln = $activity_nids;
			//drupal_set_message(hci_md_dump($location, 'red', TRUE));
			$site_wrapper -> field_location ->set($location);

			// ToDo: terms and conditions
			
			if($site_is_new){
				$site_wrapper -> author = $user->uid;
				$site_wrapper -> created = $time;
			}
			//drupal_set_message(hci_md_dump($site_active, 'red', TRUE));
			if ($site_active){
				$site_wrapper -> status = 1;
				//drupal_set_message(hci_md_dump($site_node, 'red', TRUE));
			}
			else{
				$site_wrapper -> status = 0;
			}
			if (!$test) $site_wrapper -> save();
			//drupal_set_message(hci_md_dump($activity_nids, 'red', TRUE));
			//drupal_set_message(hci_md_dump($site_wrapper->field_reference_number->value(), 'red', TRUE));
			$site_nids[]=$site_wrapper->nid->value();
		}
		else{
			drupal_set_message(t("!country cannot be mapped at line !row_number. I won't map this site. Please refer to the following list for correct country name", array('!country' => $country,'!row_number' =>$row_number,)));
			$country_list = location_get_iso3166_list();
			dpm($country_list);
		}
	}
	$establishment_wrapper -> field_sites = $site_nids;
	//return;	

	//drupal_set_message(hci_md_dump($establishment_wrapper->raw(), 'red', TRUE));
	
	if (!$test) {
		if ($establishment_wrapper -> field_sites -> count()<=0){
			$site_wrapper -> status = 0;
		}
		$establishment_wrapper -> save();
	}
	
}
function hci_md_batch_op_finished($success, $results, $operations) {
  if ($success) {
    // Here we could do something meaningful with the results.
    // We just display the number of nodes we processed...
    $message = count($results) . ' processed.';
    //field_reference_update_all_cached_option_lists();
  }
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    $message = 'An error occurred while processing ' . $error_operation[0] . ' with arguments :' . print_r($error_operation[0], TRUE);
  }// remove temp variable
	variable_del('chili_oa_migration_ids_to_delete');
  drupal_set_message($message);
}

