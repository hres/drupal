<?php

$plugin = array(
  'single' => TRUE,
  'title' => t('HC Header'),
  'description' => t(''),
  'category' => t('Custom Panes'),
  'edit form' => 'nhpd_oa2_hc_header_edit_form',
  'render callback' => 'nhpd_oa2_hc_header_render',
  'admin info' => 'nhpd_oa2_hc_header_info',
  'defaults' => array('description' => t('HC standard header'),
      								'title' => t('HC header'),),
  'all contexts' => TRUE,
	'i18n_settings' => array(
    'title',
    'description',
  ),
);

function nhpd_oa2_hc_header_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];
  $form['description'] = array(
    '#title' => t('Description'),
    '#description' => t(''),
    '#type' => 'textarea',
    '#required' => FALSE,
    '#default_value' => $conf['description'],
  );
  $form['title'] = array(
    '#title' => t('title'),
    '#type' => 'value',
    '#default_value' => $conf['title'],
  );

  return $form;
}
function nhpd_oa2_hc_header_edit_form_submit($form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

function nhpd_oa2_hc_header_render($subtype, $conf, $args, $contexts) {
  $block = new stdClass();
  $theme_dir = $GLOBALS['base_path'].drupal_get_path('theme', 'hc_ims');
	$hc_logo = $theme_dir . "/assets/images/gc-logo.gif";
	
	// language switcher
	Global $language;
	$use_native = TRUE;
	$path = drupal_is_front_page() ? '<front>' : $_GET['q'];
	if  (function_exists('language_negotiation_get_switch_links')){
		$links = language_negotiation_get_switch_links('language', $path);

		if (isset($links->links)) {
			//drupal_add_css(drupal_get_path('module', 'locale') . '/locale.css');
			//$class = "language-switcher-{$links->provider}";
			$variables = array('links' => $links->links);
			//drupal_set_message(nhpid_api_dump($variables, 'purple', TRUE));
			//$link = theme('links', $variables)
			$theme = theme('links__locale_block', $variables);
			//$block['subject'] = t('Languages');
			$lang_en = $links->links['en'];
			$lang_fr = $links->links['fr'];
			$lang_to_switch = $lang_fr;
			
			if ($language->language == 'fr'){
				$lang_to_switch = $lang_en;
			}
			//drupal_set_message(nhpid_api_dump($language, 'purple', TRUE));
			//drupal_set_message(nhpid_api_dump($lang_to_switch, 'purple', TRUE));
			$label = '';
			if ( $use_native ) {
				$label = $lang_to_switch['language']->native;
			} else {
				$label = $lang_to_switch['language']->name;
			}
			//$languge_link = l($label, $lang_to_switch['href'], array('language'=>$lang_to_switch['language']->prefix, 'html' => 'true'));
			$variables = array('links' => array($lang_to_switch['language']->language => $lang_to_switch));
			$lan_block = theme('links__locale_block', $variables);
			$dom = new DOMDocument;
			@$dom->loadHTML($lan_block);
			$elements = $dom->getElementsByTagName('a');

			//$element = $elments[0];
			//$languge_link = l($label, $href, array('language'=>$lang_to_switch['language']->prefix, 'html' => 'true'));

			foreach ($elements as $element){
				$href = $element->getAttribute('href');
				$languge_link = "<a href='$href'>$label</a>" ;
				break;
			}
			//drupal_set_message(nhpid_api_dump($languge_link, 'purple', TRUE));
		}
	}
	else{
		$languge_link = ''; // no language switch bar
	}
		
	$content_en = <<<EOD
		<div id='gcwu-gcnb'>
			<div id='gcwu-gcnb-in'>
				<div id='gcwu-gcnb-fip'>
					<div id='gcwu-sig'>
						<div id='gcwu-sig-in'>
							<div id='gcwu-sig-eng' title='Government of Canada'>
								<img width='214' height='20' alt='Government of Canada' src='$hc_logo'>
							</div>
						</div>
					</div>
					<ul>
						<li id='gcwu-gcnb1'>
							<a href='http://www.canada.ca/en/index.html' rel='external'>Canada.ca</a>
						</li>
						<li id='gcwu-gcnb2'>
							<a href='http://www.servicecanada.gc.ca/eng/home.shtml' rel='external'>Services</a>
						</li>
						<li id='gcwu-gcnb3'>
							<a href='http://canada.ca/en/gov/dept/index.html' rel='external'>Departments</a>
						</li>
						<li id='gcwu-gcnb-lang'>
							$languge_link
						</li>
					</ul>
				</div>
			</div>
		</div>
EOD;
	
	$content_fr = <<<EOD
	<div id='gcwu-gcnb'>
		<div id='gcwu-gcnb-in'>
			<div id='gcwu-gcnb-fip'>
				<div id='gcwu-sig'>
					<div id='gcwu-sig-in'>
						<div id='gcwu-sig-eng' title='Gouvernement du Canada'>
							<img width='214' height='20' alt='Gouvernement du Canada' src='$hc_logo'>
						</div>
					</div>
				</div>
				<ul>
					<li id='gcwu-gcnb1'>
						<a href='http://canada.ca/fr/index.html' rel='external'>Canada.ca</a>
					</li>
					<li id='gcwu-gcnb2'>
						<a href='http://www.servicecanada.gc.ca/fra/accueil.shtml' rel='external'>Services</a>
					</li>
					<li id='gcwu-gcnb3'>
						<a href='http://canada.ca/fr/gouv/min/index.html' rel='external'>Ministères</a>
					</li>
					<li id='gcwu-gcnb-lang'>
						$languge_link
					</li>
				</ul>
			</div>
		</div>
	</div>
EOD;
	if ($language->language == 'fr'){
		$block->content = $content_fr;
	}
	else{
		$block->content = $content_en;
	}
	//$content_en = 'test';
	//$block->content = $content_en;
	
  return $block;
}

function nhpd_oa2_hc_header_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass;
    $block->title = $conf['override_title'] ? $conf['override_title_text'] : '';
    return $block;
  }
}

