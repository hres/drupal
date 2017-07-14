<?PHP
$field_names = array('field_wiki_page',); // fields which store text, such as wiki pages, containing img links needed to be changed.
$replaces = array('<img src="/oa_demo/' => '<img src="/its/',); // key-val pair: original_text => new_text
//return $field_names ;
foreach($field_names as $field_name){
	$table = 'field_data_' . $field_name;
	$table_field = $field_name . '_value';
	$query = "select $table_field, entity_type, entity_id, revision_id, language, delta from $table";
;
	$result = db_query($query);
	while($rc = $result -> fetchObject()){
		$text = $rc ->$table_field;
		$entity_type = $rc -> entity_type;
		$entity_id = $rc -> entity_id;
		$revision_id = $rc->revision_id;
		$language = $rc ->language;
		$delta = $rc -> delta;
 //drupal_set_message(nhpid_api_dump($text, 'red', TRUE));
		foreach($replaces as $old_text => $new_text){
			//drupal_set_message(nhpid_api_dump($old_text, 'red', TRUE));
if(strpos($text, $old_text)!==false){
        $text = str_replace($old_text, $new_text, $text);
				drupal_set_message(nhpid_api_dump($text, 'red', TRUE));
				db_update($table)
				-> fields(array($table_field => $text))
				->condition('entity_type', $entity_type, '=')
				->condition('entity_id', $entity_id, '=')
				->condition('revision_id', $revision_id, '=')
				->condition('language', $language, '=')
				->condition('delta', $delta, '=')
				-> execute();

			}
		}

	}
}

?>





