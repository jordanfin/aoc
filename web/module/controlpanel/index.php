<?php
	if($lang->LoadModule("controlpanel")){
		include('funciones.php');
		$inc_other_design = true;
		$Desing_To_Show = DIR_MODULES."controlpanel/design.html";
		$Menu_To_Show = DIR_MODULES."controlpanel/menu.html";
		switch(ObtenerGet("sec")){
			case 'newplayer':
				$players = Available_Players();
				if($players < $config->e('maxplayers')){
					include('newplayer.php');
				}else{
					$Filename_To_Show = "simpleerror.html";
					$template->assign_var("Error",$lang->_module->ErrorMsg('maxplayers'));
				}
			break;
			default:
				$Filename_To_Show = DIR_MODULES."controlpanel/index.html";
			break;
		}
		
		$template->assign_vars(array(
																	"WELCOME_USER"	=>	str_replace('<usuario>','Max',$lang->_module->MenuMsg('welcomeuser')), // Menu
																	"NEW_PLAYER"		=>	$lang->_module->MenuMsg('newplayer'),
																	"LOGOUT_ACC"		=>	$lang->_module->MenuMsg('logoutacc')
																)
													);
	}else{
		$lang->Error('LoadModule');
	}
?>
