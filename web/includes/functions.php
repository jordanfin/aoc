<?php
	/*
	 *    Function to check recursively if dirname is exists in directory's tree
	 *
	 *    @param string $dir_name
	 *    @param string [$path]
	 *    @return bool
	 *    @author FanFataL
	 */
	function dir_exists($dir_name = false, $path = './') {
		if(!$dir_name) return false;

		if(is_dir($path.$dir_name)) return true;

		$tree = glob($path.'*', GLOB_ONLYDIR);
		if($tree && count($tree)>0) {
			foreach($tree as $dir)
				if(dir_exists($dir_name, $dir.'/'))
					return true;
		}

		return false;
	}

	/*
	 *    Funcion que muestra un mensaje, normalmente utilizado en errores
	 *
	 *    @param string $msg
	 *    @author LoKoMaX
	 */
	function ShowMessage($msg){
  	echo $msg;
  	exit;
  }
  
  /*
   *		Funcion para cuando se requiere acceso para ingresar
   *
   *		@author LoKoMaX
  */
  
  function NeedLogin(){
  	if(!isset($_SESSION["AOC_SITE_AUTH"])){
  		@Header('Location: /index.php?page=Login');
  		exit;
  	}
  }
  
  /*
	 *    Funcion para obtener un valor limpio que fue enviado por metodo GET
	 *
	 *    @param string $i - Nombre metodo
	 *    @param boolean [$lower] - Resultado en minusculas o no. Defecto: SI
	 *    @return string
	 *    @author LoKoMaX
	 */
  function ObtenerGet($i,$lower=true){
  	if(isset($_GET[$i])){
  		$valor = $_GET[$i];
  		$pa_dar = '';
  		if(is_array($valor)){
  			foreach($valor as $key => $val){
  				$pa_dar[$key] = strip_tags(htmlspecialchars($val, ENT_QUOTES,"UTF-8"));
  			}
		  }else{
		  	$pa_dar = strip_tags(htmlspecialchars($valor, ENT_QUOTES,"UTF-8"));
		  }
			if($lower == false){
				return $pa_dar;
			}else{
				if(is_array($pa_dar)){
					foreach($valor as $key => $val){
						$pa_dar[$key] = strtolower($val);
	  			}
	  			return $pa_dar;
				}else{
				  return strtolower($pa_dar);
			  }
			}
		}else{
			return false;
		}
  }
  
  /*
	 *    Funcion para obtener un valor limpio que fue enviado por metodo POST
	 *
	 *    @param string $i - Nombre metodo
	 *    @param boolean [$lower] - Resultado en minusculas o no. Defecto: SI
	 *    @return string
	 *    @author LoKoMaX
	 */
  function ObtenerPost($i,$lower=true){
  	if(isset($_POST[$i])){
  		$valor = $_POST[$i];
  		$pa_dar = '';
  		if(is_array($valor)){
  			foreach($valor as $key => $val){
  				$pa_dar[$key] = strip_tags(htmlspecialchars($val, ENT_QUOTES,"UTF-8"));
  			}
		  }else{
		  	$pa_dar = strip_tags(htmlspecialchars($valor, ENT_QUOTES,"UTF-8"));
		  }
			if($lower == false){
				return $pa_dar;
			}else{
				if(is_array($pa_dar)){
					foreach($valor as $key => $val){
						$pa_dar[$key] = strtolower($val);
	  			}
	  			return $pa_dar;
				}else{
				  return strtolower($pa_dar);
			  }
			}
		}else{
			return false;
		}
  }
  
  /*
	 *    Funcion que permite limpiar un texto
	 *
	 *    @param string $valor - Cadena a limpiar
	 *    @return string
	 *    @author LoKoMaX
	 */
  function LimpiarCampo($valor){
    $valor = strip_tags(htmlspecialchars($valor,ENT_QUOTES,"UTF-8"));
    return $valor;
  }
  
  /*
	 *    Funcion para obtener la fecha completa. 
	 *
	 *    @param string $i - Fecha (formato date)
	 *    @param boolean [$lower] - De volver la hora. Defecto NO
	 *    @return string
	 *    @author LoKoMaX
	 */
  function FechaCompleta($f, $withhour=false){
    $a = getdate(strtotime($f));
    $n = array(
                "Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday",
                "January","February","March","April","May","June","July","August",
                "September","October","November","December"
              );
    $m = array(
                "Lunes","Martes","Miercoles","Jueves","Viernes","Sabado","Domingo",
                "Enero","Febrero","Marzo","Abril","Mayo","Junio","Julio","Agosto",
                "Septiembre","Octubre","Noviembre","Diciembre"
              );
    $b = $a['weekday'].", ".$a['mday']." de ".$a['month']." de ".$a['year'];
    $c = str_replace($n,$m,$b);
    if($withhour == true){
    	$c .= ", a las ".date("H:i:s",strtotime($f));
    }
    return $c;
  }
 
  /*
	 *    Funcion para obtener el lenguaje del navegador
	 *
	 *    @return string
	 *    @author LoKoMaX
	 */
	function DetectarLenguaje() {
		$lang = explode(";", $_SERVER['HTTP_ACCEPT_LANGUAGE']);
		$lang = explode(",", $lang['0']);
		if(strlen($lang[0]) > 2){
			$lang = explode("-", $lang['0']);
		}
		return strtolower($lang['0']);
	}

  /*
	 *    Funcion para validar cualquier campo
	 *
	 *    @return string
	 *    @author LoKoMaX
	 */
	function Validar($valor,$tipo,$op=0){
  	if($op == 1 && empty($valor)){
  		return true;
  	}
    switch($tipo){
      case 'usuario':
        $comparar = '/^[a-z0-9_-]*$/i';
      break;
      case 'email':
        $comparar = '/^[^0-9][a-z0-9\_\.\-]*[@][a-z0-9_]*[.][a-z.]{2,6}$/i';
      break;
      case 'num':
        $comparar = '/^[0-9]*$/';
      break;
      case 'naci':
      	$comparar = '/^[0-9]{4}-[0-9]{2}-[0-9]{2}$/';
      break;
    }
    if(preg_match($comparar,$valor) && !(empty($valor)) ){
      return true;
    }else{
      return false;
    }
  }
?>
