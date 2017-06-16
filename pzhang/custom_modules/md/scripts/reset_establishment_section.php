$type = "hci_establishment"; 
$section_e_nid = 6340;
$nodes = node_load_multiple(array(), array('type' => $type));
$section_establishment = node_load($section_e_nid);
foreach ($nodes as $node){
	dpm($node->nid);
	$wrapper = entity_metadata_wrapper('node', $node);
	$wrapper->field_section-> set($section_establishment );
	$wrapper->save();
}