<?php

class activate{
	private $_locate;
	public function __construct($lang){
		switch($lang){
			case 'en':
				$this->_locate = new activate_en;
			break;
			default:
				$this->_locate = new activate_es;
			break;
		}
	}
	
	public function FormMsg($d){
		if($this->_locate->_FormMsg[strtolower($d)] != ''){
			return $this->_locate->_FormMsg[strtolower($d)];
		}else{
			return '{'.$d.'}';
		}
	}
}

class activate_es{	
	public function __construct(){
		$this->_FormMsg = array(
																"keyinvalid"			=>	"El codigo de activación no es válido",
																"failactive"			=>	"Hubo un error al activar la cuenta",
																"succactive"			=>	"Cuenta activada correctamente"
															);
	}
}

class activate_en{
	public function __construct(){
		$this->_FormMsg = array(
																"keyinvalid"			=>	"The activation code isn't valid",
																"failactive"			=>	"There was an error during the account activation	",
																"succactive"			=>	"The account has been activated correctly"
															);
	}
}

?>
