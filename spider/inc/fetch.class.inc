<?php
/**
 * Enter description here...
 *
 */
class gf_fetch{
	private $time_out = 5;
	private $cookie = null;
	private $useragent = null;
	private $header = null;
	private $referer = null;
	public function __construct($setting=array()){
		if ( isset($setting['timeout']) and $setting['timeout']>0)
		{
			$this->timeout = intval($setting['timeout']);
		}
		if ( isset($setting['cookie']))
		{
			$this->cookie = $setting['cookie'];
		}
		if ( isset($setting['useragent']))
		{
			$this->useragent = $setting['useragent'];
		}
		if ( isset($setting['header']))
		{
			$this->header = $setting['header'];
		}
		if ( isset($setting['referer']))
		{
			$this->referer = $setting['referer'];
		}
	}
	public function __destruct(){}
	/**
	 * 设置cookie
	 * 如果cookie存在,curl set option的时候会同时把cookie添加进去,在请求一些服务器端验证用户cookie信息的时候有用
	 * @param string $cookie cookie串,如：'A=123'
	 */
	function setcookie($cookie)
	{
		$this->cookie = $cookie;
	}
	/**
	 * 移除cookie
	 *
	 */
	function removecookie()
	{
		$this->cookie = null;
	}
	/**
	 * 设置useragent
	 * 如果useragent存在,curl set option的时候会同时把useragent添加进去
	 * @param string $useragent useragent
	 */
	function setuseragent($useragent)
	{
		$this->useragent = $useragent;
	}
	/**
	 * 移除useragent
	 */
	function removeuseragent()
	{
		$this->useragent = null;
	}
	/**
	 * 获取单个页面的内容和状态 (GET)
	 *
	 * @param string $url 你需要get页面的url	
	 * @param int $timeout 超时时间(秒) 可以在构造的时候设置
	 */
	function get($url,$timeout=null)
	{
		sleep(1);
		if ($timeout == null)
		{
			$timeout = $this->time_out;
		}
		$ch = curl_init();
		if(!$ch) {
			return array(null,null);
		}
		curl_setopt($ch, CURLOPT_URL,$url);
		if($this->cookie != null){
			curl_setopt($ch, CURLOPT_COOKIE,$this->cookie);
		}
		if ($this->useragent != null) {
			curl_setopt($ch, CURLOPT_USERAGENT,$this->useragent);
		}
		if ($this->header != null) {
			curl_setopt($ch, CURLOPT_HTTPHEADER,$this->header);
		}
		if ($this->referer != null) {
			curl_setopt($ch, CURLOPT_REFERER,$this->referer);
		}
		curl_setopt($ch, CURLOPT_ENCODING, 'gzip,deflate');
		curl_setopt($ch, CURLOPT_HEADER, false);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
		curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 1);
		curl_setopt($ch, CURLOPT_TIMEOUT,$timeout);
		$data = curl_exec($ch);
		$status = curl_getinfo($ch,CURLINFO_HTTP_CODE);
		curl_close($ch);
		return array($status,$data);
	}
	/**
	 * 获取单个页面的内容和状态 (POST)
	 *
	 * @param string $url 你需要get页面的url	
	 * @param string $body post的参数
	 * @param int $timeout 超时时间(秒) 可以在构造的时候设置
	 */
	function post($url,$body,$timeout=null)
	{
		if ($timeout == null)
		{
			$timeout = $this->timeout;
		}
		$ch = curl_init();
		if(!$ch) {
			return array(null,null);
		}
		curl_setopt($ch, CURLOPT_URL,$url);
		if($this->cookie != null){
			curl_setopt($ch, CURLOPT_COOKIE,$this->cookie);
		}
		if ($this->useragent != null) {
			curl_setopt($ch, CURLOPT_USERAGENT,$this->useragent);
		}
		if ($this->header != null) {
			curl_setopt($ch, CURLOPT_HTTPHEADER,$this->header);
		}
		if ($this->referer != null) {
			curl_setopt($ch, CURLOPT_REFERER,$this->referer);
		}
		curl_setopt($ch, CURLOPT_ENCODING, 'gzip,deflate');
		curl_setopt($ch, CURLOPT_HEADER, false);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
		curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 1);
		curl_setopt($ch, CURLOPT_TIMEOUT,$timeout);
		curl_setopt($ch, CURLOPT_POST, true);
		curl_setopt($ch, CURLOPT_POSTFIELDS, $body);
		$data = curl_exec($ch);
		$status = curl_getinfo($ch,CURLINFO_HTTP_CODE);
		curl_close($ch);
		return array($status,$data);
	}
}
$custom_headers = array("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" );
$custom_headers[] = "Accept-Language: zh-cn,zh;q=0.5" ;
$custom_headers[] = "Accept-Charset: GB2312,utf-8;q=0.7,*;q=0.7" ;
$custom_headers[] = "Connection: keep-alive" ;
$custom_headers[] = "Keep-Alive: 300";
$setting = array(
'cookie'=>'JSESSIONID=79F0ACB4F5FC04DE19F0F1277DA0B0F3;_eforum_recent_forums=16E;_eforum_sid=79F0ACB4F5FC04DE19F0F1277DA0B0F3;_eforum_user_auto_login=1;_eforum_user_id=u00000027;_eforum_user_hash=2a39aa07a42e4eb2205f65e1bbcf9301;_eforum_user_mode=1;_eforum_user_visited_topicids=D1021;_eforum_new_forumids18=1248675230121',
'header'=>$custom_headers,
'useragent'=>'Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.1.1) Gecko/20090715 Firefox/3.5.1',
'referer'=>'http://www.morwa.cn/index.fhtm'
);
$f = new gf_fetch($setting);
print_r($f->get('http://www.morwa.cn/forum.fhtml'));
?>