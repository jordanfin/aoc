<?php

class MySql	{
	private $conn;

	function __construct(){
		$this->conn = @mysql_connect(DB_HOST,DB_USER,DB_PASS);
		@mysql_select_db(DB_BASE, $this->conn);
	}
	
	/*
	 *		Funcion que ejecuta una sentencia SQL
	 *
	 *		@param string $sql - Sentencia SQL, Ejemplo: SELECT * FROM tabla
	 *		@return class
	 *		@author Maximiliano Duthey
	 */
	function Execute($sql){
		$qr = @mysql_query($sql, $this->conn);
		if(!$qr){
			return false;
		}else{
			return new MySql_Result($qr ,$this->conn);
		}
	}
	
	/*
	 *		Funcion para obtener id luego de un insert
	 *
	 *		@return integer 
	 *		@author Maximiliano Duthey
	 */
	function Insert_Id(){
		return @mysql_insert_id();
	}
}

class MySql_Result{
	var $connectionId;
	var $resultId;
	var $_numOfRows = -1;
	var $_currentRow = -1;
	var $EOF = false;
	
	function __construct($queryid, $connection){
		$this->connectionId = $connection;
		$this->resultId = $queryid;
		self::setData();
	}
	
	/*
	 *		Funcion para obtener total de registros
	 *
	 *		@return integer
	 *		@author Maximiliano Duthey
	 */
	function RecordCount(){
		return $this->_numOfRows;
	}
	
	/*
	 *		Funcion que verifica si existen o no datos
	 *
	 *		@return bool
	 *		@author Maximiliano Duthey
	 */
	function EOF(){
		if($this->_currentRow < $this->_numOfRows){
			$this->EOF = false;
			return false;
		}else{
			$this->EOF = true;
			return true;
		}
	}
	
	/*
	 *		Funcion para mover a la siguiente fila
	 *
	 *		@author Maximiliano Duthey
	 */
	function MoveNext(){
		$this->_currentRow++;
		$this->Move($this->_currentRow);
	}
	
	/*
	 *		Function para mover a una fila X
	 *
	 *		@param integer $n - Numero de fila
	 *		@author Maximiliano Duthey
	 */
	function Move($n=0){
		$this->_currentRow = $n;
		if(!$this->EOF()){
			mysql_data_seek($this->resultId, $this->_currentRow);
			$this->genData();
		}
	}
	
	/*
	 *		Funcion para setear los valores por defecto
	 *
	 *		@author Maximiliano Duthey
	 */
	private function setData(){
		$this->_numOfRows = @mysql_num_rows($this->resultId);
		$this->MoveNext();
	}
	
	/*
	 *		Funcion para obtener los datos de la fila
	 *
	 *		@author Maximiliano Duthey
	 */
	private function genData(){
		$data = @mysql_fetch_array($this->resultId, MYSQL_ASSOC);
		foreach($data as $key => $val){
			$this->$key = $val;
		}
	}
}
?>
