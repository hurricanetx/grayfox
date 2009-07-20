<?php
require('inc/parse.class.inc');
require('inc/fetch.class.inc');
$base_dir = 'M:\\photo';
$root_url = 'http://www.moko.cc/logout_girlvote/index.html';
$setting = array(
'cookie'=>'SERVERID=_web8888-17_&JSESSIONID=F98BA38C895A946759F2F360EBDEFF58',
'useragent'=>'Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.1.1) Gecko/20090715 Firefox/3.5.1',
'referer'=>'http://www.moko.cc/logout_girlvote/index.html',
'header'=>array(
'Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
'Accept-Language: zh-cn,zh;q=0.5',
'Accept-Charset: GB2312,utf-8;q=0.7,*;q=0.7',
'Connection: keep-alive',
'Keep-Alive: 300',
),
);
$fetcher = new gf_fetch($setting);
list($status,$contents) = $fetcher->get($root_url);
$pattern1 =<<<REG
/<a[^>]+href=["']\/([^"^']+?)\/girlvote.html["'][^>]*>([^<^\d]+)<\/a>/is
REG;
$pattern2 =<<<REG
/<a[^>]+href=["']\/([^"^']+?)["'][^>]*>READ MORE/is
REG;
$pattern3 =<<<REG
/<a[^>]+href=["']\/([^"^']+?)["'][^>]*>&gt;&nbsp;([^<]+)\(\d+\)<\/a>/is
REG;
$pattern4 =<<<REG
/<p class="mainColor weight700 mainDashedOn">([^\/^\\^\|]+?)<\/p>/is
REG;
$pattern5 =<<<REG
/<img[^>]+src=["']\/(users[^"^']+?\.jpg)["'][^>]*>/is
REG;
$done_list = explode(',',file_get_contents($base_dir.'\\'.'done.txt'));
$mm_list = gf_preg::grep($contents,$pattern1);
foreach ($mm_list as $mm) {
	$mm_id = $mm[1];
	if (in_array($mm_id,$done_list)) {
		continue;
	}
	$mm_name = $mm[2];
	$mm_name = iconv('UTF-8//IGNORE','GB2312//IGNORE',$mm_name);
	$mm_dir = $base_dir.'\\'.$mm_name;
	$mm_photo_url = "http://www.moko.cc/post/{$mm_id}/indexpost.html";
	list($status,$mm_contents) = $fetcher->get($mm_photo_url);
	$type_list = gf_preg::grep($mm_contents,$pattern3);
	$series_list = array();
	if (!empty($type_list)) {
		foreach ($type_list as $type) {
			$type_name = $type[2];
			$type_url = "http://www.moko.cc/".$type[1];
			list($status,$type_contents) = $fetcher->get($type_url);
			$series_list[$type_name] = gf_preg::grep($type_contents,$pattern2);
			unset($type_contents);
		}
	}else {
		$series_list['展示'] = gf_preg::grep($mm_contents,$pattern2);
	}
	unset($mm_contents);
	foreach ($series_list as $series_name=>$series) {
		$series_name = iconv('UTF-8//IGNORE','GB2312//IGNORE',$series_name);
		$series_name = str_replace(array('/','\\','|',':','\'','"','*','?'),'',$series_name);
		$series_dir = $mm_dir.'\\'.$series_name;
		foreach ($series as $album) {
			$album_url = "http://www.moko.cc/".$album[1];
			list($status,$album_contents) = $fetcher->get($album_url);
			$album_info = gf_preg::grep($album_contents,$pattern4);
			$album_name = $album_info[0][1];
			$album_name = iconv('UTF-8//IGNORE','GB2312//IGNORE',$album_name);
			$album_name = str_replace(array('/','\\','|',':','\'','"','*','?'),'',$album_name);
			$album_dir = $series_dir.'\\'.$album_name;
			if (!file_exists($album_dir)) {
				@mkdir($album_dir,755,true);
			}
			$photo_list = gf_preg::grep($album_contents,$pattern5);
			unset($album_contents);
			foreach ($photo_list as $photo) {
				$photo_url = "http://www.moko.cc/".$photo[1];
				$photo_name = basename($photo_url);
				$photo_path = $album_dir.'\\'.$photo_name;
				$photo_data = file_get_contents($photo_url);
				file_put_contents($photo_path,$photo_data);
				usleep(500);
			}
		}
	}
	$done_list[] = $mm_id;
	file_put_contents($base_dir.'\\'.'done.txt',join(',',$done_list));
	echo date('[Y-m-d H:i:s]').$mm_id."done.\n";
}
?>