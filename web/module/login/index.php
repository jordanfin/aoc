<?php
	if($lang->LoadModule("login")){
		$Filename_To_Show = DIR_MODULES."login/index.html";
		if(isset($_POST['submit'])){
			include('funciones.php');
			$Login = ObtenerPost('Login');
			$validar_campos = ValidarCampos($Login);
			if($validar_campos == 'OK'){
				$_SESSION["AOC_SITE_AUTH"] = true;
				$row = $conn->Execute("SELECT id, name FROM account WHERE name LIKE '{$Login['name']}'");
				$_SESSION["AOC_SITE_USER"] = $row->name;
				$_SESSION["AOC_SITE_ID"] = $row->id;
				@Header("Location: /");
			}else{
				$template->assign_block_vars("ERROR_FORM",array(
																													"Display"	=>	$lang->_module->FormMsg($validar_campos)
																												)
																		);
				if (preg_match("/password/i", $validar_campos)) {
					$template->assign_var("namev", " value=\"{$Login['name']}\"");
				}
			}
		}
		
		$template->assign_vars(array(
																		"usuario"			=>	$lang->_module->FormText('usuario'),
																		"password"		=>	$lang->_module->FormText('clave'),
																		"submit"			=>	$lang->_module->FormText('submit'),
																		"registrate"	=>	$lang->_module->FormMsg('registrate')
																	)
													);
	}else{
		$lang->Error('LoadModule');
	}
?>
