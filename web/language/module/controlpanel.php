<?php

class controlpanel{
	private $_locate;
	public function __construct($lang){
		switch($lang){
			case 'en':
				$this->_locate = new controlpanel_en;
			break;
			default:
				$this->_locate = new controlpanel_es;
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
	
	public function MenuMsg($d){
		if($this->_locate->_MenuMsg[strtolower($d)] != ''){
			return $this->_locate->_MenuMsg[strtolower($d)];
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
	public function ErrorMsg($d){
		if($this->_locate->_ErrorMsg[strtolower($d)] != ''){
			return $this->_locate->_ErrorMsg[strtolower($d)];
		}else{
			return '{'.$d.'}';
		}
	}
}

class controlpanel_es{	
	public function __construct(){
		$this->_FormText = array(
															"namepj"				=>	"Nombre del personaje",
															"origen"				=>	"Pueblo de origen",
															"raza"					=>	"Raza",
															"clase"					=>	"Clase",
															"genero"				=>	"Genero",
															"atributos"			=>	"Atributos",
															"fuerza"				=>	"Fuerza",
															"agilidad"			=>	"Agilidad",
															"inteligencia"	=>	"Inteligencia",
															"carisma"				=>	"Carisma",
															"constitucion"	=>	"Constitución",
															"gendermasc"		=>	"Hombre",
															"genderfem"			=>	"Mujer",
															"submit"				=>	"Crear"
														);
		$this->_FormMsg = array(
															"nameinvalid"				=>	"El nombre no es válido",
															"namelength"				=>	"El nombre es demasiado corto (minimo 4 caracteres)",
															"nameused"					=>	"El nombre ya esta en uso",
															"homeinvalid"				=>	"El pueblo no es válido",
															"raceinvalid"				=>	"La raza no es válida",
															"classinvalid"			=>	"La clase no es válida",
															"genderinvalid"			=>	"El genero no es válido",
															"createsuccefully"	=>	"Personaje creado exitosamente"
														);
		$this->_MenuMsg = array(
															"welcomeuser"	=>	"Bienvenido @<usuario>",
															"newplayer"		=>	"Crear personaje",
															"logoutacc"		=>	"Salir"
														);
		$this->_ErrorMsg = array(
															"maxplayers"	=>	"ERROR: No puedes crear mas personajes"
														);
	}
}

class controlpanel_en{
	public function __construct(){
		$this->_FormText = array(
															"namepj"				=>	"Nombre del personaje",
															"origen"				=>	"Pueblo de origen",
															"raza"					=>	"Raza",
															"clase"					=>	"Clase",
															"genero"				=>	"Genero",
															"atributos"			=>	"Atributos",
															"fuerza"				=>	"Fuerza",
															"agilidad"			=>	"Agilidad",
															"inteligencia"	=>	"Inteligencia",
															"carisma"				=>	"Carisma",
															"constitucion"	=>	"Constitución",
															"gendermasc"		=>	"Hombre",
															"genderfem"			=>	"Mujer",
															"submit"				=>	"Crear"
														);
		$this->_FormMsg = array(
															"nameinvalid"				=>	"El nombre no es válido",
															"namelength"				=>	"El nombre es demasiado corto (minimo 4 caracteres)",
															"nameused"					=>	"El nombre ya esta en uso",
															"homeinvalid"				=>	"El pueblo no es válido",
															"raceinvalid"				=>	"La raza no es válida",
															"classinvalid"			=>	"La clase no es válida",
															"genderinvalid"			=>	"El genero no es válido",
															"createsuccefully"	=>	"Personaje creado exitosamente"
														);
		$this->_MenuMsg = array(
															"welcomeuser"	=>	"Welcome @<usuario>",
															"newplayer"		=>	"Crear personaje",
															"logoutacc"		=>	"Logout"
														);
		$this->_ErrorMsg = array(
															"maxplayers"	=>	"ERROR: No puedes crear mas personajes"
														);
	}
}

?>
