$type = 'oa_worktracker_task';
$nids = db_query('SELECT nid FROM {node} where type = :type', array(':type' => $type))->fetchCol();
//dpm($nids);
$queue = DrupalQueue::get('node_processing');
$queue -> deleteQueue(); // delete all un-done items
$op = 'move_only';
$count = 0;
foreach($nids as $nid){
	$item = array('id' => $nid, 'op' => $op,);
	$queue->createItem($item);
	$count ++;
	if ($count > 10) break;
}
$operations = array();
$operations[] = array('node_process_batch', array($queue->numberOfItems()));
$batch = array(
	'operations' => $operations,
	'finished' => 'com_batch_op_finished',
	// We can define custom messages instead of the default ones.
	'title' => t('Process all tasks'),
	'init_message' => t('Processing starting...'),
	'progress_message' => t('Processed @current out of @total steps.'),
	'error_message' => t('Processing has encountered an error.'),
);
dpm($operations);
batch_set($batch);
batch_process();

function node_process_batch($total_items, &$context){
	$queue = DrupalQueue::get('node_processing');
	dpm($queue);
	if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = $total_items;
    $context['results']['log'] = array();
  }
	
	$item = $queue->claimItem();
	dpm($item);
	if ($item) {
		//drupal_set_message(_dump($item, 'purple', TRUE));
		$id = $item->data['nid'];
		$op = $item->data['op'];
		processing_node($nid, $op);
	}
	

	// Provide an estimation of the completion level we've reached.
	$context['sandbox']['progress'] += 1;
	$context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
	$context['message'] = t("$op OA watchers. " . 'Processed @current out of @total', array('@current' => $context['sandbox']['progress'], '@total' => $context['sandbox']['max']));
}

function processing_node($nid, $op){
	$node = node_load($nid);
	dpm($node);
	//return;
	$wrapper = entity_metadata_wrapper('node', $node);
	dpm ($wrapper->field_related_tasks2 -> value());

	$related_tasks = $node->field_related_tasks;
	dpm($related_tasks);
	return;
	foreach($related_tasks['und'] as $item){
		$field_collection_item = entity_create('field_collection_item', array('field_name' => 'field_related_tasks2'));
		$field_collection_item->field_relation_type = $item['field_relation_type'];
		$field_collection_item->field_related_task = $item['field_related_task'];
		$field_collection_item->setHostEntity('node', $node);
		$field_collection_item->save(TRUE);
	}
	if ($op != 'move_only'){
		$node -> $related_tasks = array();
	}
	
	node_save($node);
	dpm($nid);

}