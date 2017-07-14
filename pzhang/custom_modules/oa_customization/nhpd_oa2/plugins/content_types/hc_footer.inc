<?php

$plugin = array(
  'single' => TRUE,
  'title' => t('HC Footer'),
  'description' => t(''),
  'category' => t('Custom Panes'),
  'edit form' => 'nhpd_oa2_hc_footer_edit_form',
  'render callback' => 'nhpd_oa2_hc_footer_render',
  'admin info' => 'nhpd_oa2_hc_footer_info',
  'defaults' => array('description' => t('HC standard footer'),
      								'title' => 'HC footer',),
  'all contexts' => TRUE,
);

function nhpd_oa2_hc_footer_edit_form($form, &$form_state) {
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
function nhpd_oa2_hc_footer_edit_form_submit($form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

function nhpd_oa2_hc_footer_render($subtype, $conf, $args, $contexts) {
  Global $language;
	$block = new stdClass();
  $theme_dir = $GLOBALS['base_path'].drupal_get_path('theme', 'hc_ims');
	$hc_logo = $theme_dir . "/assets/images/gc-logo.gif";
	$content_en = <<<EOD
	<div id='wb-foot'>
  <div id='wb-foot-in'>
    <footer>
      <h2>Footer</h2>
      <!-- FooterStart -->
      <nav role='navigation'>
        <div id='gcwu-sft'>
          <h3>Site footer</h3>
          <div id='gcwu-sft-in'>
            <div id='gcwu-tctr'>
              <ul>
                <li class='gcwu-tc'>
                <a href='?pgid=003' rel='license'>Terms and conditions</a></li>
                <li class='gcwu-tr'>
                <a href='http://www.tbs-sct.gc.ca/pd-dp/gr-rg/index_e.asp'>Transparency</a></li>
              </ul>
            </div>
            <div class='clear'></div>
          </div>
        </div>
      </nav>

      <nav role='navigation'>
        <div id='gcwu-gcft'>
          <h3>Government of Canada footer</h3>
          <div id='gcwu-gcft-in'>
            <div id='gcwu-gcft-fip'>
              <ul>
                <li><a rel='external' href='http://healthycanadians.gc.ca/index-eng.php'><span>Health</span><br />healthycanadians.gc.ca</a></li>
                <li><a rel='external' href='http://www.voyage.gc.ca/index-eng.asp'><span>Travel</span><br />travel.gc.ca</a></li>
                <li><a rel='external' href='http://www.servicecanada.gc.ca/eng/home.shtml'><span>Service Canada</span><br />servicecanada.gc.ca</a></li>
                <li><a rel='external' href='http://www.jobbank.gc.ca/intro-eng.aspx'><span>Jobs</span><br />jobbank.gc.ca</a></li>
                <li><a rel='external' href='http://actionplan.gc.ca/eng/index.asp'><span>Economy</span><br />actionplan.gc.ca</a></li>
                <li id='gcwu-gcft-ca'><div><a rel='external' href='http://www.canada.ca/menu-eng.html'>Canada.ca</a></div></li>
              </ul>
            </div>
          </div>
        </div>
      </nav>
    </footer>
  </div>
	</div>
EOD;
  
	$content_fr = <<<EOD
	<div id="wb-foot"><div id="wb-foot-in"><footer><h2 id="wb-nav">Pied de page</h2>
	<!-- FooterStart -->
	<nav role="navigation"><div id="gcwu-sft"><h3>Pied de page du site</h3><div id="gcwu-sft-in">
	<div id="gcwu-tctr">
		<ul>
		<li class="gcwu-tc"><a href="/fra/commun/avis.shtml" rel="license">Avis</a></li>
		<li class="gcwu-tr"><a href="/fra/commun/divulgation.shtml">Transparence</a></li>
		</ul>
	</div>
	<div class="clear"></div>
	
	</div></div></nav>

	<nav role="navigation"><div id="gcwu-gcft"><h3>Pied de page du gouvernement du Canada</h3><div id="gcwu-gcft-in"><div id="gcwu-gcft-fip">
	<ul>
	<li><a rel="external" href="http://canadiensensante.gc.ca/index-fra.php"><span>Santé</span></a></li>
	<li><a rel="external" href="http://www.voyage.gc.ca/index-fra.asp"><span>Voyage</span></a></li>
	<li><a href="/fra/accueil.shtml"><span>Service Canada</span></a></li>
	<li><a rel="external" href="http://www.guichetemplois.gc.ca/Intro-fra.aspx"><span>Emplois</span></a></li>
	<li><a rel="external" href="http://plandaction.gc.ca/fr"><span>Économie</span></a></li>
	<li id="gcwu-gcft-ca"><div><a rel="external" href="http://canada.ca/fr/index.html">Canada.ca</a></div></li>
	</ul>
	</div></div></div></nav>
	<!-- FooterEnd -->
	</footer>
	</div></div></div>
	
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

function nhpd_oa2_hc_footer_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass;
    $block->title = $conf['override_title'] ? $conf['override_title_text'] : '';
    return $block;
  }
}

