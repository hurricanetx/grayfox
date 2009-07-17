<?php
class gf_parse{
	
}
class gf_preg{
	private function __construct(){}
	public static function grep($contents,$pattern){
		if(preg_match_all($pattern,$contents,$matches,PREG_SET_ORDER)){
			return $matches;
		}else {
			return null;
		}
	}
}
?>