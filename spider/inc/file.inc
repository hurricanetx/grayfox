<?php
class grayfox_file{
	private static $default_options = array(
	'method'        => 'get',
	'return_info'    => true,
	'return_body'    => true,
	'cache'            => false,
	'referer'        => '',
	'headers'        => array(),
	'session'        => false,
	'session_close'    => false,
	'cookie'=>'',
	);
	private static $cache_folder = 'E:/down/cache';
	private static $download_folder = 'E:/down';
	private static $curl_timeout = 10;
	private function __construct(){}
	private static function seperate_response($response,$header_size=null){
		if (empty($header_size)) {
			//Seperate header and content
			$separator_position = strpos($response,"\r\n\r\n");
			$header_text = substr($response,0,$separator_position);
			$body = substr($response,$separator_position+4);
		}else {
			$header_text = substr($response, 0, $header_size);
			$body = substr($response, $header_size);
		}
		$headers = array();
		foreach(explode("\n",$header_text) as $line) {
			$parts = explode(": ",$line);
			if(count($parts) == 2) $headers[$parts[0]] = chop($parts[1]);
		}
		return array($headers,$body);
	}
	public static function fetch($url,$options=array()){
		foreach(self::$default_options as $opt=>$value) {
			if(!isset($options[$opt]))
			{
				$options[$opt] = $value;
			}
		}
		$url_parts = parse_url($url);
		$ch = false;
		$info = array(//Currently only supported by curl.
		'http_code'    => 200
		);
		$response = '';

		$send_header = array(
		'Accept' => 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
		'Accept-Language' => 'zh-cn,zh;q=0.5',
		'Accept-Charset' => 'GB2312,utf-8;q=0.7,*;q=0.7',
		'Connection' => 'keep-alive',
		'User-Agent' => 'Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.1.1) Gecko/20090715 Firefox/3.5.1'
		) + $options['headers']; // Add custom headers provided by the user.

		if($options['cache']) {
			$cache_folder = self::$cache_folder;
			if(isset($options['cache_folder'])) $cache_folder = $options['cache_folder'];
			if(!file_exists($cache_folder)) {
				$old_umask = umask(0); // Or the folder will not get write permission for everybody.
				mkdir($cache_folder, 0777);
				umask($old_umask);
			}

			$cache_file_name = md5($url) . '.cache';
			$cache_file = $cache_folder.'/'.$cache_file_name;

			if(file_exists($cache_file)) { // Cached file exists - return that.
				$response = file_get_contents($cache_file);
				list($headers,$body) = self::seperate_response($response);
				$headers['cached'] = true;

				if(!$options['return_info']) return $body;
				else return array('headers' => $headers, 'body' => $body, 'info' => array('cached'=>true));
			}
		}
		///////////////////////////// Curl /////////////////////////////////////
		//If curl is available, use curl to get the data.
		if(function_exists("curl_init")
		and (!(isset($options['use']) and $options['use'] == 'fsocketopen'))) { //Don't use curl if it is specifically stated to use fsocketopen in the options

			if(isset($options['post_data'])) { //There is an option to specify some data to be posted.
				$page = $url;
				$options['method'] = 'post';

				if(is_array($options['post_data'])) { //The data is in array format.
					$post_data = array();
					foreach($options['post_data'] as $key=>$value) {
						$post_data[] = "$key=" . urlencode($value);
					}
					$url_parts['query'] = implode('&', $post_data);

				} else { //Its a string
					$url_parts['query'] = $options['post_data'];
				}
			} else {
				if(isset($options['method']) and $options['method'] == 'post') {
					$page = $url_parts['scheme'] . '://' . $url_parts['host'] . $url_parts['path'];
				} else {
					$page = $url;
				}
			}

			if($options['session'] and isset($GLOBALS['_binget_curl_session'])) $ch = $GLOBALS['_binget_curl_session']; //Session is stored in a global variable
			else $ch = curl_init($url_parts['host']);

			curl_setopt($ch, CURLOPT_URL, $page) or die("Invalid cURL Handle Resouce");
			curl_setopt($ch, CURLOPT_RETURNTRANSFER, true); //Just return the data - not print the whole thing.
			curl_setopt($ch, CURLOPT_HEADER, true); //We need the headers
			curl_setopt($ch, CURLOPT_NOBODY, !($options['return_body'])); //The content - if true, will not download the contents. There is a ! operation - don't remove it.
			if(isset($options['method']) and $options['method'] == 'post' and isset($url_parts['query'])) {
				curl_setopt($ch, CURLOPT_POST, true);
				curl_setopt($ch, CURLOPT_POSTFIELDS, $url_parts['query']);
			}
			//Set the headers our spiders sends
			curl_setopt($ch, CURLOPT_USERAGENT, $send_header['User-Agent']); //The Name of the UserAgent we will be using ;)
			$custom_headers = array("Accept: " . $send_header['Accept'] );
			$custom_headers[] = "Accept-Language: zh-cn,zh;q=0.5" ;
			$custom_headers[] = "Accept-Charset: GB2312,utf-8;q=0.7,*;q=0.7" ;
			$custom_headers[] = "Connection: keep-alive" ;
			$custom_headers[] = "Keep-Alive: 300";
			if(isset($options['modified_since'])){
				array_push($custom_headers,"If-Modified-Since: ".gmdate('D, d M Y H:i:s \G\M\T',strtotime($options['modified_since'])));
			}
			if(isset($url_parts['user']) and isset($url_parts['pass'])) {
				$custom_headers = array("Authorization: Basic ".base64_encode($url_parts['user'].':'.$url_parts['pass']));
			}
			print_r($custom_headers);
			curl_setopt($ch, CURLOPT_HTTPHEADER, $custom_headers);
			if(!empty($options['referer'])){
				curl_setopt($ch, CURLOPT_REFERER, $options['referer']);
			}
			if(!empty($options['cookie'])) {
				curl_setopt($ch, CURLOPT_COOKIE,$options['cookie']);
			}
			curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
			curl_setopt($ch, CURLOPT_MAXREDIRS, 5);
			curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
			curl_setopt($ch, CURLOPT_TIMEOUT,self::$curl_timeout);

			$response = curl_exec($ch);
			$info = curl_getinfo($ch); //Some information on the fetch

			if($options['session'] and !$options['session_close']) $GLOBALS['_binget_curl_session'] = $ch; //Dont close the curl session. We may need it later - save it to a global variable
			else curl_close($ch);  //If the session option is not set, close the session.

			////////////////////////////////////// FSockOpen //////////////////////////////
		} else { //If there is no curl, use fsocketopen - but keep in mind that most advanced features will be lost with this approch.
			if(isset($url_parts['query'])) {
				if(isset($options['method']) and $options['method'] == 'post')
				$page = $url_parts['path'];
				else
				$page = $url_parts['path'] . '?' . $url_parts['query'];
			} else {
				$page = $url_parts['path'];
			}

			if(!isset($url_parts['port'])) $url_parts['port'] = 80;
			$fp = fsockopen($url_parts['host'], $url_parts['port'], $errno, $errstr, 30);
			if ($fp) {
				$out = '';
				if(isset($options['method']) and $options['method'] == 'post' and isset($url_parts['query'])) {
					$out .= "POST $page HTTP/1.1\r\n";
				} else {
					$out .= "GET $page HTTP/1.0\r\n"; //HTTP/1.0 is much easier to handle than HTTP/1.1
				}
				$out .= "Host: $url_parts[host]\r\n";
				$out .= "Accept: $send_header[Accept]\r\n";
				$out .= "User-Agent: {$send_header['User-Agent']}\r\n";
				if(isset($options['modified_since']))
				{
					$out .= "If-Modified-Since: ".gmdate('D, d M Y H:i:s \G\M\T',strtotime($options['modified_since'])) ."\r\n";
				}
				$out .= "Connection: Close\r\n";

				//HTTP Basic Authorization support
				if(isset($url_parts['user']) and isset($url_parts['pass'])) {
					$out .= "Authorization: Basic ".base64_encode($url_parts['user'].':'.$url_parts['pass']) . "\r\n";
				}

				//If the request is post - pass the data in a special way.
				if(isset($options['method']) and $options['method'] == 'post' and $url_parts['query']) {
					$out .= "Content-Type: application/x-www-form-urlencoded\r\n";
					$out .= 'Content-Length: ' . strlen($url_parts['query']) . "\r\n";
					$out .= "\r\n" . $url_parts['query'];
				}
				$out .= "\r\n";

				fwrite($fp, $out);
				while (!feof($fp)) {
					$response .= fgets($fp, 128);
				}
				fclose($fp);
			}
		}

		//Get the headers in an associative array
		$headers = array();

		if($info['http_code'] == 404) {
			$body = "";
			$headers['Status'] = 404;
		} else {
			//Seperate header and content
			$header_size = empty($info['header_size'])?null:$info['header_size'];
			list($headers,$body) = self::seperate_response($response,$header_size);
		}

		if(isset($cache_file)) { //Should we cache the URL?
			file_put_contents($cache_file, $response);
		}
		if(empty($options['return_info']))return $body;
		return array('headers' => $headers, 'body' => $body, 'info' => $info);
	}
	public static function download($url,$download_folder=null,$options=array(),$flag=true){
		if (empty($download_folder)) {
			$download_folder = self::$download_folder;
		}
		$options['return_info'] = true;
		$options['return_body'] = true;
		$file_contents = self::fetch($url,$options);
		$real_url = !empty($file_contents['info']['url'])?$file_contents['info']['url']:$url;
		$url_parts = parse_url($real_url);
		$file_name = basename($url_parts['path']);
		if (empty($file_name)) {
			$file_name = uniqid('grayfox');
		}
		$file = $download_folder.'/'.$file_name;
		if ($flag) {
			$file = $file.'.gfd';
		}
		if(file_put_contents($file,$file_contents['body'])){
			return $file_name;
		}else {
			return false;
		}
	}
}
//$url = "http://www.taobao.com/wangwang/download.php";
//grayfox_file::down($url);
?>