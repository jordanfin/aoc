<?php

class login{
	private $_locate;
	public function __construct($lang){
		switch($lang){
			case 'en':
				$this->_locate = new login_en;
			break;
			default:
				$this->_locate = new login_es;
			break;
		}
	}
	
	public function FormText($d){
		if($this->_locate->_FormText[strtolower($d)] != ''){
			return $this->_locate->_FormText[strtolower($d)];
		}else{
			return '{'.$d.'}';
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

class login_es{	
	public function __construct(){
		$this->_FormText = array(
																"usuario"	=>	"Usuario",
																"clave"		=>	"Contraseña",
																"submit"	=>	"Entrar"
															);
		$this->_FormMsg = array(
																"registrate"			=>	"Registrate Aqui!",
																"userinvalid"			=>	"El usuario no es válido",
																"userdontexists"	=>	"El usuario no existe",
																"passwordfail"		=>	"La contraseña es erronea",
																"needactive"			=>	"Debes activar tu cuenta para poder acceder"
															);
	}
}

class login_en{
	public function __construct(){
		$this->_FormText = array(
																"usuario"	=>	"Username",
																"clave"		=>	"Password",
																"submit"	=>	"Login"
															);
		$this->_FormMsg = array(
																"registrate"			=>	"Sing up here!",
																"userinvalid"			=>	"The user is invalid",
																"userdontexists"	=>	"The user doesn't exist",
																"passwordfail"		=>	"The password is incorrect",
																"needactive"			=>	"Debes activar tu cuenta para poder acceder"
															);
	}
}

?>
