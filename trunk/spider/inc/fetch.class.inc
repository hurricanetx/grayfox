<?php
/**
 * Enter description here...
 *
 */
class gf_fetch{
	private $time_out = 5;
	private $cookie = null;
	private $useragent = null;
	public function __construct($setting){
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
?>