<?php
require('inc/parse.class.inc');
require('inc/fetch.class.inc');
$base_dir = 'M:\\photo';
$root_url = 'http://www.moko.cc/logout_girlvote/index.html';
$fetcher = new gf_fetch();
list($status,$contents) = $fetcher->get($root_url);
$pattern1 =<<<REG
/<a[^>]+href=["']\/([^"^']+?)\/girlvote.html["'][^>]*>([^<^\d]+)<\/a>/is
REG;
$pattern2 =<<<REG
/<a[^>]+href=["']\/([^"^']+?)["'][^>]*>READ MORE/is
REG;
$pattern3 =<<<REG
/<a[^>]+href=["']\/([^"^']+?)["'][^>]*>&gt;([^<]+)\(\d+\)<\/a>/is
REG;
$pattern4 =<<<REG
/<title>(.*+?)<\/title>/is
REG;
$pattern5 =<<<REG
/<img[^>]+src=["']\/([^"^']+?\.jpg)["'][^>]*>/is
REG;
$mm_list = gf_preg::grep($contents,$pattern1);
foreach ($mm_list as $mm) {
	$mm_id = $mm[1];
	$mm_name = $mm[2];
	$mm_name = iconv('UTF-8//IGNORE','GB2312//IGNORE',$mm_name);
	$mm_dir = $base_dir.'\\'.$mm_name;
	$mm_photo_url = "http://www.moko.cc/post/{$mm_id}/indexpost.html";
	list($status,$mm_contents) = $fetcher->get($mm_photo_url);
	$type_list = gf_preg::grep($mm_contents,$pattern3);
	if (!empty($type_list)) {
		$series_list = array();
		foreach ($type_list as $type) {
			$type_name = $type[2];
			$type_url = "http://www.moko.cc/".$type[1];
			list($status,$type_contents) = $fetcher->get($type_url);
			$series_list[$type_name] = gf_preg::grep($type_contents,$pattern2);
		}
	}else {
		$series_list['展示'] = gf_preg::grep($mm_contents,$pattern2);
	}
	foreach ($series_list as $series_name=>$series) {
		$series_name = iconv('UTF-8//IGNORE','GB2312//IGNORE',$series_name);
		$series_dir = $mm_dir.'\\'.$series_name;
		foreach ($series as $album) {
			$album_url = "http://www.moko.cc/".$album[1];
			list($status,$album_contents) = $fetcher->get($album_url);
			$album_info = gf_preg::grep($album_contents,$pattern4);
			$album_name = $album_info[0][1];
			$album_name = iconv('UTF-8//IGNORE','GB2312//IGNORE',$album_name);
			$album_dir = $series_dir.'\\'.$album_name;
			if (!file_exists($album_dir)) {
				@mkdir($album_dir,755,true);
			}
			chdir($album_dir);
			$photo_list = gf_preg::grep($album_contents,$pattern5);
			foreach ($photo_list as $photo) {
				$photo_url = "http://www.moko.cc/".$photo[1];
				echo $photo_url."\n";
			}
		}
	}
	exit();
}
?>