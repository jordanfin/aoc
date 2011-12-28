<?php	
	session_start();
	function __autoload($class_name){
		require_once('class/'.strtolower($class_name).'.class.php');
	}
	
	date_default_timezone_set("America/Argentina/Buenos_Aires");
	define('CHARSET', 'utf-8');
	define('DB_HOST', 'localhost');
	define('DB_USER', 'USERNAME');
	define('DB_PASS', 'PASSWORD');
	define('DB_BASE', 'BASE_DE_DATOS');
	define('DB_PREFIX', '');
	define('URL_BASE', "http://".$_SERVER["HTTP_HOST"]."/");
	
	define('DIR_MODULES', 'module/');
	define('DIR_LANG','language/');
	
	$conn = new MySql;
	$config = new config;	
	
	define("TEMPLATE_ROUTER","themes/".$config->e('theme')."/");
	
	require_once('template_engine.php');
	$template = new Template();
	
	require_once('functions.php');

	if(!isset($_SESSION['AOC_SITE_LANG'])){
		$lang = DetectarLenguaje();
		switch($lang){
			case 'en':
				$_SESSION['AOC_SITE_LANG'] = 'english';
			break;
			default:
				$_SESSION['AOC_SITE_LANG'] = 'spanish';
			break;
		}
	}
	require_once(DIR_LANG.$_SESSION['AOC_SITE_LANG'].'.php');
	
	$lang = new $_SESSION['AOC_SITE_LANG'];
	
	if($_SESSION["AOC_SITE_AUTH"] == true){
		$acc_controller = new acc_controller($_SESSION["AOC_SITE_ID"]);
		if($acc_controller->_status == false){
			ShowMessage($lang->Error('cantloadacc'));
		}
	}
?>
