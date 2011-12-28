<?php

class newaccount{
	private $_locate;
	public function __construct($lang){
		switch($lang){
			case 'en':
				$this->_locate = new newaccount_en;
			break;
			default:
				$this->_locate = new newaccount_es;
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
	
	public function MailToSend($n,$p,$c,&$a){
		$enlace = "http://".$_SERVER['HTTP_HOST']."/index.php?page=Activate&key=$c";
		$reemplazar = array("<usuario>","<clave>","<enlace>","<emailsoporte>");
		$reemplazarpor = array($n,$p,$enlace,"soporte@aoc-dev.com.ar");
		$cuerpo = str_replace($reemplazar, $reemplazarpor, $this->_locate->_MailToSend);
		$a = $this->_locate->_SubjectToSend;
		return $cuerpo;
	}
}

class newaccount_es{	
	public function __construct(){
		$this->_FormText = array(
																"usuario"	=>	"Usuario",
																"clave"		=>	"Contraseña",
																"reclave"	=>	"Confirmar Contraseña",
																"email"		=>	"Correo Electronico",
																"submit"	=>	"Crear cuenta"
															);
		$this->_FormMsg = array(
																"userinvalid"			=>	"El usuario no es válido",
																"userlength"			=>	"El usuario debe tener minimo 4 caracteres",
																"passempty"				=>	"Debes ingresar una contraseña",
																"passdifferent"		=>	"Las contraseñas no coinciden",
																"emailinvalid"		=>	"El correo electronico no es válido",
																"succregister"		=>	"Se ha enviado un mail para activar su cuenta",
																"failregister"		=>	"Hubo un error en el registro",
																"accountexists"		=>	"El usuario ya esta registrado",
																"emailexists"			=>	"El correo electronico ya esta registrado"
															);
		$this->_MailToSend = "<html>
														<head>
															<title>World Of Argentum</title>
														</head>
														<body>
															<p>Estimado <usuario>,</p>
															<p>Gracias por registrarse en World Of Argentum. Antes de que podamos activarle su cuenta ha de realizar un ultimo paso para poder completar el registro.</p>
															<p>Recuerde, ha de completar este ultimo paso para pasar a se un miembro registrado. Usted solo necesitará visitar esta dirección para activar su cuenta.</p>
															<p>
																Para completar su registro, por favor, visite esta dirección:<br>
																<a href=\"<enlace>\"><enlace></a>
															</p>
															</p>
																Los datos de su cuenta son los siguientes:<br>
																Usuario: <usuario><br>
																Contraseña: <clave><br>
															</p>
															<p>
																Si continua teniendo problemas a la hora de usar su cuenta, por favor contacte con nuestro personal de soporte via email:<br>
																<emailsoporte>
															</p>
															<p>
																Un cordial saludo,<br>
																World Of Argentum.
															</p>
														</body>
													</html>";
		$this->_SubjectToSend = "Activación de la cuenta";
	}
}

class newaccount_en{
	public function __construct(){
		$this->_FormText = array(
																"usuario"	=>	"Username",
																"clave"		=>	"Password",
																"reclave"	=>	"Confirm Password",
																"email"		=>	"e-Mail",
																"submit"	=>	"Create account"
															);
		$this->_FormMsg = array(
																"userinvalid"			=>	"The user is invalid",
																"userlength"			=>	"The user must have four characters at least",
																"passempty"				=>	"You have to write a password",
																"passdifferent"		=>	"The passwords aren't equal",
																"emailinvalid"		=>	"The email is invalid",
																"succregister"		=>	"An email has been sent to activate your account",
																"failregister"		=>	"There was an error on the registry",
																"accountexists"		=>	"The user is already registered",
																"emailexists"			=>	"The email is already registered"
															);
		$this->_MailToSend = "<html>
														<head>
															<title>World Of Argentum</title>
														</head>
														<body>
															<p>Dear <usuario>,</p>
															<p>Thanks for registering at World Of Argentum. Before we can activate your account one last step must be taken to complete your registration.</p>
															<p>Remember, you must complete this last step to become a registered member. You will only need to visit this URL once to activate your account.</p>
															<p>
																To complete your registration, please visit this URL:<br>
																<enlace>
															</p>
															</p>
																The account details are as follows:<br>
																User: <usuario><br>
																Password: <clave><br>
															</p>
															<p>
																If you keep on having problems when you try to log in your account, please contact with our support staff via email:<br>
																<emailsoporte>
															</p>
															<p>
																All the best,<br>
																World Of Argentum.
															</p>
														</body>
													</html>";
		$this->_SubjectToSend = "­Account activation";
	}
}

?>
