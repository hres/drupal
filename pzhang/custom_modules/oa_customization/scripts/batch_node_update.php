module_load_include('inc', 'node', 'node.admin');
$type = 'oa_worktracker_task';
$nids = db_query('SELECT nid FROM {node} where type = :type', array(':type' => $type))->fetchCol();
dpm($nids);
node_mass_update($nids, array());