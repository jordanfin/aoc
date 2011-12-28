<?php
	function ValidarCamposNP($NewPJ){
		if(Validar($NewPJ['name'],'usuario')){
			if(strlen($NewPJ['name']) >= 4){
				if(!ExistsUser($Login['name'])){
					if(Validar($NewPJ['home'],'num')){
						if(Validar($NewPJ['race'],'num')){
							if(Validar($NewPJ['class'],'num')){
								if(Validar($NewPJ['gender'],'num') && (($NewPJ['gender'] == '1') || ($NewPJ['gender'] == '2'))){
									return "OK";
								}else{
									return 'genderinvalid';
								}
							}else{
								return 'classinvalid';
							}
						}else{
							return 'raceinvalid';
						}
					}else{
						return 'homeinvalid';
					}
				}else{
					return 'nameused';
				}
			}else{
				return 'namelength';
			}
		}else{
			return 'nameinvalid';
		}
	}

	function ExistsUser($u){
		global $conn;
		$sql = "SELECT name FROM acc_players WHERE name = '$u' LIMIT 1";
		if($row = $conn->Execute($sql)){
			if(!$row->EOF()){
				return true;
			}else{
				return false;
			}
		}
		return false;
	}
	
	function Available_Players(){
		global $acc_controller;
		return $acc_controller->get('players');
	}
	
?>
