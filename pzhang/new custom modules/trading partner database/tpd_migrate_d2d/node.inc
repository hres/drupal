<?php

/**
 * Common mappings for the TPD node migrations.
 */
abstract class TPDNodeMigration extends DrupalNode7Migration {
  public function __construct(array $arguments) {
    parent::__construct($arguments);

    $this->addFieldMapping('field_legacy_nid', 'nid')
         ->description('We have a common field to save the D6 nid');
	//$this->addFieldMapping('uid', 'uid');
  }
  public function complete($entity, stdClass $row){
	  //print_r($entity);
	  //print_r($row);
	  // author uid
	if ($row->uid){
		//print_r ('legacy_uid: ' . $entity->uid . "\n");
		$entity->uid=$row->uid;
		//print_r ('new_uid: ' . $entity->uid . "\n");
	}
	print_r($row->title . " is migrated \n");
  }
   public function get_new_user_uid($legacy_uid){
	 $legacy_user_row = Database::getConnection('legacy', $this->sourceConnection)
                 ->select('users', 'u')
                 ->fields('u', array('uid', 'name'))
				 ->condition('uid', $legacy_uid)
                 ->execute()
                 ->fetchObject();
	if ($legacy_user_row ) {
       $user_name = $user_row->name;
	   
	   $user_row = Database::getConnection('default')
                 ->select('users', 'u')
                 ->fields('u', array('uid', 'name'))
				 ->condition('name', $user_name)
                 ->execute()
                 ->fetchObject();
		if($user_row){
			$new_uid = $user_row -> uid;
			return $$new_uid;
		}
    }
	return 1;
  }
}

/**
 * Code Amendment Status mappings and handling.
 */
class TPDCodeAmendmentStatusMigration extends TPDNodeMigration {
  public function __construct(array $arguments) {
    // Add any other data we're pulling into the source row, before the parent
    // constructor.
    $this->sourceFields['field_code'] = 'Code';
    $this->sourceFields['field_code_name'] = 'Code Name';
	$this->sourceFields['field_description'] = 'Code Description';
	$this->sourceFields['field_workflow'] = 'Workflow';

    parent::__construct($arguments);

    $this->addFieldMapping('field_code', 'field_code');
    $this->addFieldMapping('field_code_name', 'field_code_name');
    $this->addFieldMapping('field_description', 'field_description');
	$this->addFieldMapping('field_workflow', 'workflow_node');
	
  }

  /**
   * Implementation of Migration::prepareRow().
   *
   * Here we pull in additional data from a non-core, non-CCK table.
   *
   * @param $row
   */
   
  public function prepareRow($row) {
    // If the  workflow states setting is different from old D7 to new D7, a mapping must be done.
	//print_r($row);
	if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    $workflow_row = Database::getConnection('legacy', $this->sourceConnection)
                 ->select('workflow_node', 'wd')
                 ->fields('wd', array('nid', 'sid', 'uid', 'stamp'))
				 ->condition('nid', $row->nid)
                 ->execute()
                 ->fetchObject();
	//print_r($workflow_row);

    if ($workflow_row ) {
       $row->workflow_node = array($workflow_row->sid);
    }
    else {
      $this->saveMessage(t('No workflow_node data for node !nid', array('!nid' => $nid)));
    }
	$row->title = $row->field_code_name[0];
	//print_r($row);
	return True;
  }
  
}

/**
 * Code TP Status mappings and handling.
 */
class TPDCodeTPStatusMigration extends TPDNodeMigration {
  public function __construct(array $arguments) {
    // Add any other data we're pulling into the source row, before the parent
    // constructor.
    $this->sourceFields['field_code'] = 'Code';
    $this->sourceFields['field_code_name'] = 'Code Name';
	$this->sourceFields['field_description'] = 'Code Description';

    parent::__construct($arguments);

    $this->addFieldMapping('field_code', 'field_code');
    $this->addFieldMapping('field_code_name', 'field_code_name');
    $this->addFieldMapping('field_description', 'field_description');
	$this->addFieldMapping('field_workflow', 'workflow_node');
	//$this->addFieldMapping('title', 'title');
  }
   public function prepareRow($row) {
    // If the  workflow states setting is different from old D7 to new D7, a mapping must be done.
	//print_r($row);
	if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    $workflow_row = Database::getConnection('legacy', $this->sourceConnection)
                 ->select('workflow_node', 'wd')
                 ->fields('wd', array('nid', 'sid', 'uid', 'stamp'))
				 ->condition('nid', $row->nid)
                 ->execute()
                 ->fetchObject();
	
    if ($workflow_row ) {
       $row->workflow_node = array($workflow_row->sid);
    }
    else {
      $this->saveMessage(t('No workflow_node data for node !nid', array('!nid' => $nid)));
    }
	$row->title = $row->field_code_name[0];

	return True;
  }
}

/**
 * Code Trading Partner mappings and handling.
 */
class TPDTradingPartnerMigration extends TPDNodeMigration {
  public function __construct(array $arguments) {
    // Add any other data we're pulling into the source row, before the parent
    // constructor.
    //$this->sourceFields['field_code'] = 'Code';
    //$this->sourceFields['field_code_name'] = 'Code Name';
	//$this->sourceFields['field_description'] = 'Code Description';
	//$this->sourceFields['field_amendment_status:field_amendment_status_entity_id'] = 'field reference target id';
    parent::__construct($arguments);

    $this->addFieldMapping('field_id', 'field_id');
    $this->addFieldMapping('field_company_name', 'field_company_name');
    $this->addFieldMapping('field_processor', 'field_processor');
	$this->addFieldMapping('field_date_received', 'field_date_received');
	$this->addFieldMapping('field_interview_date', 'field_interview_date');
	$this->addFieldMapping('field_date_completed', 'field_date_completed');
	$this->addFieldMapping('field_notes', 'field_notes');
	$this->addFieldMapping('field_epost_application', 'field_epost_application');
	$this->addFieldMapping('field_sent_date', 'field_sent_date');
	$this->addFieldMapping('field_signed_date', 'field_signed_date');
	//$this->addFieldMapping('field_primary_user', 'field_primary_user');
	//$this->addFieldMapping('field_delegated_users', 'field_delegated_user');
	//$this->addFieldMapping('field_other_users', 'field_other_user');
	$this->addFieldMapping('field_documents', 'field_documents');
	$this->addFieldMapping('field_current_tp_status', 'field_current_tp_status');
	$this->addFieldMapping('field_amendment_status', 'field_amendment_status');
	$this->addFieldMapping('field_workflow', 'workflow_node');
  }
   public function prepareRow($row) {
    // If the  workflow states setting is different from old D7 to new D7, a mapping must be done.
	
	if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }
	//print_r($row);
	// workflow node to workflow field
    $workflow_row = Database::getConnection('legacy', $this->sourceConnection)
                 ->select('workflow_node', 'wd')
                 ->fields('wd', array('nid', 'sid', 'uid', 'stamp'))
				 ->condition('nid', $row->nid)
                 ->execute()
                 ->fetchObject();
	
    if ($workflow_row ) {
       $row->workflow_node = array($workflow_row->sid);
    }
    else {
      $this->saveMessage(t('No workflow_node data for node !nid', array('!nid' => $nid)));
    }
	
	// entity reference target_ids
	$code_type ='code_amendment_status';
	$target_row = Database::getConnection('legacy', $this->sourceConnection)
                 ->select('field_data_field_amendment_status', 'f')
                 ->fields('f', array('entity_id', 'field_amendment_status_entity_id'))
				 ->condition('bundle', 'trading_partner')
				 ->condition('entity_id', $row->nid)
                 ->execute()
                 ->fetchObject();

	if ($target_row) {
		$old_target_id = $target_row->field_amendment_status_entity_id;
		//print_r($row->{'field_current_tp_status:entity_id'});
		$new_target_id = $this->old_code_nid_new_code_nid_mapping($old_target_id, $code_type);
		if ($new_target_id) {
		   $row->field_amendment_status = array($new_target_id);
		}
		else {
		  $this->saveMessage(t('No workflow_node data'));
		}
	}
	$code_type ='code_tp_status';
	$target_row = Database::getConnection('legacy', $this->sourceConnection)
                 ->select('field_data_field_current_tp_status', 'f')
                 ->fields('f', array('entity_id', 'field_current_tp_status_entity_id'))
				 ->condition('bundle', 'trading_partner')
				 ->condition('entity_id', $row->nid)
                 ->execute()
                 ->fetchObject();
	//print_r($target_row);
	if ($target_row) {
		$old_target_id = $target_row->field_current_tp_status_entity_id;
		//print_r($row->{'field_current_tp_status:entity_id'});
		$new_target_id = $this->old_code_nid_new_code_nid_mapping($old_target_id, $code_type);
		//print_r( '$new_target_id ');
		//print_r("\n");
		//print_r($new_target_id);
		if ($new_target_id) {
		   $row->field_current_tp_status = array($new_target_id);
		   $row->{'field_current_tp_status:entity_type'} = array('node');
		   //print_r($row->field_amendment_status);
		}
		else {
		  $this->saveMessage(t('No workflow_node data'));
		}
	}
	//print_r($row->field_amendment_status);
	//print_r($row->{'field_current_tp_status:entity_id'});
			
	return True;
  }
  public function old_code_nid_new_code_nid_mapping($old_nid, $code_type){
	//print_r($this);
	//print_r("\n");
	$code_row = Database::getConnection('legacy', $this->sourceConnection)
                 ->select('field_data_field_code', 'c')
                 ->fields('c', array('entity_id', 'field_code_value'))
				 ->condition('bundle', $code_type)
				 ->condition('entity_id', $old_nid)
                 ->execute()
                 ->fetchObject();
	//print_r($code_row);
	//print_r("\n");
	if ($code_row ) {
       $code = $code_row->field_code_value;
	   
	   $entity_row = Database::getConnection('default')
                 ->select('field_data_field_code', 'c')
                 ->fields('c', array('entity_id', 'field_code_value'))
				 ->condition('bundle', $code_type)
				 ->condition('field_code_value', $code)
                 ->execute()
                 ->fetchObject();
		if($entity_row){
			$entity_id = $entity_row -> entity_id;
			return $entity_id;
		}
    }
	return Null;
  }
 
}

