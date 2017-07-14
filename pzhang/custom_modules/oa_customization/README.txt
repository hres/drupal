Modules

	nhpd_oa2
		Base module. Contains some basic functions and plugins for parent task, related tasks, HC header, HC Footer, etc.

		How to install the plugins
		All pluglins belongs to the category 'Custom Panes'. Suggested place for those plugins to install:

		hc_header (HC Header): Mini Panel Open Atrium Toolbar
		hc_footer (HC Footer): Mini Panel Open Atrium Footbar
		related_tasks (Related Tasks): oa_worktracker_task panelizer, right sidebar
		sibling_tasks (Sibling taks: oa_worktracker_task panelizer, right sidebar
		is_parent_task (Is parent task): oa_worktracker_task panelizer, right sidebar
		task_list_visible_fields_filter (Task list visible custom fields and filters): oa_space panelizer, Content
	  task_rpt_visible_fields_filter (Task reporting visible custom fields and filters): task_reporting panelizer, Center

	oa_watchers
		Handles oa watchers and watch list. A watcher is a non-member who can view selected tasks. Watchers will be notified of changes of watched tasks.
		Watchers can go to their account to see and manage their watch list.

		Plugins:
		oa_watchers_admin (Watchers): oa_worktracker_task panelizer, right sidebar

	nhpd_oa_member
		Handles space member admin. This module allows to assign custom og role to space member.

		Plugin: nhpd_oa_member-widget(NHPD oa member widget): replace Page Space member>>Variants » Panel » Content>>OA core member widget

  oa_worktracker_task_custom_fields_workflow_control
		Handles allowed vaules (dropdown lists) for fields:
      field_oa_worktracker_task_status (workflow)
      field_oa_worktracker_task_type
      field_oa_worktracker_priroity
      field_issue_category

		And dymamic custom field visibility.

		Configure: admin/oa_worktracker_allowed_value_setting

	nhod_oa2_unique_tag
		Generates unique tags for tasks and wikis. Automatically generates unique tags in format [abbrevation of space name: nid]. The tags are prefixed to node titles.
		Configure: admin/oa_unique_tag


Customization procedures
	Referring site: https://dev.hc.ircan-rican.org/its

	Content types:
		Modify the following content types and their panalizers based on referring site:

			oa_worktracker_task
			oa_space
			oa_section

		Create new content types:
			nhpd_wiki
			task_reporting

	Views:
		Create views:
      Customized work tracker
      wiki

	Taxonomy terms:
		Modify or add new term section type tasks section
		Add new term section type wike section

	Text filter:
		Add new text format NHPD Wiki

Variable settings:
		configure: admin/openatrium/worktracker

To set task reporting function
	Create node of task reporting:
	Title: Task reporting or anything
	Tasks: customized_work_tracker - Task report (views variant)


