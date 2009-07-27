<?php
require('../inc/file.inc');
require('../inc/content.inc');
require('../inc/database.inc');

$file_dir = './www.morwa.cn/';
$bbs_url = "http://www.morwa.cn/forum.fhtml";
$referer = "http://www.morwa.cn/index.fhtml";
$cookie = "JSESSIONID=6C60B2E90CDF6A32E56EF772F5C05BEE&_eforum_sid=6C60B2E90CDF6A32E56EF772F5C05BEE";

$options['referer'] = $referer;
$options['cookie'] = $cookie;
$options['return_info'] = false;

$bbs_page = grayfox_file::fetch($bbs_url,$options);
$tmp_boards = grayfox_content::reg($bbs_page,'/<div class="title">\s*<a href="(http:\/\/www\.morwa\.cn)?\/topics-(\d+)-1\.fhtml".+?>(.+?)<\/a>\s*<\/div>/is');
if (empty($tmp_boards)) {
	var_dump( $tmp_boards);
	exit();
}
foreach ($tmp_boards as $board) {
	$board_id = $board[2];
	$board_url = "http://www.morwa.cn/topics-{$board_id}-1.fhtml";
	$board_dir = $file_dir.'topics-'.$board_id.'/';
	$board_attachment_dir = $board_dir.'attachment/';
	if (!file_exists($board_dir)) {
		@mkdir($board_dir,777,true);
	}
	if (!file_exists($board_attachment_dir)) {
		@mkdir($board_attachment_dir,777,true);
	}
	$board_name = $board[3];
	$board_page = grayfox_file::fetch($board_url,$options);
	$tmp_threads = grayfox_content::reg($board_page,'/<div class="topicTitle">\s*<a href="(http:\/\/www\.morwa\.cn)?\/post-(\d+)-1\.fhtml".+?>(.+?)<\/a>.+?<\/div>/is');
	if (empty($tmp_threads)) {
		var_dump($tmp_threads);
		exit();
	}
	foreach ($tmp_threads as $thread) {
		$thread_id = $thread[2];
		$thread_file = $board_dir.'post-'.$thread_id.'.txt';
		$thread_attachment_file = $board_dir.'post-'.$thread_id.'attachments.txt';
		$thread_attachment_list = array();
		if (file_exists($thread_file)) {
			continue;
		}
		$thread_url = "http://www.morwa.cn/post-{$thread_id}-1.fhtml";
		$thread_title = $thread[3];
		$thread_page = grayfox_file::fetch($thread_url,$options);
		$thread_infos = grayfox_content::reg($thread_page,'/<div[^>]+?class="message"[^>]+?>(.+?)<\/div>/is');
		$thread_contents = $thread_infos[0][1];
		$thread_contents = preg_replace('/<script.*?<\/script>/is','',$thread_contents);
		$tmp_attachments = grayfox_content::reg($thread_contents,'/<span class="ubb_attach_info">.*?<a href="(http:\/\/www\.morwa\.cn)?\/download.do\?id=(\d+)".*?>(.+?)<\/a><\/span>/is');
		if (!empty($tmp_attachments)) {
			foreach ($tmp_attachments as $attachment) {
				$attachment_url = "http://www.morwa.cn/download.do?id={$attachment[2]}";
				$attachment_name = grayfox_file::download($attachment_url,$board_attachment_dir,$options,false);
				if ($attachment_name) {
					$thread_attachment_list[] = $attachment_name;
				}
			}
		}
		$thread_contents = "Link:".$thread_url."\r\nTitle:".$thread_title."\r\nContent:\r\n".$thread_contents;
		file_put_contents($thread_file,$thread_contents);
		file_put_contents($thread_attachment_file,join("\r\n",$thread_attachment_list));
	}
}
?>