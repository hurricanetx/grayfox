<?php
class grayfox_db{
	private static $db_host = '';
	private static $db_name = '';
	private static $db_user = '';
	private static $db_pass = '';
	public function __construct(){}
	public function get_link(){
		
	}
	public function insert($table,$keys,$values){
		
	}
	public function check_key_exists($table,$keyname,$keyvalue){
		$db = $this->get_link();
		$query = 'select * from '.mysql_escape_string($table).' where '.mysql_escape_string($keyname).' =\''.mysql_escape_string($keyvalue).'\'';
		$result = mysql_query($query,$db);
		if (mysql_fetch_assoc($result)) {
			return true;
		}else {
			return false;
		}
	}
}
?>