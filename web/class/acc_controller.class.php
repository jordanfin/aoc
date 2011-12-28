<?php

class acc_controller{
	public $_status = false;
	private $_data = null;
	
	function __construct($id){
		global $conn;
		if($row=$conn->Execute("SELECT * FROM account WHERE id = '".(int)$id."'")){
			if(!$row->EOF()){
				$this->_data = $row;
				$this->_status = true;
			}
		}
	}
	
	function get($dato){
		return $this->_data->$dato;
	}
}

?>
