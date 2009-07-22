<?php
function parse($string){
	$rerurn = '';
	$reserved_words = array('fetch','download','reg','xpath','filesave','dbinsert');
	$function_call = 0;
	$line_data = explode(':=',$line,2);
	if (count($line_data) == 2) {
		$left = $line_data[0];
		$right = $line_data[1];
	}else {
		$left = '';
		$right = $line;
	}
	$rights = array();
	$rest = $right;
	$i = 0;
	while(!empty($rest)){
		$rest = trim($rest);
		if ($rest[0] == '"') {
			$rest = substr($rest,1);
			if ($pos = strpos($rest,'"')) {
				$rights[] = array('type'=>'string','value'=>'"'.substr($rest,0,$pos).'"');
				$rest = substr($rest,$pos+1);
			}else {
				return '';
			}
		}elseif (strpos($rest,'fetch') == 0) {
			$rest = substr($rest,5);
			$rest = trim($rest);
			$rights[] = array('type'=>'function','value'=>'"'.substr($rest,0,$pos).'"');
		}
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