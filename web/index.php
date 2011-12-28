<?php
	require_once('includes/config.php');	
	switch(ObtenerGet('page')){
		case 'newaccount':
			require_once(DIR_MODULES."newaccount/index.php");
		break;
		case 'activate':
			require_once(DIR_MODULES."activate/index.php");
		break;
		case 'login':
			require_once(DIR_MODULES."login/index.php");
		break;
		case 'controlpanel':
			NeedLogin();
			require_once(DIR_MODULES."controlpanel/index.php");
		break;
		case 'logout':
			session_destroy();
		break;
		default:
			echo "Index";
			exit;
		break;
	}
	if($inc_other_design == true){
		$template->set_filenames(array("resultado2" => TEMPLATE_ROUTER.$Filename_To_Show));
		$template->set_filenames(array("menu_ndesign" => TEMPLATE_ROUTER.$Menu_To_Show));
		$template->set_filenames(array("resultado" => TEMPLATE_ROUTER.$Desing_To_Show));
	}else{
		$template->set_filenames(array("resultado" => TEMPLATE_ROUTER.$Filename_To_Show));	
	}
	$template->assign_vars(array(
																"TEMPLATE_ROUTER"	=>	TEMPLATE_ROUTER,
																"CHARSET"					=>	CHARSET,
																"TITLE_SITE"			=>	$config->e('title_site'),
															)
												);
	$template->set_filenames(array("INDEX_DESIGN"	=>	TEMPLATE_ROUTER."design.html"));
	if($inc_other_design == true){
		$template->assign_var_from_handle("MENU","menu_ndesign");
		$template->assign_var_from_handle("DIV_DEL_RESULTADONEW","resultado2");
	}
	$template->assign_var_from_handle("DIV_DEL_RESULTADO","resultado");
	$template->pparse("INDEX_DESIGN");
?>
