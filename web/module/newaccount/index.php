<?php
	if($lang->LoadModule("newaccount")){
		$Filename_To_Show = DIR_MODULES."newaccount/index.html";
		
		if(isset($_POST['submit'])){
			include('funciones.php');
			$Reg = ObtenerPost("Reg");
			$validar_campos = ValidarCampos($Reg);
			if($validar_campos == 'OK'){
				$codeactive = md5(time().'/&/'.$Reg['name'].'/&/'.$Reg['email']);
				$sql = "INSERT INTO account (name, password, email, codeactive) VALUES 
	('".$Reg['name']."','".md5($Reg['password'])."','".$Reg['email']."','".$codeactive."')";
				if($conn->Execute($sql)){
					$para = $Reg['email'];
					$cuerpo = $lang->_module->MailToSend($Reg['name'],$Reg['password'],$codeactive,$asunto);
					$cabecera  = 'MIME-Version: 1.0' . "\r\n";
					$cabecera .= 'Content-type: text/html; charset=utf-8' . "\r\n";
					$cabecera .= 'From: No-Reply <no-reply@aoc-dev.com.ar>' . "\r\n";
					@mail($para,$asunto,$cuerpo,$cabecera);
					$template->assign_block_vars("NOERROR_FORM",array(
																													"Display"	=>	$lang->_module->FormMsg('succregister')
																												)
																		);
				}else{
					$template->assign_block_vars("ERROR_FORM",array(
																													"Display"	=>	$lang->_module->FormMsg('failregister')
																												)
																		);
				}
			}else{
				$template->assign_block_vars("ERROR_FORM",array(
																													"Display"	=>	$lang->_module->FormMsg($validar_campos)
																												)
																		);
				if (preg_match("/user/i", $validar_campos)) {
					$template->assign_var("emailv", " value=\"{$Reg['email']}\"");
				}elseif (preg_match("/email/i", $validar_campos)) {
					$template->assign_var("namev", " value=\"{$Reg['name']}\"");
				}else{
					$template->assign_vars(array(
																				"emailv"	=>	" value=\"{$Reg['email']}\"",
																				"namev"		=>	" value=\"{$Reg['name']}\""
																			)
																);
				}
			}
			
			
		}
		
		$template->assign_vars(array(
																		"usuario"		=>	$lang->_module->FormText('usuario'),
																		"password"	=>	$lang->_module->FormText('clave'),
																		"repasswd"	=>	$lang->_module->FormText('reclave'),
																		"email"			=>	$lang->_module->FormText('email'),
																		"submit"		=>	$lang->_module->FormText('submit')
																	)
													);
	}else{
		$lang->Error('LoadModule');
	}
?>
