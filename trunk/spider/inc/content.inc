<?php
class grayfox_content{
	private function __construct(){}
	public static function reg($contents,$pattern){
		if(preg_match_all($pattern,$contents,$matches,PREG_SET_ORDER)){
			return $matches;
		}else {
			return null;
		}
	}
	public static function xpath($contents,$xpath){
		$xml = simplexml_load_string($contents);
		return $xml->xpath($xpath);
	}
}
?>