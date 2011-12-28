<?php
	if($lang->LoadModule("activate")){
		$Filename_To_Show = DIR_MODULES."activate/index.html";
		$clave = ObtenerGet('key',false);
		$sql = "SELECT id FROM account WHERE codeactive = '$clave' LIMIT 1";
		if($row = $conn->Execute($sql)){
			if(!$row->EOF()){
				$sql = "UPDATE account SET active=true, codeactive='' WHERE id = '".$row->id."'";
				if($conn->Execute($sql)){
					$template->assign_var("RESULTADO_ACTIVE", $lang->_module->FormMsg('succactive'));
				}else{
					$template->assign_var("RESULTADO_ACTIVE", $lang->_module->FormMsg('failactive'));
				}
			}else{
				$template->assign_var("RESULTADO_ACTIVE", $lang->_module->FormMsg('keyinvalid'));
			}
		}
	}else{
		$lang->Error('LoadModule');
	}
?>
