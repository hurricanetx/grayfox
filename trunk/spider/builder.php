<?php
class grayfox_builder{
	public static $reserved_words = array('fetch','download','reg','xpath','filesave','dbinsert');
	public static function parse($string){
		$rerurn = '';
		$function_call = 0;
		$line_data = explode(':=',$line,2);
		if (count($line_data) == 2) {
			$left = $line_data[0];
			$right = $line_data[1];
		}else {
			$left = '';
			$right = $line;
		}
		$right_parse = self::sub_call($right,null);
	}
	public static function sub_call($rest,$current){
		$rest = trim($rest);
		if ($rest[0] == '"') {
			$rest = substr($rest,1);
			if ($pos = strpos($rest,'"')) {
				$cc = array('type'=>'string','value'=>'"'.substr($rest,0,$pos).'"','tag'=>0);
				$rest = substr($rest,$pos+1);
			}else {
				return 0;
			}
		}elseif ($rest[0] == "'") {
			$rest = substr($rest,1);
			if ($pos = strpos($rest,"'")) {
				$cc = array('type'=>'string','value'=>"'".substr($rest,0,$pos)."'",'tag'=>0);
				$rest = substr($rest,$pos+1);
			}else {
				return 0;
			}
		}elseif (preg_match('/^(fetch|download|reg|xpath|filesave|dbinsert)(\s+|\s*\()/is',$rest,$match)) {
			$rest = substr($rest,strlen($match[1]));
			$cc = array('type'=>'function','value'=>$match[1],'args'=>array(),'open'=>0);
		}elseif (preg_match('/^([_a-zA-Z0-9]+)(\s+|\s*[\(\.])/is',$rest,$match)) {
			$rest = substr($rest,strlen($match[1]));
			$cc = array('type'=>'variable','value'=>$match[1],'tag'=>0);
		}else {
			return 0;
		}
		if (($cc['type']=='string'||$cc['type']=='variable')) {
			if (preg_match('/^\s*\./is',$rest)) {
				$cc['tag'] = 1;
				$rest = preg_replace('/^\s*\./is','',$rest);
			}
			if (!empty($current)) {
				if ($current['type']=='variable'&&!empty($current['tag'])) {
					$cc['type'] = 'variable';
					$cc['value'] = $last['value']."[{$cc['value']}]";
					$current = &$cc;
					continue;
				}elseif ($current['type']=='function'&&!empty($current['open'])) {
					$current['args'][] = $cc;
					if (preg_match('/^\s*\)/is',$rest)) {
						$current['open'] = 0;
						$rest = preg_replace('/^\s*\)/is','',$rest);
					}
					continue;
				}else {
					return '';
				}
			}else {
				if ($cc['type']=='variable') {
					$cc['value'] = "\${$cc['value']}";
				}
				$rights[$i] = $cc;
				$last = &$rights[$i];
				$i++;
			}
		}
		if ($cc['type']=='function') {
			if (preg_match('/^\s*\(?\s*/is',$rest)) {
				$cc['open'] = 1;
				$rest = preg_replace('/^\s*\(?\s*/is','',$rest);
			}
			if (!empty($last)&&($last['type']=='variable'||$last['type']=='string')&&!empty($last['tag'])) {
				$cc['args'][] = $last;
				$last = $cc;
			}else {
				$rights[$i] = $cc;
				$last = &$rights[$i];
				$i++;
			}
		}
		$rights[] = $cc;
	}
}
if ($argc != 2 || in_array($argv[1], array('--help', '-help', '-h', '-?'))) {
	echo<<<HELP
Usage:
  {$argv[0]} <template_name>

  With the --help, -help, -h,
  or -? options, you can get this help.
HELP;
	exit(0);
}
$template_name = $argv[1];
$template_file = './templates/'.$template_name.'.gtp';
if (!file_exists($template_file)) {
	echo<<<HELP
Template:{$template_name} not exists.
HELP;
	exit(0);
}
$out_file = './spider/'.$template_name.'.php';
$out = "<?php\n";
$out .= "require('../inc/file.inc');\n";
$out .= "require('../inc/content.inc');\n";
$out .= "require('../inc/database.inc');\n";
$out .= "\n";
$template_data = file($template_file);
foreach ($template_data as $line) {
	if ($line[0] == '#' || $line[0] == ';') {
		continue;
	}
	$out .= parse($line)."\n";
}
$out .= "?>";
file_put_contents($out_file,$out);
?>