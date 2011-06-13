package com.xProject.net
{
	import flash.events.ServerSocketConnectEvent;
	import flash.net.ServerSocket;
	
	public class XSocket
	{
		private var _port:int=12126;
		private var _server:ServerSocket;
		
		public function XSocket()
		{
			this.port=port;
		}
		
		public function get port():int
		{
			return _port;
		}
		
		public function set port(value:int):void
		{
			_port = value;
		}
		
		public function start():void{
			try{
				_server=new ServerSocket;
				_server.bind(port);
				_server.listen();
				_server.addEventListener(ServerSocketConnectEvent.CONNECT,_server_connectHandler);
				//xProject.mainApp.writeLine("服务器已经启动，端口为："+port);
			}catch(error:Error){
				//xProject.mainApp.writeLine(error.message);
			}
		}
		
		private function _server_connectHandler(event:ServerSocketConnectEvent):void{
			ClientConnection.addClientConnection(new ClientConnection(event.socket));
		}
	}
}