<?php
function parse($string){
	$rerurn = '';
	$reserved_words = array('fetch','download','reg','xpath','filesave','dbinsert');
	$function_call = 0;
	
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
	$line_data = explode(':=',$line,2);
	if (count($line_data) == 2) {
		$left = $line_data[0];
		$right = parse($line_data[1]);
		$out .= "\${$left} = {$right};\n";
	}else {
		$right = parse($line);
		$out .= "{$right};\n";
	}
}
$out .= "?>";
file_put_contents($out_file,$out);
?>