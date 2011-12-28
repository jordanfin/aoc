<?php
	function ValidarCampos($Login){
		if(Validar($Login['name'],'usuario')){
			if(ExistsUser($Login['name'])){
				if(CheckPasswd($Login['name'],$Login['password'])){
					if(CheckActive($Login['name'])){
						return "OK";
					}else{
						return 'needactive';
					}
				}else{
					return "passwordfail";
				}
			}else{
				return "userdontexists";
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
	
	function CheckPasswd($u,$p){
		global $conn;
		$sql = "SELECT name FROM account WHERE name = '$u' AND password = '".md5($p)."' LIMIT 1";
		if($row = $conn->Execute($sql)){
			if(!$row->EOF()){
				return true;
			}else{
				return false;
			}
		}
		return false;
	}
	
	function CheckActive($u){
		global $conn;
		$sql = "SELECT active FROM account WHERE name = '$u' LIMIT 1";
		if($row = $conn->Execute($sql)){
			if(!$row->EOF()){
				if($row->active == true){
					return true;
				}
			}
		}
		return false;
	}
?>
