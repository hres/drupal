<?php
/**
 *This
*/
define('DRUPAL_ROOT', getcwd());

$temp_attachment_file_folder = '';
$offcial_attacment_file_folder = '';
$chili_db = '';
$custom_fields_mapping = array();

function chili_oa_migration_selection_page() {
  drupal_set_title('Migrating Chiliproject to OA');
  $elements = drupal_get_form('com_admin_form');
  $output = drupal_render($elements);
  //drupal_set_message(_dump($output, 'purple', TRUE));
  return $output;
}



function com_admin_form($form, &$form_state = NULL) {


    $action_option =  array('select_task' => 'Select A Task',
                'load_chiliproject_project_data' => 'Migrate Chiliproject project data',
                'load_chiliproject_user_data' => 'Migrate Chiliproject user data',
                'load_chiliproject_issue_data' => 'Migrate Chiliproject issue data',
                'load_chiliproject_wiki_data' => 'Migrate Chiliproject Wiki data',
                 );

 // }
  $action_default = !empty($form_state['values']['action']) ? $form_state['values']['action'] : '';
  $action = null;
  if (isset($form_state['values'])) $action = $form_state['values']['action'];
  $form['#attributes'] = array('enctype' => "multipart/form-data");
   $form['action'] = array(
    '#type' => 'select',
    '#title' => t('Actions'),
    '#default_value' => $action_default,
    '#options' => $action_option,
    '#description' => t('Select an action.'),
    '#attributes' => array('onchange' => 'this.form.submit();'),
  );

  $form['next'] = array(
    '#type' => 'submit',
    '#value' => 'Next >>',
    '#attributes' => array('onclick' => 'return confirm("Are you sure you wish to continue?");'),
  );
  if ($action) {
    timer_start('nhpid_init');
    switch ($action) {

			case 'load_chiliproject_user_data':
        $form[] =com_load_chiliproject_user_data_form($form_state);
        break;
      case 'load_chiliproject_issue_data':
        $form[] =com_load_chiliproject_issue_data_form($form_state);
        break;
      case 'load_chiliproject_wiki_data':
        $form[] =com_load_chiliproject_wiki_data_form($form_state);
        break;
      case 'load_chiliproject_project_data':
        $form[] =com_load_chiliproject_project_data_form();
        break;
      default:
        //exit (0);

    }


  }
  return $form;
}
function com_admin_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  $form_state['storage']['values'] = $form_state['values'];

}


function com_batch_op_finished($success, $results, $operations) {
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
  }
  drupal_set_message($message);
}

/* chiliproject migaration

*/

function com_load_chiliproject_user_data_form(&$form_state = NULL){
	$form = array();

	$form['warning']=array(
    '#type' => 'item',
    '#markup' => "This operation will re-migrate user data from chili project. The existng user data will be replaced with chili data.",
	);
  $form['action-submit']=array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#attributes' => array('onclick' => 'return confirm("You are going to migrate all issues from chiliporject to OA. Are you sure you wish to continue?");'),
  );

  if ($form_state['clicked_button']['#id'] == 'edit-action-submit') {
  	
    com_load_chiliproject_user_data();
	}

	return $form;
}

function com_load_chiliproject_user_data(){
  global $temp_attachment_file_folder;
	global $offcial_attacment_file_folder;
	global $chili_db;
	global $custom_fields_mapping;
  require_once DRUPAL_ROOT . '/' . variable_get('password_inc', 'includes/password.inc');
	$form = array();
	//goto SPACEMEMBER;
	# roles
	drupal_set_message('Starting migragting roles');
	$table = $chili_db. '.roles';
	$sql = db_select($table, 'r') -> fields('r') -> execute();
	while ($rc = $sql -> fetchObject()){
		$name = $rc->name;
		if (!user_role_load_by_name($name)){
			$role = new stdClass();
			$role -> name = $name;
			user_role_save($role);
		}
	}
  drupal_set_message('Migragting roles is done.');
	#user
  drupal_set_message('Starting migragting users');
	$table = $chili_db. '.users';
	$sql = db_select($table, 'u') -> fields('u') -> execute();
	while ($rc = $sql -> fetchObject()){
		$login = $rc->login;
		if (!$login) continue; // skip Anonymous
		$firstname = $rc->firstname;
		$lastname = $rc->lastname;
		$mail = $rc->mail;
		$last_login = $rc->last_login_on;
		$created_on = $rc->created_on;
		$status = $rc->status;
		$uid = $rc -> id;
		$custom_fields = array();
		/*
		$sql2 = db_select($chili_db. '.custom_values', 'cv')
		-> join ($chili_db. '.custom_fields', 'cf', 'cv.cystom_field_id=cf.id')
		-> fields('cf', array('name'))
		-> fields('cv', array('value'))
		-> condition('cf.type', 'UserCustomField', '=')
		-> condition ('cv.customized_id', $uid, '=')
		-> execute();
		*/
    $sql2 =  "select cv.customized_id, cv.customized_type, cv.value, cf.type, cf.name from $chili_db.custom_values cv, $chili_db.custom_fields cf  where customized_id in (select id from $chili_db.users) and cv.custom_field_id = cf.id and cf.type = 'UserCustomField' and customized_id = :uid order by customized_id";
		$result = db_query($sql2, array(':uid' => $uid));
		while ($rc2 = $result -> fetchObject()){
		  //drupal_set_message(_dump($rc, 'purple', TRUE,true, true));
			switch($rc2->name){
				case 'NHPD':
				$custom_fields['field_nhpd'] = $rc2->value;
				break;
        case 'Co-op':
				$custom_fields['field_co_op'] = $rc2->value;
				break;
        case 'SERLO':
				$custom_fields['field_serlo'] = $rc2->value;
				break;
			}
		}

		$roles = array(DRUPAL_AUTHENTICATED_RID);
    $account = new stdClass();
		$account -> is_new = TRUE;
		$new_user = array(
			'name' => $login,
			'pass' => 'password',
			'status' => $status == 1? $status: 0,
			'roles' => $roles,
			'timezone' =>'America/New_York',
			'mail' => $mail,
			'created' => strtotime($created_on),
			'access' => strtotime($last_login),
		);
		$a_user = user_load_by_name($new_user['name']);
		if (!$a_user){
		 $a_user = user_save($account, $new_user);
		}
		//drupal_set_message(_dump($a_user, 'purple', TRUE,true, true));
		$user_wrapper = entity_metadata_wrapper('user', $a_user);
		$user_wrapper -> field_first_name = $firstname;
		$user_wrapper -> field_last_name = $lastname;
		$user_wrapper -> field_user_display_name = "$firstname $lastname";
    //$user_wrapper -> status = $status == 1? $status: 0;
    //$user_wrapper -> mail = $mail;
    //$user_wrapper -> created = strtotime($created_on);
    //$user_wrapper -> access = strtotime($last_login);

		foreach($custom_fields as $field => $value){
			$user_wrapper -> {$field} = $value;
		}
   	$user_wrapper->save();

	}
  drupal_set_message('Migragting users is done.');

	# groups
	global $user;
  drupal_set_message('Starting migragting groups');
  $table = $chili_db. '.users';
	$sql = "select * from $table where type = 'Group'";
	$result = db_query($sql);
	while ($rc = $result->fetchObject()){
		$group_id = $rc -> id;
		$group_name = $rc -> lastname;
    $last_login = $rc->last_login_on;
		$created_on = $rc->created_on;
    $entity = entity_create('node', array('type' => 'oa_group'));
    $entity_wrapper = entity_metadata_wrapper('node',$entity);
    //drupal_set_message(_dump($type, 'purple', TRUE));
		//$entity -> uid = $user -> uid;

		$oa_group_node = com_node_load_by_title($group_name, 'oa_group');
		$oa_group_node_nid = null;

    if ($oa_group_node){
      $entity_wrapper = entity_metadata_wrapper('node',$oa_group_node);
		}
    $entity_wrapper -> title = $group_name;
    $entity_wrapper -> created = strtotime($created_on);
    $entity_wrapper -> og_user_inheritance = 1;
    $entity_wrapper -> og_user_permission_inheritance = 1;
    $entity_wrapper -> group_access = 1;
    $entity_wrapper -> status = 1;
    $entity_wrapper -> author = $user;
    $entity_wrapper -> save();
    $oa_group_node = node_load($entity_wrapper -> value()-> nid);
    //drupal_set_message(_dump((array)$entity_wrapper, 'purple', TRUE));

    $oa_group_node_nid = $oa_group_node -> nid;


		//group members
		$sql2 = "select u.* from " . $chili_db. ".users u, " . $chili_db. ".groups_users gu where gu.user_id = u.id and group_id = :group_id ";
		$result2 = db_query($sql2, array(":group_id" => $group_id));

		while($rc2 =$result2 -> fetchObject()){
			$login = $rc2->login;
			$member = user_load_by_name($login);
			if ($member){
        $user_wrapper = entity_metadata_wrapper('user',$member);
        $og_user_node = $user_wrapper -> og_user_node -> value(array('identifier' => TRUE));
        if (!in_array($oa_group_node_nid, $og_user_node)){
				  $user_wrapper -> og_user_node ->offsetSet( $user_wrapper -> og_user_node ->count(), $oa_group_node);
					$user_wrapper -> save();
				}
			}
			else{
				drupal_set_message ("User $login cannot be found.");
			}

		}

	}
  drupal_set_message('Migragting group is done.');
SPACEMEMBER:
	//space members
  drupal_set_message('Starting migragting space members');
	$sql = "select project_id, name, login, lastname, user_id , type from " . $chili_db. ".members m, " . $chili_db. ".users u, " . $chili_db. ".projects p where m.user_id = u.id and p.id = m.project_id";
  $result = db_query($sql);

	while($rc =$result -> fetchObject()){
		$project_name = $rc -> name;
    $login = $rc -> login;
    $lastname = $rc -> lastname;
    $member_type = $rc -> type;
    $user_id = $rc -> user_id;
    $project_id = $rc -> project_id;
    $oa_space_node = com_node_load_by_title($project_name, 'oa_space');
		if (!$oa_space_node){
      drupal_set_message ("Space $project_name (chili id: $project_id) cannot be found.");
			continue;
		}

		if ($member_type == "User"){
      $member = user_load_by_name($login);
			if ($member){
        $user_wrapper = entity_metadata_wrapper('user',$member);
        $og_user_node = $user_wrapper -> og_user_node -> value(array('identifier' => TRUE));
        if (!in_array($oa_space_node->nid, $og_user_node)){
				  $user_wrapper -> og_user_node ->offsetSet( $user_wrapper -> og_user_node ->count(), $oa_space_node);
					$user_wrapper -> save();
				}
			}
      else{
				drupal_set_message ("User $login(chili id: $user_id) cannot be found.");
			}
		}
		elseif ($member_type == "Group"){
      $group = com_node_load_by_title($lastname, 'oa_group');
			if($group){
				$space_wrapper = entity_metadata_wrapper('node',$oa_space_node);
        $oa_parent_space = $space_wrapper -> oa_parent_space -> value(array('identifier' => TRUE));
        if (!in_array($group->nid, $oa_parent_space)){
				  $space_wrapper -> oa_parent_space ->offsetSet( $space_wrapper -> oa_parent_space ->count(), $group);
					$space_wrapper -> save();
				}
			}
			else{
        drupal_set_message ("Group $lastname (chili id: $user_id)cannot be found.");
			}
		}
	}
  drupal_set_message('Migragting space members is done.');
}

function com_load_chiliproject_issue_data_form(&$form_state = NULL){
	$form = array();

  $mapping = isset($form_state['values']['mapping_chili_project_id']) ? $form_state['values']['mapping_chili_project_id'] : 0;

  $form['mapping_chili_project_id']=array(
    '#type' => 'checkbox',
    '#default_value' => $mapping,
    '#title' => "Map chili project id to the Task sections? ",
	);

  $round2 = isset($form_state['values']['round2']) ? $form_state['values']['round2'] : 0;
	$form['round2']=array(
    '#type' => 'checkbox',
    '#default_value' => $round2,
    '#title' => "Round2 only?",
	);

	$test = isset($form_state['values']['test']) ? $form_state['values']['test'] : 0;
	$form['test']=array(
    '#type' => 'checkbox',
    '#default_value' => $test,
    '#title' => "Testing? (first 100 records)",
	);
  $form['action-submit']=array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#attributes' => array('onclick' => 'return confirm("You are going to migrate all issues from chiliporject to OA. Are you sure you wish to continue?");'),
  );

  if ($form_state['clicked_button']['#id'] == 'edit-action-submit') {
  	if ($mapping){
			drupal_set_message("Stating mapping chili porject id to the Task sections...");
	    mapping_chili_project_id_to_task_sections('Task Section');
	    drupal_set_message("Mapping chili porject id to the Task sections is done.");
		}
    $sql = "select id from " . $chili_db. ".issues order by id";
		if ($test) $sql = "select id from " . $chili_db. ".issues order by id limit 100";
		$result = db_query($sql);
		$operations = array();
		if(!$round2){
			while ($rc = $result ->fetchObject()){
				$id = $rc -> id;
		    $operations[] = array('load_chiliproject_issue_data_batch', array($id, $test, 1));
			}
		}
    $result = db_query($sql);
    while ($rc = $result ->fetchObject()){
			$id = $rc -> id;
			$round = 2;
	    $operations[] = array('load_chiliproject_issue_data_batch', array($id, $test, $round));
		}
	  $batch = array(
	    'operations' => $operations,
	    'finished' => 'batch_op_finished',
	    // We can define custom messages instead of the default ones.
	    'title' => t('Migrating chili issues'),
	    'init_message' => t('Migrating starting...'),
	    'progress_message' => t('Processed @current out of @total steps.'),
	    'error_message' => t('Migrating has encountered an error.'),
	  );
	  batch_set($batch);
	  batch_process();
    //drupal_set_message(_dump($operations, 'purple', TRUE));

	}

	return $form;
}

function com_load_chiliproject_issue_data_batch($id, $testing,$round, $content){
  //drupal_set_message(_dump("$id, $testing,$round", 'purple', TRUE));
	$message = '';
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_file'] = 0;
    $context['sandbox']['max'] = count($sheets);
  }
	if ($round == 2) {
		com_load_chiliproject_issue_data_2($id, $testing);
	}
	else{
    com_load_chiliproject_issue_data($id, $testing);
	}
  // Inform the batch engine that we are not finished,
  // and provide an estimation of the completion level we reached.
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

function com_mapping_chili_project_id_to_spaces(){
  global $temp_attachment_file_folder;
	global $offcial_attacment_file_folder;
	global $chili_db;
	global $custom_fields_mapping;
  $sql = "select id, name from " . $chili_db. ".projects order by id";

	$result = db_query($sql);
	while ($rc = $result ->fetchObject()){
		$project_id = $rc -> id;
		$project_name = $rc -> name;
		$oa_space_node = com_node_load_by_title($project_name, 'oa_space');
    $oa_space_node_wrapper = entity_metadata_wrapper('node',$oa_space_node);
    $oa_space_node_wrapper -> field_chili_project_id = $project_id;
	  $oa_space_node_wrapper -> save();
	}
}

function com_mapping_chili_project_id_to_task_sections($section = 'Task Section'){
  global $temp_attachment_file_folder;
	global $offcial_attacment_file_folder;
	global $chili_db;
	global $custom_fields_mapping;
	$tid = array_shift(taxonomy_get_term_by_name($section, 'section_type')) -> tid;
  //drupal_set_message(_dump($tid, 'purple', TRUE));
	$sql = "select id, name from " . $chili_db. ".projects order by id";

	$result = db_query($sql);
	while ($rc = $result ->fetchObject()){
		$project_id = $rc -> id;
		$project_name = $rc -> name;
		$oa_space_node = com_node_load_by_title($project_name, 'oa_space');
		if (!$oa_space_node){
      drupal_set_message ("Space $project_name (chili id: $project_id) cannot be found.");
		}
		else{
      $sql = new EntityFieldQuery();
		  $sql->entityCondition('entity_type', 'node')
		  	->entityCondition('bundle', 'oa_section')
		    ->fieldCondition('og_group_ref', 'target_id', $oa_space_node -> nid, '=')
        ->fieldCondition('field_oa_section', 'tid', $tid, '=');
		  $result2 = $sql->execute();
      //drupal_set_message(_dump($result2, 'purple', TRUE));
			if ($result2){
				$section_nid = array_shift((array_keys($result2['node'])));
				$section_node = node_load($section_nid);
				$section_wrapper = entity_metadata_wrapper('node',$section_node);
	      $section_wrapper -> field_chili_project_id = $project_id;
	      $section_wrapper -> save();
	      //drupal_set_message(_dump((array)$section_wrapper, 'purple', TRUE));
	      //drupal_set_message(_dump($project_name, 'purple', TRUE));
	      //drupal_set_message(_dump($oa_space_node, 'purple', TRUE));
			}
		}

	}
}

function com_load_chiliproject_issue_data($id, $testing){
  global $temp_attachment_file_folder;
	global $offcial_attacment_file_folder;
	global $chili_db;
	global $custom_fields_mapping;
	$sql = "select i.*, ist.name as status_name, au.login as author, ast.login as assigned_to, p.name as project_name, t.name as tracker_name from  ". $chili_db. ".issues i LEFT JOIN ". $chili_db. ".issue_statuses ist on i.status_id=ist.id LEFT JOIN ". $chili_db. ".users au on i.author_id=au.id LEFT JOIN ". $chili_db. ".users ast ON i.assigned_to_id=ast.id LEFT JOIN ". $chili_db. ".projects p on i.project_id=p.id LEFT JOIN ". $chili_db. ".trackers t ON i.tracker_id=t.id where  i.id = :id";
  //drupal_set_message(_dump($sql, 'purple', TRUE));

	if (!$id) return;
	$result = db_query($sql, array(':id' => $id));
	if ($result->rowCount() < 1){
    drupal_set_message("Chili issue $id cannot be found. Skipped");
	}
	else {

		$rc = $result->fetchObject();
	  //drupal_set_message(_dump($rc, 'purple', TRUE));
		//return;

		$tracter_id = $rc -> tracker_id;
		$project_id = $rc -> project_id;
	  $project_name = $rc -> project_name;
		$subject = $rc -> subject;
		$description = $rc -> description;
		$due_date = $rc -> due_date;
		$category_id = $rc -> category_id;
		$status_id = $rc -> status_id;
		$assigned_to = $rc -> assigned_to;
		$priority_id = $rc -> priority_id;
		$author = $rc -> author;
		$created_on = $rc -> created_on;
		$updated_on = $rc -> updated_on;
		$start_date = $rc -> start_date;
		$parent_id = $rc -> parent_id;
    $status_name = $rc -> status_name;
		$tracker_name = $rc -> tracker_name;
    //return;

		//oa task
		$sql = new EntityFieldQuery();
	  $sql->entityCondition('entity_type', 'node')
	  	->entityCondition('bundle', 'oa_worktracker_task')
	    ->fieldCondition('field_chili_id', 'value', $id, '=');
	  $result2 = $sql->execute();
		$task_node = null;

		if ($result2){
			$task_node = node_load(array_shift((array_keys($result2['node']))));
		}
		else{
			$task_node =  entity_create('node', array('type' => 'oa_worktracker_task'));
		}
    $task_wrapper = entity_metadata_wrapper('node',$task_node);
		//return;
	   // oa_section_ref

		$tid = array_shift(taxonomy_get_term_by_name('Task Section', 'section_type')) -> tid;
	  $sql = new EntityFieldQuery();
	  $sql->entityCondition('entity_type', 'node')
	  	->entityCondition('bundle', 'oa_section')
	    ->fieldCondition('field_chili_project_id', 'value', $project_id, '=')
			->fieldCondition('field_oa_section', 'tid', $tid, '=');
	  $result2 = $sql->execute();
		if ($result2){
	    $section_nid = array_shift((array_keys($result2['node'])));
	  	$task_wrapper -> oa_section_ref = $section_nid;
		}
		else{
	    drupal_set_message("Task section under $project_name cannot be found. The issue is skipped.");
			return;
		}
		//return;
	  // oa_group_ref
		$space = com_node_load_by_title($project_name, 'oa_space');
		if ($space){
	    $task_wrapper -> og_group_ref = $space -> nid;
		}
		else{
	    drupal_set_message("Space $project_name cannot be found. og_group_ref is not assined.");
		}
		// other fields

	  $task_wrapper -> title = $subject;
		$task_wrapper -> body = array('value' => $description, 'text_processing' => 1);
	  $task_wrapper -> created = strtotime($created_on);
	  $task_wrapper -> status = 1;
	  $task_wrapper -> field_oa_worktracker_task_type = $tracker_name;
	  $task_wrapper -> field_oa_worktracker_priority = $priority_id;
	  $task_wrapper -> field_oa_worktracker_task_status = strtolower($status_name);
	  $task_wrapper -> field_chili_id = $id;

	  if ($due_date) $task_wrapper ->  field_oa_worktracker_duedate ->set(strtotime($due_date));
	  if ($start_date) $task_wrapper -> field_start_date ->set(strtotime($start_date));

		$author_id = user_load_by_name($author) -> uid;
		if ($author_id){
	    $task_wrapper -> author = $author_id;
		}
		else{
			drupal_set_message("User $author cannot be found. Author is not assigned.");
		}

		if ($assigned_to){
	    $assigned_to_id = user_load_by_name($assigned_to) -> uid;
			if ($assigned_to_id){
		    $task_wrapper -> field_oa_worktracker_assigned_to = $assigned_to_id;
			}
			else{
				drupal_set_message("User $assigned_to cannot be found. Assigned to is not assigned.");
			}
		}
  //drupal_set_message(_dump((array)$task_wrapper, 'purple', TRUE));
		//custom fields

    $sql2 =  "select cv.custom_field_id, cv.value from " . $chili_db. ".custom_values cv  where cv.customized_type = 'Issue' and cv.customized_id = :id";
		$result = db_query($sql2, array(':id' => $id));
		while ($rc2 = $result -> fetchObject()){
			$value = trim($rc2 -> value);
			$custom_field_id = $rc2 -> custom_field_id;
			$oa_custom_field_name = com_get_oa_custom_field_name($custom_field_id);
			$field_info = field_info_field($oa_custom_field_name);
			if($value){
	      //drupal_set_message(_dump($oa_custom_field_name, 'purple', TRUE));
	      //drupal_set_message(_dump("~$value~", 'purple', TRUE));
				switch ($field_info['module']){
					case 'date':
	          $task_wrapper -> $oa_custom_field_name ->set(strtotime($value));
					break;
					default;
	          $task_wrapper -> $oa_custom_field_name =$value;
				}
			}

		}
		// attachments
    $attachments = $task_wrapper -> field_oa_media -> value();
    //drupal_set_message(_dump((array)$attachments, 'purple', TRUE));

    $sql2 =  "select a.*, u.login as author from " . $chili_db. ".attachments a LEFT JOIN " . $chili_db. ".users u on a.author_id=u.id  where a.container_type = 'Issue' and a.container_id = :id";
		$result = db_query($sql2, array(':id' => $id));
		//next fid
    //$sql_next_fid = "SELECT `AUTO_INCREMENT` FROM  INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = 'oa_demo' AND  TABLE_NAME   = 'file_managed'";
    //$next_fid = db_query($sql_next_fid )->fetchObject()->AUTO_INCREMENT;
		while ($rc2 = $result -> fetchObject()){
			$filename = $rc2 -> filename;
      $disk_filename = $rc2 -> disk_filename;
      $filesize = $rc2 -> filesize;
      $content_type = $rc2 -> content_type;
      $attachment_author = $rc2 -> author;
      $created_on = $rc2 -> created_on;
      $description = $rc2 -> description;
      $attachment_author_id = user_load_by_name($attachment_author) -> uid;

			$exist = FALSE;
			foreach($attachments as $attachment){
				if ($attachment['uri'] == 'private://' . $disk_filename){
					$exist = TRUE;
					continue 2;
				}
			}

			//if (!$exist){
				$tmp_uri = $temp_attachment_file_folder . $disk_filename;
        $uri = $official_attachment_file_folder . $disk_filename;
        //drupal_set_message(_dump(drupal_realpath($uri), 'purple', TRUE));
        $file_temp = file_get_contents(drupal_realpath($tmp_uri));
        $doc = file_save_data($file_temp, $uri, FILE_EXISTS_REPLACE);
				/*
				$doc = array(
					'filename' => $filename,
					'uri' => $uri,
					'filemime' => $content_type,
					'type' => 'document',
					'display' => 1,
					'timestamp' => strtotime($created_on),
					'description' => $description,
					'fid' => $next_fid,
          'status' => FILE_STATUS_PERMANENT,
				);

        if ($attachment_author_id){
		    	$doc['uid'] = $attachment_author_id;
				}
        $doc = (object)$doc;
				*/
        //drupal_write_record('files', $doc);
				$doc -> display = 1;
				$doc -> status = FILE_STATUS_PERMANENT;
				$doc -> uid = $attachment_author_id;
        $doc -> filename = $filename;
				file_save($doc);
				//drupal_set_message(_dump($doc, 'purple', TRUE));
        $task_wrapper -> field_oa_media ->offsetSet( $task_wrapper -> field_oa_media ->count(), (array)$doc);
			//}

		}

    //drupal_set_message(_dump((array)$task_wrapper, 'purple', TRUE));
    $task_wrapper -> save();

		//comments, migrate comment only. No changes.
		$sql = "select j.*, u.login as author from " . $chili_db. ".journals j LEFT JOIN ". $chili_db. ".users u ON j.user_id=u.id where j.type='IssueJournal' and j.journaled_id=:id";
    $result2 = db_query($sql, array(':id' => $id));
		while ($rc2 = $result2->fetchObject()){
	    $comment_id = $rc2 -> id;
			$commnet_author = $rc2 -> author;
			$notes = $rc2 -> notes;
			if(!$notes) continue;
			$created_at = $rc2 -> created_at;
      $sql = new EntityFieldQuery();
		  $sql->entityCondition('entity_type', 'comment')
		  	->propertyCondition('nid', $task_node ->nid, '=')
	      ->fieldCondition('field_chili_journal_id', 'value', $comment_id, '=');
	    $result3 =$sql ->execute();
			$comment = null;
			if ($result3){
        $comment = comment_load(array_shift((array_keys($result3['comment']))));
			}
			else{
        $comment = entity_create('comment',array('node_type'=>'comment_node_oa_worktracker_task', 'nid' => $task_node ->nid));
			}
			$comment_wrapper = entity_metadata_wrapper('comment',$comment);
      if($notes) $comment_wrapper -> comment_body = array('value' => $notes, 'text_processing' => 1);
      if($notes) $comment_wrapper -> subject = truncate_utf8(trim(decode_entities(strip_tags($notes))), 29, TRUE);
		 	if($created_at) $comment_wrapper -> created = strtotime($created_at);
		  $comment_wrapper -> status = 1;
		  if($comment_id) $comment_wrapper -> field_chili_journal_id = $comment_id;
      $comment_author_id = user_load_by_name($commnet_author) -> uid;
			if ($comment_author_id){
		    $comment_wrapper -> author = $comment_author_id;
			}
			$comment_wrapper -> save();
      //drupal_set_message(_dump((array)$comment_wrapper, 'purple', TRUE));
		}

    drupal_set_message("Issue $subject ($id) has been successfully transferred.");


 }
}

function com_load_chiliproject_issue_data_2($id, $testing){
  global $temp_attachment_file_folder;
	global $offcial_attacment_file_folder;
	global $chili_db;
	global $custom_fields_mapping;
	$sql = "select i.* from " . $chili_db. ".issues i  where i.id = :id";
  //drupal_set_message(_dump($id, 'purple', TRUE));
	if (!$id) return;
	$result = db_query($sql, array(':id' => $id));
	if ($result->rowCount() < 1){
    drupal_set_message("Chili issue $id cannot be found. Skipped");
	}
	else {

		$rc = $result->fetchObject();
	  //drupal_set_message(_dump($rc, 'purple', TRUE));
		//return;
    $subject = $rc -> subject;
		$parent_id = $rc -> parent_id;

    //return;

		//parent task
		$sql = new EntityFieldQuery();
	  $sql->entityCondition('entity_type', 'node')
	  	->entityCondition('bundle', 'oa_worktracker_task')
	    ->fieldCondition('field_chili_id', 'value', $id, '=');
	  $result2 = $sql->execute();
		$task_node = null;
		if ($result2){
			$task_node = node_load(array_shift((array_keys($result2['node']))));
		}
		else{
			drupal_set_message("Task $subject cannot be found. The issue is skipped.");
			return;
		}
		if ($parent_id){
	    $sql = new EntityFieldQuery();
		  $sql->entityCondition('entity_type', 'node')
		  	->entityCondition('bundle', 'oa_worktracker_task')
		    ->fieldCondition('field_chili_id', 'value', $parent_id, '=');
		  $result2 = $sql->execute();
			$paretn_task_node = null;
			if ($result2){
				$parent_task_node = node_load(array_shift((array_keys($result2['node']))));
        $task_node_wrapper = entity_metadata_wrapper('node',$task_node);
				$task_node_wrapper -> field_parent_task -> set($parent_task_node);
       // drupal_set_message(_dump((array)$task_node_wrapper, 'purple', TRUE));
        $task_node_wrapper -> save();
			}
			else{
	      drupal_set_message("Parent task($parent_id) for $subject cannot be found. The issue is skipped.");
				return;
			}
			/*
			$parent_task_wrapper = entity_metadata_wrapper('node',$parent_task_node);
	    $subtasks = $parent_task_wrapper -> field_subtasks -> value(array('identifier' => TRUE));
	    if (!in_array($task_node -> nid, $subtasks)){
			  $parent_task_wrapper -> field_subtasks ->offsetSet( $parent_task_wrapper -> field_subtasks ->count(), $task_node);
			}
	    $parent_task_wrapper -> save();
      */

		}

		//related tasks
		$sql = "select i.* from " . $chili_db. ".issue_relations i  where i.issue_from_id = :id";
    $result2 = db_query($sql, array(':id' => $id));
		$lan = $task_node -> language;
		while ($rc2 = $result2->fetchObject()){
			$issue_to_id = $rc2 -> issue_to_id;
			if (!$issue_to_id) continue;
			$relation_type = $rc2 -> relation_type;
      $sql = new EntityFieldQuery();
		  $sql->entityCondition('entity_type', 'node')
		  	->entityCondition('bundle', 'oa_worktracker_task')
		    ->fieldCondition('field_chili_id', 'value', $issue_to_id, '=');
		  $result3 = $sql->execute();
			$new_related_task_node_id = null;
			if ($result3){
				$new_related_task_node_id = array_shift((array_keys($result3['node'])));
			}
			else{
	      drupal_set_message("Related task (chili id $issue_to_id) for $subject cannot be found. The related task is skipped.");
				continue;
			}

      $new_related_task = array(
				'field_relation_type'=>array(
					$lan => array(array('value'=>$relation_type))
				),
				'field_related_task' => array(
					$lan => array(array('target_id'=>$new_related_task_node_id))
				),
				'id' => multifield_get_next_id(),

			);

			$exist = FALSE;
			foreach($task_node->field_related_tasks[$lan] as $item){
				if ($item['field_relation_type'] == $new_related_task['field_relation_type'] and $item['field_related_task'] == $new_related_task['field_related_task']){
          $exist = TRUE;
				}
			}
			if (!$exist){
        $task_node->field_related_tasks[$lan][] = $new_related_task;
        //drupal_set_message(_dump($task_node, 'purple', TRUE));
			}

		}
    //drupal_set_message(_dump($task_node->field_related_tasks, 'purple', TRUE));
		node_save($task_node);
    //drupal_set_message(_dump((array)$task_wrapper, 'purple', TRUE));

 }
}
function com_load_chiliproject_project_data_form(&$form_state = NULL){
	$form = array();

	$form['warning']=array(
    '#type' => 'item',
    '#markup' => "This operation will re-migrate project structure data from chili project. The existng space data will be replaced with chili data.",
	);
  $form['action-submit']=array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#attributes' => array('onclick' => 'return confirm("You are going to migrate all issues from chiliporject to OA. Are you sure you wish to continue?");'),
  );

  if ($form_state['clicked_button']['#id'] == 'edit-action-submit') {

    com_load_chiliproject_project_data();
	}

	return $form;
}
function com_load_chiliproject_project_data(){
  global $temp_attachment_file_folder;
	global $offcial_attacment_file_folder;
	global $chili_db;
	global $custom_fields_mapping;
	//round 1
	global $user;
	$sql = 'select * from ' . $chili_db. '.projects';
  $result = db_query($sql);
	while ($rc = $result ->fetchObject()){
		$id = $rc -> id;
		$name = $rc ->name;
		$description = $rc -> description;
		$is_public = $rc -> is_public;
		$status = $rc -> status;

    $sql = new EntityFieldQuery();
	  $sql->entityCondition('entity_type', 'node')
	  	->entityCondition('bundle', 'oa_space')
	    ->fieldCondition('field_chili_project_id', 'value', $id, '=');
	  $result2 = $sql->execute();
		$space_node = null;
		if ($result2){
			$space_node = node_load(array_shift((array_keys($result2['node']))));
		}
		else{
			$space_node =  entity_create('node', array('type' => 'oa_space'));
		}
    $space_node -> field_view_field[$space_node->language][0] = array('vname'=>'customized_work_tracker|view_field_for_space', 'vargs'=>'[node:nid]');
    $space_wrapper = entity_metadata_wrapper('node',$space_node);
		$space_wrapper -> title = $name;
		$space_wrapper -> body = array('value'=>$description, 'text_processing'=>1, 'format' => 'panopoly_wysiwyg_text');
		$space_wrapper -> field_oa_section_override = 0;
    $tid = array_shift(taxonomy_get_term_by_name('default', 'space_type')) -> tid;
    $space_wrapper -> field_oa_space_type = $tid;
    $space_wrapper -> group_access = !$is_public;
    $space_wrapper -> group_group = 1;
    $space_wrapper -> og_user_inheritance = 0;
    $space_wrapper -> og_user_permission_inheritance = 0;
    $space_wrapper -> field_chili_project_id = $id;
    $space_wrapper -> author = $user;
    //$space_wrapper -> field_view_field = array('vname'=>'customized_work_tracker|view_field_for_space', 'vargs'=>'[node:nid]');
		$space_wrapper -> save();
    //drupal_set_message(_dump((array)$space_wrapper, 'purple', TRUE));
	}

	//round 2
  $sql = 'select * from ' . $chili_db. '.projects where parent_id is not null';
  $result = db_query($sql);
	while ($rc = $result ->fetchObject()){
		$id = $rc -> id;
		$parent_id = $rc -> parent_id;
    $sql = new EntityFieldQuery();
	  $sql->entityCondition('entity_type', 'node')
	  	->entityCondition('bundle', 'oa_space')
	    ->fieldCondition('field_chili_project_id', 'value', $id, '=');
	  $result2 = $sql->execute();
    $space_node = node_load(array_shift((array_keys($result2['node']))));
    $sql = new EntityFieldQuery();
	  $sql->entityCondition('entity_type', 'node')
	  	->entityCondition('bundle', 'oa_space')
	    ->fieldCondition('field_chili_project_id', 'value', $parent_id, '=');
	  $result3 = $sql->execute();

		if(!$result3){
			drupal_set_messasge("Parent space (chili project id: $parent_id) could not be found.");
			countinue;
		}
    $parent_node_id = array_shift((array_keys($result3['node'])));
    $space_wrapper = entity_metadata_wrapper('node',$space_node);
		$space_wrapper -> oa_parent_space = array('target_id' => $parent_node_id);
    $space_wrapper -> save();
	}

	// issue sections
  $tid = array_shift(taxonomy_get_term_by_name('Task Section', 'section_type')) -> tid;
	$sql = 'select distinct project_id from ' . $chili_db. '.issues';
  $result = db_query($sql);
	while ($rc = $result ->fetchObject()){
		$project_id = $rc -> project_id;
    $sql = new EntityFieldQuery();
	  $sql->entityCondition('entity_type', 'node')
	  	->entityCondition('bundle', 'oa_section')
      ->fieldCondition('field_oa_section', 'tid', $tid, '=')
	    ->fieldCondition('field_chili_project_id', 'value', $project_id, '=');
	  $result2 = $sql->execute();
		$section_node = null;
		if ($result2){
			$section_node = node_load(array_shift((array_keys($result2['node']))));
		}
		else{
			$section_node =  entity_create('node', array('type' => 'oa_section'));
		}
    $section_node -> field_view_field[$section_node->language][0] = array('vname'=>'customized_work_tracker|view_field_for_section', 'vargs'=>'[node:nid]');

    $section_wrapper = entity_metadata_wrapper('node',$section_node);
		$section_wrapper -> title = TASK_SECTION_TITLE;
		//$space_wrapper -> body = array('value'=>$description, 'text_processing'=>1; 'format' => 'panopoly_wysiwyg_text');
		$section_wrapper -> field_oa_section_override = 0;
    $tid = array_shift(taxonomy_get_term_by_name('Task Section', 'section_type')) -> tid;
    $section_wrapper -> field_oa_section = $tid;
    $sql = new EntityFieldQuery();
	  $sql->entityCondition('entity_type', 'node')
	  	->entityCondition('bundle', 'oa_space')
	    ->fieldCondition('field_chili_project_id', 'value', $project_id, '=');
	  $result3 = $sql->execute();

		if(!$result3){
			drupal_set_messasge("Parent space (chili project id: $project_id) could not be found.");
			countinue;
		}
    $parent_node = node_load(array_shift((array_keys($result3['node']))));
		$section_wrapper -> og_group_ref = $parent_node;
    $section_wrapper -> field_oa_node_types->offsetSet(0, 'oa_worktracker_task');

    $section_wrapper -> field_chili_project_id = $project_id;
    $section_wrapper -> author = $user;
    //$section_wrapper -> field_view_field = array('vname'=>'customized_work_tracker|view_field_for_section', 'vargs'=>'[node:nid]');
		$section_wrapper -> save();
	}
	//return;
  // wiki sections
  $tid = array_shift(taxonomy_get_term_by_name('Wiki Section', 'section_type')) -> tid;
	$sql = 'select distinct project_id from ' . $chili_db. '.wikis';
  $result = db_query($sql);
	while ($rc = $result ->fetchObject()){
		$project_id = $rc -> project_id;
    $sql = new EntityFieldQuery();
	  $sql->entityCondition('entity_type', 'node')
	  	->entityCondition('bundle', 'oa_section')
      ->fieldCondition('field_oa_section', 'tid', $tid, '=')
	    ->fieldCondition('field_chili_project_id', 'value', $project_id, '=');
	  $result2 = $sql->execute();
		$section_node = null;
		if ($result2){
			$section_node = node_load(array_shift((array_keys($result2['node']))));
		}
		else{
			$section_node =  entity_create('node', array('type' => 'oa_section'));
		}
    $section_wrapper = entity_metadata_wrapper('node',$section_node);
		$section_wrapper -> title = WIKI_SECTION_TITLE;
		//$space_wrapper -> body = array('value'=>$description, 'text_processing'=>1; 'format' => 'panopoly_wysiwyg_text');
		$section_wrapper -> field_oa_section_override = 0;
    $tid = array_shift(taxonomy_get_term_by_name('Wiki Section', 'section_type')) -> tid;
    $section_wrapper -> field_oa_section = $tid;
    $sql = new EntityFieldQuery();
	  $sql->entityCondition('entity_type', 'node')
	  	->entityCondition('bundle', 'oa_space')
	    ->fieldCondition('field_chili_project_id', 'value', $project_id, '=');
	  $result3 = $sql->execute();

		if(!$result3){
			drupal_set_messasge("Parent space (chili project id: $project_id) could not be found.");
			countinue;
		}
    $parent_node = node_load(array_shift((array_keys($result3['node']))));
		$section_wrapper -> og_group_ref = $parent_node;
    $section_wrapper -> field_oa_node_types->offsetSet(0, 'nhpd_wiki');

    $section_wrapper -> field_chili_project_id = $project_id;
    $section_wrapper -> author = $user;
		$section_wrapper -> save();
	}
	drupal_set_message('The creation of spaces and sections has done.');
	//statuses, trackers, and prioirty
  com_transfer_status_tracker_priority();
	//custom fields conditional field mapping
  com_custom_fields_conditional_field_mapping();


}
function com_transfer_status_tracker_priority(){
  global $temp_attachment_file_folder;
	global $offcial_attacment_file_folder;
	global $chili_db;
	global $custom_fields_mapping;
	//Tracker, Chiliproject trackers are translated to OA task type as key-value pair tracker name - tracker name
	$sql = 'select * from ' . $chili_db. '.trackers order by position';
  $result = db_query($sql);

	$options_text = '';
	$options_value = '';
	$default_value_text = 'Task';
	$default_value_field = 'Task';
	$variable_name = "oa_worktracker_allowed_values_task_type";
	$setting = variable_get($variable_name, null);
	$trackers = array();
	while ($rc = $result ->fetchObject()){
		$name = $rc -> name;
		$id = $rc -> id;
		$setting['options'][$name] = $name;
		$trackers[$id] = $name;
	}
	foreach($setting['options'] as $key => $value){
		$options_text = $options_text . "$key|$value\n";
	}
	$setting = array('options' => $setting['options'], 'options_text' => $options_text, 'options_field' => $options_text, 'default_value' => $default_value_field, 'default_value_text' => $default_value_text);
  variable_del($variable_name);
	variable_set($variable_name, $setting);
  drupal_set_message(_dump( $variable_name, 'purple', TRUE));
  drupal_set_message(_dump( $setting, 'purple', TRUE));
  drupal_set_message(_dump( $id, 'purple', TRUE));
   drupal_set_message(_dump( $trackers, 'purple', TRUE));

	// setting for types (tracker) for task sections
  $variable_name = "nhpd_oa_task_type_allowed_values_setting";
  $sql = 'select * from ' . $chili_db. '.projects_trackers';
  $result = db_query($sql);
  $options = array();
  $tid = array_shift(taxonomy_get_term_by_name('Task Section', 'section_type')) -> tid;
  while ($rc = $result ->fetchObject()){
		$project_id = $rc -> project_id;
		$tracker_id = $rc -> tracker_id;
    $sql2 = new EntityFieldQuery();
	  $sql2->entityCondition('entity_type', 'node')
	  	->entityCondition('bundle', 'oa_section')
      ->fieldCondition('field_oa_section', 'tid', $tid, '=')
	    ->fieldCondition('field_chili_project_id', 'value', $project_id, '=');
	  $result2 = $sql2->execute();
		if ($result2){
			$section_node_nid = array_shift((array_keys($result2['node'])));
			$options[$section_node_nid][] = $trackers[$tracker_id];
		}
	}
	$setting = array('setting' => $options, 'all_options' => $setting['options']);
	variable_del($variable_name);
  variable_set($variable_name, $setting);
  //  variable_set($variable_name, array());
  drupal_set_message(_dump( $variable_name, 'purple', TRUE));
  drupal_set_message(_dump( $setting, 'purple', TRUE));
//return;
// priority
  $variable_name = "oa_worktracker_allowed_values_priority";
	$sql = 'select * from ' . $chili_db. '.enumerations where type =:IssuePriority';
  $result = db_query($sql, array(':IssuePriority' => 'IssuePriority'));
  $setting = variable_get($variable_name, null);
  $options_text = '';
	$options_value = '';
	$default_value_text = '';
	$default_value_field = '';

  while ($rc = $result ->fetchObject()){
		//$name = str_replace(' ', '',str_replace(')', '',str_replace('(', '',$rc -> name)));
		$name = preg_replace( '/[\(\)\d\s]/', '', $rc -> name);
		$id = $rc -> id;
    $setting['options'][$id] = $name;
		if ($name == 'Normal'){
      $default_value_text = $id;
			$default_value_field = $id;
		}
	}
  foreach($setting['options'] as $key => $value){
		$options_text = $options_text . "$key|$value\n";
	}
	$setting = array('options' => $setting['options'], 'options_text' => $options_text, 'options_field' => $options_text, 'default_value' => $default_value_field, 'default_value_text' => $default_value_text);
  variable_del($variable_name);
	variable_set($variable_name, $setting);

  drupal_set_message(_dump( $variable_name, 'purple', TRUE));
  drupal_set_message(_dump( $setting, 'purple', TRUE));
  // status
  $variable_name = "oa_worktracker_allowed_values_task_status";
  $sql = 'select * from ' . $chili_db. '.issue_statuses';
  $result = db_query($sql);
  $setting = variable_get($variable_name, null);
  $options_text = '';
	$options_value = '';
	$default_value_text = '';
	$default_value_field = '';
  while ($rc = $result ->fetchObject()){
		$name = $rc -> name;
		$id = $rc -> id;
    $setting['options'][strtolower($name)] = $name;
		if ($rc->is_default){
      $default_value_text = strtolower($name);
			$default_value_field = strtolower($name);
		}
	}
  foreach($setting['options'] as $key => $value){
		$options_text = $options_text . "$key|$value\n";
	}
	$setting = array('options' => $setting['options'], 'options_text' => $options_text, 'options_field' => $options_text, 'default_value' => $default_value_field, 'default_value_text' => $default_value_text);
  variable_del($variable_name);
	variable_set($variable_name, $setting);

  drupal_set_message(_dump( $variable_name, 'purple', TRUE));
  drupal_set_message(_dump( $setting, 'purple', TRUE));
  // TODo: setting for statuses  for task sections
	/*
  $variable_name = "nhpd_oa_task_status_allowed_values_setting";
  $sql = 'select * from ' . $chili_db. '.issue_categories';
  $result = db_query($sql);
  $options = array();
  $tid = array_shift(taxonomy_get_term_by_name('Task Section', 'section_type')) -> tid;
  while ($rc = $result ->fetchObject()){
		$project_id = $rc -> project_id;
		$name = $rc -> name;
    $sql2 = new EntityFieldQuery();
	  $sql2->entityCondition('entity_type', 'node')
	  	->entityCondition('bundle', 'oa_section')
      ->fieldCondition('field_oa_section', 'tid', $tid, '=')
	    ->fieldCondition('field_chili_project_id', 'value', $project_id, '=');
	  $result2 = $sql2->execute();
		if ($result2){
			$section_node_nid = array_shift((array_keys($result2['node'])));
			$options[$section_node_nid][] = $name;
		}
	}
	$setting = array('setting' => $options, 'all_options' => $setting['options']);
	variable_del($variable_name);
  variable_set($variable_name, $setting);
	*/
  //  variable_set($variable_name, array());
  drupal_set_message(_dump( $variable_name, 'purple', TRUE));
  drupal_set_message(_dump( $setting, 'purple', TRUE));

	// task category

	$sql = 'select distinct name from ' . $chili_db. '.issue_categories';
  $result = db_query($sql);

	$options_text = '';
	$options_value = '';
	$default_value_text = 'General';
	$default_value_field = 'General';
	$variable_name = "oa_worktracker_allowed_values_task_category";
	$setting = variable_get($variable_name, null);
	$trackers = array();
	while ($rc = $result ->fetchObject()){
		$name = $rc -> name;
		$setting['options'][$name] = $name;
	}
	foreach($setting['options'] as $key => $value){
		$options_text = $options_text . "$key|$value\n";
	}
	$setting = array('options' => $setting['options'], 'options_text' => $options_text, 'options_field' => $options_text, 'default_value' => $default_value_field, 'default_value_text' => $default_value_text);
  variable_del($variable_name);
	variable_set($variable_name, $setting);
  drupal_set_message(_dump( $variable_name, 'purple', TRUE));
  drupal_set_message(_dump( $setting, 'purple', TRUE));
  drupal_set_message(_dump( $id, 'purple', TRUE));
   drupal_set_message(_dump( $trackers, 'purple', TRUE));

	// setting for types (tracker) for task sections
  $variable_name = "nhpd_oa_task_category_allowed_values_setting";
  $sql = 'select * from ' . $chili_db. '.issue_categories';
  $result = db_query($sql);
  $options = array();
  $tid = array_shift(taxonomy_get_term_by_name('Task Section', 'section_type')) -> tid;
  while ($rc = $result ->fetchObject()){
		$project_id = $rc -> project_id;
		$name = $rc -> name;
    $sql2 = new EntityFieldQuery();
	  $sql2->entityCondition('entity_type', 'node')
	  	->entityCondition('bundle', 'oa_section')
      ->fieldCondition('field_oa_section', 'tid', $tid, '=')
	    ->fieldCondition('field_chili_project_id', 'value', $project_id, '=');
	  $result2 = $sql2->execute();
		if ($result2){
			$section_node_nid = array_shift((array_keys($result2['node'])));
			$options[$section_node_nid][] = $name;
		}
	}
	$setting = array('setting' => $options, 'all_options' => $setting['options']);
	variable_del($variable_name);
  variable_set($variable_name, $setting);
  //  variable_set($variable_name, array());
  drupal_set_message(_dump( $variable_name, 'purple', TRUE));
  drupal_set_message(_dump( $setting, 'purple', TRUE));
//return;
}


function com_load_chiliproject_wiki_data_form(&$form_state = NULL){
	$form = array();

  $mapping = isset($form_state['values']['mapping_chili_project_id']) ? $form_state['values']['mapping_chili_project_id'] : 0;
  $form['mapping_chili_project_id']=array(
    '#type' => 'checkbox',
    '#default_value' => $mapping,
    '#title' => "Map chili project id to the Wiki sections? ",
	);
  $round2 = isset($form_state['values']['round2']) ? $form_state['values']['round2'] : 0;
	$form['round2']=array(
    '#type' => 'checkbox',
    '#default_value' => $round2,
    '#title' => "Round2 only?",
	);
 $test = isset($form_state['values']['test']) ? $form_state['values']['test'] : 0;
	$form['test']=array(
    '#type' => 'checkbox',
    '#default_value' => $test,
    '#title' => "Testing? (first 1 record)",
	);
  $reload = isset($form_state['values']['reload']) ? $form_state['values']['reload'] : 0;
	$form['reload']=array(
    '#type' => 'checkbox',
    '#default_value' => $reload,
    '#title' => "Reload data?",
	);
  $form['action-submit']=array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#attributes' => array('onclick' => 'return confirm("You are going to migrate all issues from chiliporject to OA. Are you sure you wish to continue?");'),
  );

  if ($form_state['clicked_button']['#id'] == 'edit-action-submit') {
  	if ($mapping){
			drupal_set_message("Stating mapping chili porject id to the Task sections...");
	    mapping_chili_project_id_to_task_sections('Wiki Section');
	    drupal_set_message("Mapping chili porject id to the Task sections is done.");
		}
    $sql = "select id from " . $chili_db. ".wiki_pages order by id";
		if ($test) $sql = "select id from " . $chili_db. ".wiki_pages order by id limit 100";
		$result = db_query($sql);
		$operations = array();
		if(!$round2){
			while ($rc = $result ->fetchObject()){
				$id = $rc -> id;
		    $operations[] = array('load_chiliproject_wiki_data_batch', array($id, $reload, 1));
			}
		}
    $result = db_query($sql);
    while ($rc = $result ->fetchObject()){
			$id = $rc -> id;
			$round = 2;
	    $operations[] = array('load_chiliproject_wiki_data_batch', array($id, $reload, $round));
		}
	  $batch = array(
	    'operations' => $operations,
	    'finished' => 'batch_op_finished',
	    // We can define custom messages instead of the default ones.
	    'title' => t('Migrating chili Wiki'),
	    'init_message' => t('Migrating starting...'),
	    'progress_message' => t('Processed @current out of @total steps.'),
	    'error_message' => t('Migrating has encountered an error.'),
	  );
	  batch_set($batch);
	  batch_process();
    //drupal_set_message(_dump($operations, 'purple', TRUE));

	}

	return $form;
}
function com_load_chiliproject_wiki_data_batch($id, $reload,$round, $content){
  //drupal_set_message(_dump("$id, $testing,$round", 'purple', TRUE));
	$message = '';
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_file'] = 0;
    $context['sandbox']['max'] = count($sheets);
  }
	if ($round == 2) {
		com_load_chiliproject_wiki_data_2($id);
	}
	else{
    com_load_chiliproject_wiki_data($id, $reload);
	}
  // Inform the batch engine that we are not finished,
  // and provide an estimation of the completion level we reached.
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}
/**
 *$reload: 1 - reloads wiki text from chiliproject; 0 - changes viki text from OA2
 */

function com_load_chiliproject_wiki_data($id, $reload=0){
  global $temp_attachment_file_folder;
	global $offcial_attacment_file_folder;
	global $chili_db;
	global $custom_fields_mapping;
	$sql = "select w.*, wp.*, wc.*, au.login as author,  p.name as project_name from  ". $chili_db. ".wikis w LEFT JOIN ". $chili_db. ".wiki_pages wp on w.id=wp.wiki_id LEFT JOIN ". $chili_db. ".wiki_contents wc on wp.id=wc.page_id LEFT JOIN ". $chili_db. ".users au on wc.author_id=au.id LEFT JOIN ". $chili_db. ".projects p on w.project_id=p.id where wp.id=:id";
  //drupal_set_message(_dump($sql, 'purple', TRUE));
//return;
	if (!$id) return;
	$result = db_query($sql, array(':id' => $id));
	if ($result->rowCount() < 1){
    drupal_set_message("Chili issue $id cannot be found. Skipped");
		return;
	}


	while ($rc = $result->fetchObject()){
	  //drupal_set_message(_dump($rc, 'purple', TRUE));

		$project_id = $rc -> project_id;
	  $project_name = $rc -> project_name;
		$start_page = $rc -> start_page;
		$title = $rc -> title;
    $created_on = $rc -> created_on;
		$protected = $rc -> protected;
    $author = $rc -> author;
		$text = $rc -> text;
    $updated_on = $rc -> updated_on;
		$parent_id = $rc -> parent_id;
		$page_id = $rc -> page_id;
    drupal_set_message(_dump($page_id, 'purple', TRUE));
    drupal_set_message(_dump($title, 'purple', TRUE));

		if(!$title) continue;


		//oa wiki
		$sql = new EntityFieldQuery();
	  $sql->entityCondition('entity_type', 'node')
	  	->entityCondition('bundle', 'nhpd_wiki')
	    ->fieldCondition('field_chili_id', 'value', $page_id, '=');
	  $result2 = $sql->execute();
		$wiki_node = null;

		if ($result2){
			$wiki_node = node_load(array_shift((array_keys($result2['node']))));
		  if (!$reload){
						$text = $wiki_node->field_wiki_page['und'][0]['value'];
			}
		}
		else{
			$wiki_node =  entity_create('node', array('type' => 'nhpd_wiki'));
		}
    $wiki_wrapper = entity_metadata_wrapper('node',$wiki_node);
		//return;
	   // oa_section_ref
    $tid = array_shift(taxonomy_get_term_by_name('Wiki Section', 'section_type')) -> tid;
	  $sql = new EntityFieldQuery();
	  $sql->entityCondition('entity_type', 'node')
	  	->entityCondition('bundle', 'oa_section')
	    ->fieldCondition('field_chili_project_id', 'value', $project_id, '=')
			->fieldCondition('field_oa_section', 'tid', $tid, '=');
	  $result2 = $sql->execute();
		if ($result2){
	    $section_nid = array_shift((array_keys($result2['node'])));
	  	$wiki_wrapper -> oa_section_ref = $section_nid;
		}
		else{
	    drupal_set_message("Task section under $project_name cannot be found. The issue is skipped.");
			continue;
		}
		//return;
	  // oa_group_ref
		$space = com_node_load_by_title($project_name, 'oa_space');
		if ($space){
	    $wiki_wrapper -> og_group_ref = $space -> nid;
		}
		else{
	    drupal_set_message("Space $project_name cannot be found. og_group_ref is not assined.");
		}

		//procesing TOC - {{toc}}
		// Parses {{toc}} to [toc title] of drupla toc filter
		$pattern = '/h1\..+\n/';
		$matches = array();
    preg_match($pattern, $text, $matches);
		if (!empty($matches)){
			$h1 = trim(str_replace('h1.', '', $matches[0]));
		}
		$text = str_replace('{{toc}}', "[toc $h1]", $text);


    //procesing image link - !{attributes}img.png!
		// Parses !img.png! to !system/files/img.png
		$pattern = '/\![^\!]+\!/';
		$matches = array();
    preg_match_all($pattern, $text, $matches);
		$appended = 'system/files';
		foreach ($matches[0] as $match){
			if(preg_match('/^\!system\/files\//', $match)) continue; // skips parsed img links
			$pattern = '/\{.+\}/';
			$matches2 = array();
      //drupal_set_message(_dump($match, 'purple', TRUE));
      preg_match($pattern, $match, $matches2);
      //drupal_set_message(_dump($matches2, 'purple', TRUE));
			$file_name = '';
			$attributes = '';
			if($matches2[0]){
				$attributes = $matches2[0];
				$file_name = str_replace('!', '', str_replace($attributes, '', $match));
        drupal_set_message(_dump($match, 'purple', TRUE));

			}
			else{
        $file_name = str_replace('!', '', $match);
			}
			//drupal_set_message(_dump($file_name, 'purple', TRUE));
			$sql = "select * from " . $chili_db . ".attachments where container_type = 'WikiPage' and container_id = :container_id and filename=:filename";

      $result_img = db_query($sql, array(':container_id' => $page_id, ':filename' => $file_name));

			while ($rc_img = $result_img -> fetchObject()){
				$filename = $rc_img -> filename;
	      $disk_filename = $rc_img -> disk_filename;
	      $filesize = $rc_img -> filesize;
	      $content_type = $rc_img -> content_type;
	      $created_on = $rc_img -> created_on;
	      $description = $rc_img -> description;

				$tmp_uri = $temp_attachment_file_folder . $disk_filename;
        $uri = $officail_attachment_file_folder . $disk_filename;
        //drupal_set_message(_dump(drupal_realpath($uri), 'purple', TRUE));
        $file_temp = file_get_contents(drupal_realpath($tmp_uri));
        $doc = file_save_data($file_temp, $uri, FILE_EXISTS_REPLACE);
				$link = "!$attributes" . "$appended/$disk_filename($filename)!";
        drupal_set_message(_dump($link, 'purple', TRUE));
        $text = str_replace($match, $link, $text);
			}

		}

	  $wiki_wrapper -> title = $title;
		$wiki_wrapper -> field_wiki_page = array('value' => $text, 'text_processing' => 1, 'format' => 'nhpd_wiki');
	  $wiki_wrapper -> created = strtotime($created_on);
	  $wiki_wrapper -> status = 1;
	  $wiki_wrapper -> field_chili_id = $page_id;
		if (str_replace(' ', '_', $start_page) == $title){
      $wiki_wrapper -> field_is_first_page = 1;
		}

		$author_id = user_load_by_name($author) -> uid;
		if ($author_id){
	    $wiki_wrapper -> author = $author_id;
		}
		else{
			drupal_set_message("User $author cannot be found. Author is not assigned.");
		}
    //drupal_set_message(_dump((array)$wiki_wrapper, 'purple', TRUE));
    $wiki_wrapper -> save();

    drupal_set_message("Wiki ($page_id) has been successfully transferred.");

 }
}

function com_load_chiliproject_wiki_data_2($id){
  global $temp_attachment_file_folder;
	global $offcial_attacment_file_folder;
	global $chili_db;
	global $custom_fields_mapping;
	if (!$id) return;
	$sql = "select wp.*, wc.* from  " . $chili_db . ".wiki_pages wp LEFT JOIN ". $chili_db . ".wiki_contents wc on wp.id=wc.page_id  where wp.id=:id";
  //drupal_set_message(_dump($sql, 'purple', TRUE));

	//return;
	$result = db_query($sql, array(':id' => $id));
	if ($result->rowCount() < 1){
    drupal_set_message("Chili wiki $id cannot be found. Skipped");
		return;
	}

	while ($rc = $result->fetchObject()){
		$title = $rc -> title;
    $created_on = $rc -> created_on;
		$protected = $rc -> protected;
		//$text = $rc -> text;
    $updated_on = $rc -> updated_on;
		$parent_id = $rc -> parent_id;
		$page_id = $rc -> page_id;
		$wiki_id = $rc -> wiki_id;


    //drupal_set_message(_dump($page_id, 'purple', TRUE));
    //drupal_set_message(_dump($wiki_id, 'purple', TRUE));
		//oa wiki
		$sql = new EntityFieldQuery();
	  $sql->entityCondition('entity_type', 'node')
	  	->entityCondition('bundle', 'nhpd_wiki')
	    ->fieldCondition('field_chili_id', 'value', $page_id, '=');
	  $result2 = $sql->execute();
		$wiki_node = null;

		if (!$result2){
			drupal_set_message("Wiki node wiht $title ($page_id) could not be found.");
			continue;
		}

    $wiki_node = node_load(array_shift((array_keys($result2['node']))));
		$text = $wiki_node->field_wiki_page['und'][0]['value'];
    //drupal_set_message(_dump((array)$text, 'purple', TRUE));
    $wiki_wrapper = entity_metadata_wrapper('node',$wiki_node);

		// parses internal links [[wiki title]]to freelinling syntax [[nid:n]]
    //$pattern = '/[^!]\[\[[\w\s\d\/\-]+\]\]/';
    $pattern = '/[^!]\[\[[^\[^\]]+\]\]/';
		$matches = array();

    preg_match_all($pattern, $text, $matches);
    //drupal_set_message(_dump($matches, 'purple', TRUE));
		foreach($matches[0] as $match){
			$linked_page_id = null;
      $likined_wiki_nid = null;
			$match = preg_replace('/^.\[\[/', '[[', $match);
			$linked_text = str_replace(']]', '', str_replace('[[', '', $match));
			if(preg_match('/^nid:\d+/', $linked_text)) continue; //skips parsed links
      $linked_title = com_titleize($linked_text);
      //drupal_set_message(_dump($match, 'purple', TRUE));
      //drupal_set_message(_dump($linked_text, 'purple', TRUE));
			$sql3 = "select wp.id from " . $chili_db . ".wiki_pages wp where  wp.wiki_id = :wiki_id and wp.title = :title ";
      $result3 = db_query($sql3, array(':wiki_id'=>$wiki_id, ':title'=> $linked_title));
			$rc3 = $result3 ->fetchObject();
      //drupal_set_message(_dump($sql3, 'purple', TRUE));
      //drupal_set_message(_dump($linked_title, 'purple', TRUE));

			if ($rc3){
        $linked_page_id = $rc3 -> id;
			}
			else{ // check wiki_redirects


				$sql5 = "select wp.id as page_id, wp.title as title  from "  . $chili_db . ".wiki_pages wp JOIN, " . $chili_db . ".wiki_redirects wr on wp.wiki_id = wr.wiki_id and wp.title= wr.redirects_to where wr.wiki_id = :wiki_id and wr.title = :title ";
	      //drupal_set_message(_dump($sql5, 'purple', TRUE));
	      //drupal_set_message(_dump($wiki_id, 'purple', TRUE));
	      //drupal_set_message(_dump($linked_title, 'purple', TRUE));
				$result5 = db_query($sql3, array(':wiki_id'=>$wiki_id, ':title'=> $linked_title));
				$rc5 = $result ->fetchObject();
	      //drupal_set_message(_dump($rc5, 'purple', TRUE));
				if ($rc5){
					$linked_title=$rc5->title;
					$linked_page_id = $rc5 -> page_id;
	        $linked_text = str_replace('_', ' ', $rc5 -> title);
				}
     }
      $sql4 = new EntityFieldQuery();
		  $sql4->entityCondition('entity_type', 'node')
		  	->entityCondition('bundle', 'nhpd_wiki')
		    ->fieldCondition('field_chili_id', 'value', $linked_page_id, '=');
		  $result4 = $sql4->execute();
      $likined_wiki_nid =  array_shift((array_keys($result4['node'])));
      //drupal_set_message(_dump($likined_wiki_nid, 'purple', TRUE));
			if ($likined_wiki_nid){
				$link = "[[nid:$likined_wiki_nid|$linked_text]]";
				$text = str_replace($match, $link, $text);
			}

		}
    //drupal_set_message(_dump((array)$text, 'purple', TRUE));

    // parses issue links #issue_id to freelinling syntax [[nid:n]]
		$pattern = '/#\d+/';
		$matches = array();
    preg_match_all($pattern, $text, $matches);
		foreach($matches[0] as $match){
			$issue_id = trim(str_replace('#', '', $match));
      $sql = new EntityFieldQuery();
		  $sql->entityCondition('entity_type', 'node')
		  	->entityCondition('bundle', 'oa_worktracker_task')
		    ->fieldCondition('field_chili_id', 'value', $issue_id, '=');
		  $result6 = $sql->execute();
			$task_node_id = array_shift((array_keys($result6['node'])));
      //drupal_set_message(_dump($match, 'purple', TRUE));
      //drupal_set_message(_dump($task_node_id, 'purple', TRUE));
			if ($task_node_id){
				$link = "[[nid:$task_node_id|$task_node_id]]";
        $text = str_replace($match, $link, $text);
			}
      //drupal_set_message(_dump((array)$text, 'purple', TRUE));
		}
    $wiki_wrapper -> field_wiki_page = array('value' => $text, 'text_processing' => 1, 'format' => 'nhpd_wiki');
    //drupal_set_message(_dump($page_id, 'purple', TRUE));
    //drupal_set_message(_dump((array)$text, 'purple', TRUE));

		//parent pages
		/*
		$sql = "select subpages.id as subpage_id, subpages.wiki_id  as subpage_wiki_id, subpages.title as subpage_title from ". $chili_db . ".wiki_pages subpages where parent_id=:parent_id";
    $result_subpage = db_query($sql, array(':parent_id' => $page_id));
    //drupal_set_message(_dump($page_id, 'purple', TRUE));
    //drupal_set_message(_dump($sql, 'purple', TRUE));
		while ($rc_subpage = $result_subpage -> fetchObject()){
			$subpage_id = $rc_subpage -> subpage_id;
      $subpage_wiki_id = $rc_subpage -> subpage_wiki_id;
      $subpage_title = $rc_subpage -> subpage_title;
      //drupal_set_message(_dump($subpage_title, 'purple', TRUE));
      //drupal_set_message(_dump($subpage_wiki_id, 'purple', TRUE));
      //drupal_set_message(_dump($subpage_id, 'purple', TRUE));
			//redirect
      $sql = "select wp.id as page_id  from "  . $chili_db . ".wiki_pages wp JOIN " . $chili_db . ".wiki_redirects wr on wp.wiki_id = wr.wiki_id and wp.title= wr.redirects_to where wr.wiki_id = :wiki_id and wr.title = :title ";
      //drupal_set_message(_dump($sql, 'purple', TRUE));
			$result_subpage3 = db_query($sql, array(':wiki_id'=>$subpage_wiki_id, ':title'=> $subpage_title));
			$rc_subpage3 = $result_subpage3 ->fetchObject();
      //drupal_set_message(_dump($rc_subpage3, 'purple', TRUE));
			if ($rc_subpage3){
				$subpage_id = $rc_subpage3 -> page_id;
			}
      $sql = new EntityFieldQuery();
		  $sql->entityCondition('entity_type', 'node')
		  	->entityCondition('bundle', 'nhpd_wiki')
		    ->fieldCondition('field_chili_id', 'value', $subpage_id, '=');
		  $result_subpage2 = $sql->execute();
      $subpage_node = null;
			if ($result_subpage2){
				$subpage_node = node_load(array_shift((array_keys($result_subpage2['node']))));
        $subpages = $wiki_wrapper -> field_subpage -> value(array('identifier' => TRUE));
		    if (!in_array($subpage_node -> nid, $subpages)){
					$wiki_wrapper -> field_subpage ->offsetSet( $wiki_wrapper -> field_subpage ->count(), $subpage_node);
				}
			}
      else {
	      drupal_set_message("Subpage (page_id: $subpage_id) could not found.");
			}
		}
		*/
    //drupal_set_message(_dump($parent_id, 'purple', TRUE));
    if ($parent_id){
	    $sql = new EntityFieldQuery();
		  $sql->entityCondition('entity_type', 'node')
		  	->entityCondition('bundle', 'nhpd_wiki')
		    ->fieldCondition('field_chili_id', 'value', $parent_id, '=');
			  $result2 = $sql->execute();
				$parent_node = null;
				if ($result2){
					$parent_node = node_load(array_shift((array_keys($result2['node']))));
					$wiki_wrapper -> field_parent_page -> set($parent_node);
				}
				else{
		      drupal_set_message("Parent task($parent_id) for $subject cannot be found. The issue is skipped.");
					return;
				}
		}
    $wiki_wrapper -> save();
    //drupal_set_message(_dump((array)$wiki_wrapper, 'purple', TRUE));
    drupal_set_message("Wiki  ($page_id) has been successfully updated.");

 }

}


/**
 * Based on http://drupal.stackexchange.com/a/34400
 */

/**
 * Helper function; Load node by title
 */

 function com_node_load_by_title($title, $node_type) {
  $query = new EntityFieldQuery();
  $entities = $query->entityCondition('entity_type', 'node')
    ->propertyCondition('type', $node_type)
    ->propertyCondition('title', $title)
    ->propertyCondition('status', 1)
    ->range(0,1)
    ->execute();
  if(!empty($entities)) {
    return node_load(array_shift(array_keys($entities['node'])));
  }
}
/**
 *
 *
 */

function com_custom_fields_conditional_field_mapping(){
  global $temp_attachment_file_folder;
	global $offcial_attacment_file_folder;
	global $chili_db;
	global $custom_fields_mapping;
	$chili_custom_field_id_oa_field_name_mapping = chili_custom_field_id_oa_field_name_mapping();
  $dependencies = conditional_fields_load_dependencies('node', 'oa_worktracker_task');
  $tid = array_shift(taxonomy_get_term_by_name('Task Section', 'section_type')) -> tid;
	// depdendee: oa_section_ref
  $dependee_instance = field_read_instance('node', 'oa_section_ref', 'oa_worktracker_task');
	$dependee_id = $dependee_instance['id'];
	$basic_options = conditional_fields_dependency_default_options();
	$basic_options['values_set'] = 3;
  $basic_options['value_form'] = '_none';
	foreach($chili_custom_field_id_oa_field_name_mapping as $chili_custom_field_id => $oa_custom_field_name){
    drupal_set_message(_dump( "$chili_custom_field_id => $oa_custom_field_name", 'purple', TRUE));
		// if custom field is for all project, skip
		$sql = 'select is_for_all from ' . $chili_db . '.custom_fields where id = :id';
		$result =db_query($sql, array(':id' => $chili_custom_field_id));
		if($result->fetchColumn(0)){
			continue;
		}

    $dependent_instance = field_read_instance('node', $oa_custom_field_name, 'oa_worktracker_task');
		$dependent_id = $dependent_instance['id'];
		$sql = 'select * from ' . $chili_db . '.custom_fields_projects where custom_field_id=:id';
		$result = db_query($sql, array(':id' => $chili_custom_field_id));
		$options_values = array();
    $options_values[] = 0; // in case the custom field is not used for any project, it should be visible to project nid 0, which does not exist.
		while ($rc = $result -> fetchObject()){
			$project_id = $rc -> project_id;
      $sql = new EntityFieldQuery();
		  $sql->entityCondition('entity_type', 'node')
		  	->entityCondition('bundle', 'oa_section')
        ->fieldCondition('field_chili_project_id', 'value', $project_id, '=')
	      ->fieldCondition('field_oa_section', 'tid', $tid, '=');
		  $result2 = $sql->execute();
			$section_node = null;
			if ($result2){
				$section_id = array_shift((array_keys($result2['node'])));
        $options_values[] = $section_id;
			}
		}
		$dependency_id = null;
		$options = array();
		$result_options = db_select('conditional_fields', 'cf')
    ->fields('cf', array('id', 'dependee', 'dependent', 'options'))
    ->condition('dependee', $dependee_id)
    ->condition('dependent', $dependent_id)
    ->execute()
    ->fetchAssoc();
		if ($result_options){
      $options = unserialize($result_options['options']);
      $dependency_id = $result_options['id'];
		}
    $options['values'] = $options_values;

    if ($dependency_id){
			$dependency = array(
			'id'        => $dependency_id,
			'dependee'  => $dependee_id,
	    'dependent' => $dependent_id,
	    'options'   => $options + $basic_options,
	  	);
	    drupal_set_message(_dump( $dependency, 'purple', TRUE));
	    drupal_write_record('conditional_fields', $dependency, 'id');
		}
		else{
      $dependency = array(
			'dependee'  => $dependee_id,
	    'dependent' => $dependent_id,
	    'options'   => $options + $basic_options,
	  	);
	    drupal_set_message(_dump( $dependency, 'purple', TRUE));
	    drupal_write_record('conditional_fields', $dependency);
		}
	}

  // depdendee: field_oa_worktracker_task_type
  $dependee_instance = field_read_instance('node', 'field_oa_worktracker_task_type', 'oa_worktracker_task');
	$dependee_id = $dependee_instance['id'];
	reset($chili_custom_field_id_oa_field_name_mapping);
	foreach($chili_custom_field_id_oa_field_name_mapping as $chili_custom_field_id => $oa_custom_field_name){
    drupal_set_message(_dump( $oa_custom_field_name, 'purple', TRUE));
    	// if custom field is for all project, skip
		$sql = 'select is_for_all from ' . $chili_db . '.custom_fields where id = :id';
		$result =db_query($sql, array(':id' => $chili_custom_field_id));
		if($result->fetchColumn(0)){
			continue;
		}
    $dependent_instance = field_read_instance('node', $oa_custom_field_name, 'oa_worktracker_task');
		$dependent_id = $dependent_instance['id'];
		$sql = 'select * from ' . $chili_db . '.custom_fields_trackers ft, ' . $chili_db . '.trackers t where t.id = ft.tracker_id and custom_field_id=:id';
		$result = db_query($sql, array(':id' => $chili_custom_field_id));
		$options_values = array();
		while ($rc = $result -> fetchObject()){
			$options_values[] = $rc -> name;
 		}
		$dependency_id = null;
		$options = array();
		$result_options = db_select('conditional_fields', 'cf')
    ->fields('cf', array('id', 'dependee', 'dependent', 'options'))
    ->condition('dependee', $dependee_id)
    ->condition('dependent', $dependent_id)
    ->execute()
    ->fetchAssoc();
		if ($result_options){
      $options = unserialize($result_options['options']);
      $dependency_id = $result_options['id'];
		}
    $options['values'] = $options_values;
    if ($dependency_id){
			$dependency = array(
			'id'        => $dependency_id,
			'dependee'  => $dependee_id,
	    'dependent' => $dependent_id,
	    'options'   => $options + $basic_options,
	  	);
	    drupal_set_message(_dump( $dependency, 'purple', TRUE));
	    drupal_write_record('conditional_fields', $dependency, 'id');
		}
		else{
      $dependency = array(
			'dependee'  => $dependee_id,
	    'dependent' => $dependent_id,
	    'options'   => $options + $basic_options,
	  	);
	    drupal_set_message(_dump( $dependency, 'purple', TRUE));
	    drupal_write_record('conditional_fields', $dependency);
		}
	}
}

function com_get_oa_custom_field_name($chili_custom_field_id){
	$chili_custom_field_id_oa_field_name_mapping = chili_custom_field_id_oa_field_name_mapping();

	return $chili_custom_field_id_oa_field_name_mapping[$chili_custom_field_id];
}
function com_chili_custom_field_id_oa_field_name_mapping(){
	return $custom_field_mapping;
}

/***
 *Conver text to redmine wiki title
 *Translated from Wiki.titleize
 *PeijunZhang
 */

function com_titleize($text){
	$title = str_replace(' ', '_', $text);
	//$unwanted_chars = preg_quote(',./?;|:');
	$title = ucfirst(preg_replace('/[,;\.\?\|\:\/]/', '', $title));
	return $title;
}

function _dump($data, $color='blue', $return = FALSE, $show_caller = TRUE, $use_devel = TRUE) {
  $out = "<div style='color:$color;'>";
	if ($use_devel && module_exists('devel') && user_access('access devel information')){
     $out = $out . kprint_r($data, TRUE);
     $backtrace = debug_backtrace(DEBUG_BACKTRACE_IGNORE_ARGS);
     $out = $out . "--File: " . $backtrace[0]['file'] . "\n";
	   $out = $out . "--Line: " . $backtrace[0]['line'] . "\n";
	}
	else{
		$out = $out . "<pre>";
	  //print var_name($data);
	  $out =  $out . print_r($data, TRUE);
		if ($show_caller){
			$backtrace = debug_backtrace(DEBUG_BACKTRACE_IGNORE_ARGS);
	    $out = $out . "\nCaller:\n";
	    //$out = $out . "--Function: " . $backtrace[0]['function'] . "\n";
	    $out = $out . "--File: " . $backtrace[0]['file'] . "\n";
	    $out = $out . "--Line: " . $backtrace[0]['line'] . "\n";
		}
	  $out = $out . "</pre>";
	}
  $out = $out . "</div>";
  if ($return) {
    return $out;
  }
  print $out;
}

ini_set('display_errors', TRUE);
// We prepare a minimal bootstrap for the update requirements check to avoid
// reaching the PHP memory limit.
require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
require_once DRUPAL_ROOT . '/includes/update.inc';
require_once DRUPAL_ROOT . '/includes/common.inc';
require_once DRUPAL_ROOT . '/includes/file.inc';
require_once DRUPAL_ROOT . '/includes/entity.inc';
require_once DRUPAL_ROOT . '/includes/unicode.inc';
include_once DRUPAL_ROOT . '/includes/batch.inc';
//update_prepare_d7_bootstrap();


// Determine if the current user has access to run update.php.
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
drupal_maintenance_theme();

// read .ini file
$ini_array = parse_ini_file(DRUPAL_ROOT . "/chili_oa_migration_tool.ini");
$temp_attachment_file_folder = $ini_array['temp_attachment_file_folder'];
$offcial_attacment_file_folder = $ini_array['official_attachment_file_folder'];
$chili_db = $ini_array['chili_db'];
$custom_fields_mapping = $ini_array['custom_field_mapping'];

//print _dump($temp_attachment_file_folder, 'purple', TRUE);
//print _dump($offcial_attacment_file_folder, 'purple', TRUE);
//print _dump($chili_db, 'purple', TRUE);
//print _dump($custom_fields_mapping, 'purple', TRUE);
//$output = _dump($ini_array, 'purple', TRUE);
// Only allow the requirements check to proceed if the current user has access
// to run updates (since it may expose sensitive information about the site's
// configuration).

drupal_set_title('Migrating Chiliproject to OA');

$output=chili_oa_migration_selection_page();

if (isset($output) && $output) {
  // Explicitly start a session so that the update.php token will be accepted.
  //drupal_session_start();
  // We defer the display of messages until all updates are done.
  //$progress_page = ($batch = batch_get()) && isset($batch['running']);
  print theme('update_page', array('content' => $output));
}

?>