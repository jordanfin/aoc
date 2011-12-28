<?php
	function ValidarCampos($Reg){
		if(Validar($Reg['name'],'usuario')){
			if((strlen($Reg['name']) >= 4) && (strlen($Reg['name']) < 25) ){
				if(!ExistsUser($Reg['name'])){
					if(!empty($Reg['password']) || !empty($Reg['password2'])){
						if(md5($Reg['password']) == md5($Reg['password2'])){
							if(Validar($Reg['email'],'email')){
								if(!ExistsEmail($Reg['email'])){
									return "OK";
								}else{
									return "emailexists";
								}
							}else{
								return "emailinvalid";
							}
						}else{
							return "passdifferent";
						}
					}else{
						return "passempty";
					}
				}else{
					return "accountexists";
				}
			}else{
				return "userlength";
			}
		}else{
			return "userinvalid";
		}
	}
	
	function ExistsUser($u){
		global $conn;
		$sql = "SELECT name FROM account WHERE name = '$u' LIMIT 1";
		if($row = $conn->Execute($sql)){
			if(!$row->EOF()){
				return true;
			}else{
				return false;
			}
		}
		return false;
	}
	
	function ExistsEmail($u){
		global $conn;
		$sql = "SELECT email FROM account WHERE email = '$u' LIMIT 1";
		if($row = $conn->Execute($sql)){
			if(!$row->EOF()){
				return true;
			}else{
				return false;
			}
		}
		return false;
	}
?>
