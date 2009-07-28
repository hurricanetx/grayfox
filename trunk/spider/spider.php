<?php
require('inc/file.inc');
require('inc/content.inc');
require('inc/database.inc');

$board_dir = './files/www.weiphone.com/5/';
$board_attachment_dir = $board_dir.'attachment/';
$list_url = "http://www.weiphone.com/forumdisplay.php?fid=5&filter=type&typeid=132";
$referer = "http://www.weiphone.com/";
$cookie = "__utma=101886599.468671584.1248746069.1248746069.1248762457.2;__utmb=101886599;__utmc=101886599;__utmz=101886599.1248746069.1.1.utmccn=(direct)|utmcsr=(direct)|utmcmd=(none);-1ifocus_match_=1;cdb_fid5=1248764106;cdb_oldtopics=D447389D447214D447570D446571D;cdb_sid=UQrcBE;cdb_smile=1DD0D10;cdb_visitedfid=5D16D153";

$options['referer'] = $referer;
$options['cookie'] = $cookie;
$options['return_info'] = false;

$list_page = grayfox_file::fetch($list_url,$options);
$tmp_threads = grayfox_content::reg($list_page,'/<span id="thread_(\d+)"><a href="(.+?)".*?>(.+?)<\/a><\/span>/is');
if (empty($tmp_threads)) {
	exit();
}
if (!file_exists($board_dir)) {
	@mkdir($board_dir,777,true);
}
if (!file_exists($board_attachment_dir)) {
	@mkdir($board_attachment_dir,777,true);
}
foreach ($tmp_threads as $thread) {
	$thread_id = $thread[1];
	$thread_file = $board_dir.'post-'.$thread_id.'.txt';
	$thread_attachment_file = $board_dir.'post-'.$thread_id.'_attachments.txt';
	$thread_attachment_list = array();
	if (file_exists($thread_file)) {
		continue;
	}
	$thread_url = "http://www.weiphone.com/".$thread[2];
	$thread_title = $thread[3];
	$thread_page = grayfox_file::fetch($thread_url,$options);
	$thread_infos = grayfox_content::reg($thread_page,'/<div.+?id="postmessage_(\d+)">(.+?)<\/div>/is');
	$thread_contents = $thread_infos[0][2];
	$thread_contents = preg_replace('/<script.*?<\/script>/is','',$thread_contents);
	$thread_contents = "Link:".$thread_url."\r\nTitle:".$thread_title."\r\nContent:\r\n".$thread_contents;
	file_put_contents($thread_file,$thread_contents);
}
?>