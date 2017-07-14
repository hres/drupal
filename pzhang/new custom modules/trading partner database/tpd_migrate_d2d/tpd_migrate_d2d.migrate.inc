<?php
/**
 * @file tpd_migrate_d2d.migrate.inc
 */

/**
 * This is code for TPD migration from old D7 to new D7.
 */

/**
 * Implements hook_migrate_api().
 */
function tpd_migrate_d2d_migrate_api() {
  /**
   * Declare the api version and migration group.
   */
  $api = array(
    'api' => 2,
    'groups' => array(
      'tpd_group' => array(
        'title' => t('Old D7 to new D7 TPD migrations.'),
      ),
      'migrations' => array(),
    ),
  );

  /**
   * Each migration being registered takes an array of arguments, some required
   * and some optional. Start with the common arguments required by all - the
   * source_connection (connection key, set up in settings.php, pointing to
   * the Drupal 6 database), source_version (major version of Drupal), and
   * group_name (a.k.a. import job).
   */
  $common_arguments = array(
    'source_connection' => 'legacy',
    'source_version' => 7,
    'group_name' => 'tpd_group',
	'source_database' => array(
    'driver' => 'mysql',
    'database' => 'old_tpd',
    'username' => 'drupal',
    'password' => 'drupal_pwd',
    'host' => 'localhost',
    'prefix' => '',
	),
  );

  // Register the user migration.
   // Migrate roles
  
  $api['migrations']['Role'] = $common_arguments + array(
    'description' => t('Migration of Roles'),
    'class_name' => 'DrupalRole7Migration',
	'role_mappings' => array(
      'manager' => 'manager',
      'data entry' => 'data entry',
      'observer' => 'observer',
    ),
  );

  // We just use the migrate_d2d D7 migration class as-is.
  $api['migrations']['User'] = $common_arguments + array(
    'description' => t('Migration of users'),
    'class_name' => 'DrupalUser7Migration',
	'role_migration' => 'Role',
  );

  // For vocabulary migrations, source_vocabulary and destination_vocabulary are
  // required arguments. Note that in Drupal 6 vocabularies did not have machine
  // names, so we use the vocabulary ID to uniquely identify them.
  /*
  $vocabulary_arguments = array(
    'Countries' => array(
      'description' => t('Migration of Countries terms from Drupal 6'),
      'source_vocabulary' => '1',  // D6 Vocabulary ID
      'destination_vocabulary' => 'countries',
    ),
    'Topics' => array(
      'description' => t('Migration of Topics terms from Drupal 6'),
      'source_vocabulary' => '2',  // D6 Vocabulary ID
      'destination_vocabulary' => 'topics',
    ),
  );
	*/
  // Again, we're using the migrate_d2d class directly.
  // The soft dependency says that while we don't have to run the user migration
  // first, we want to make sure it's listed first so the vocabularies are
  // listed right ahead of the node migrations.
  /*
  $common_vocabulary_arguments = $common_arguments + array(
    'class_name' => 'DrupalTerm6Migration',
    'soft_dependencies' => array('User'),
  );
  foreach ($vocabulary_arguments as $migration_name => $arguments) {
    $arguments += $common_vocabulary_arguments;
    $api['migrations'][$migration_name] = $arguments;
  }
	*/
  // Node migrations - each has its own class derived from the migrate_d2d class,
  // specifying its particular field mappings and transformations. source_type
  // and destination_type are required arguments.
  $node_arguments = array(
    'CodeAmendmentStatus' => array(
      'class_name' => 'TPDCodeAmendmentStatusMigration',
      'description' => t('Migration of Code Amendment Status nodes'),
      'source_type' => 'code_amendment_status',
      'destination_type' => 'code_amendment_status',
    ),
	'CodeTPStatus' => array(
      'class_name' => 'TPDCodeTPStatusMigration',
      'description' => t('Migration of Code TP Status nodes'),
      'source_type' => 'code_tp_status',
      'destination_type' => 'code_tp_status',
    ),
	
	'TradingPartner' => array(
      'class_name' => 'TPDTradingPartnerMigration',
      'description' => t('Migration of Trading Partner nodes'),
      'source_type' => 'trading_partner',
      'destination_type' => 'trading_partner',
    ),
	
  );

  // Tell the node migrations where the users are coming from, so they can
  // set up the dependency and resolve D6->D7 uids.
  $common_node_arguments = $common_arguments + array(
      'user_migration' => 'User'
    );

  foreach ($node_arguments as $migration_name => $arguments) {
    $arguments = array_merge_recursive($arguments, $common_node_arguments);
    $api['migrations'][$migration_name] = $arguments;
  }
	/*
  // Menu migrations -- same requirements as users
  $menu_arguments = $common_arguments + array(
    'description' => t('Migration of menus from Drupal 6'),
    'class_name' => 'DrupalMenu6Migration',
  );

  // We just use the migrate_d2d D6 migration class as-is.
  $api['migrations']['Menu'] = $menu_arguments;

  // Menu links migrations -- basic requirements, plus must set up menu dependency
  $menu_links_arguments = $common_arguments + array(
    'description' => t('Migration of menu links from Drupal 6'),
    'class_name' => 'DrupalMenuLinks6Migration',
    'menu_migration' => 'Menu',
  );
  $api['migrations']['MenuLinks'] = $menu_links_arguments;
	*/
	// field collection
	$api['migrations']['PrimaryUser'] = $common_arguments + array(
    'description' => t('Migration of primary users'),
    'class_name' => 'TPDPrimaryUserMigration',
	);
	$api['migrations']['DelegatedUsers'] = $common_arguments + array(
    'description' => t('Migration of delegated users'),
    'class_name' => 'TPDDelegatedUsersMigration',
	);
	$api['migrations']['OtherUsers'] = $common_arguments + array(
    'description' => t('Migration of other users'),
    'class_name' => 'TPDOtherUsersMigration',
	);
 
  return $api;
}
