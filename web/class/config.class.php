<?php
class config {
	private $table;
	public function __construct(){
		$this->table = DB_PREFIX."config";
	}
	/*
	 *		Function para obtener los valores de configuracion
	 *
	 *		@param string $name
	 *		@return value
	 *		@author Maximiliano Duthey
	 */
  public function e($name){
    global $conn;
    $sql = "SELECT value FROM ".$this->table." WHERE config LIKE '$name'";
    if($row = $conn->Execute($sql)){
      if(!$row->EOF()){
        return $row->value;
      }else{
      	return false;
      }
    }
  }
  
  /*
	 *		Function para insertar o actualizar un dato en la configuracion
	 *
	 *		@param string $name
	 *		@param string $value
	 *		@param string [$title]
	 *		@return bool
	 *		@author Maximiliano Duthey
	 */
  public function set($name,$value,$title=0){
    global $conn;
    if($this->e($name)){
    	$sql = "UPDATE ".$this->table." SET ";
	  	if($value != '0'){
				$sql .= "value = '$value'";
			}
			if($title != "0"){
				$sql .= ($value != "0") ? ', ' : ''; $sql .= "titleconfig = '$title'";
			}
			$sql .= " WHERE config LIKE '$name'";
			if($conn->Execute($sql)) 
				return true; 
			else 
				return false;
    }else{
    	$sql = "INSERT INTO ".$this->table." (config,value,titleconfig) VALUES ('$name','$value','$title')";
			if($conn->Execute($sql))
				return true;
			else
				return false;
    }
  }
  
  /*
	 *		Function que obtiene todos los valores de configuracion
	 *
	 *		@return array
	 *		@author Maximiliano Duthey
	 */
  function getAll(){
  	global $conn;
  	 $sql = "SELECT * FROM ".$this->table."";
    if($row = $conn->Execute($sql)){
      if(!$row->EOF()){
        while(!$row->EOF()){
        	$r[$row->config] = array(
        																			"valor"	=>	$row->value,
        																			"title"	=>	$row->titleconfig
        																		);
        	$row->MoveNext();
        }
        return $r;
      }else{
      	return false;
      }
    }
  }
}
?>
