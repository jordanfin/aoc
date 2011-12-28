<?php

class english{
	private $_locate = 'en';
	public $_module = null;
	
	public function LoadModule($module){
		$file = DIR_LANG.DIR_MODULES.$module.'.php';
		if(file_exists($file)){
			require_once($file);
			$this->_module = new $module($this->_locate);
			return true;
		}
		return false;
	}
	
	public function Error($e){
		switch(strtolower($e)){
			case "loadmodule": echo "Hubo un error al cargar el modulo"; exit; break;
			case "cantloadacc": echo "ERROR: Al cargar los datos del usuario"; exit; break;
		}
	}
}

?>
