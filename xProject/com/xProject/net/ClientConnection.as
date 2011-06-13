package com.xProject.net
{
	import flash.events.Event;
	import flash.events.ProgressEvent;
	import flash.net.Socket;
	import flash.utils.ByteArray;
	
	public class ClientConnection
	{
		private static var __socketArr:Vector.<ClientConnection>
		
		public function ClientConnection()
		{
			if (__socketArr == null)
			{
				__socketArr=new Vector.<ClientConnection>;
			}
			
			this.socket=socket;
			this.socket.addEventListener(ProgressEvent.SOCKET_DATA, socket_socketDataHandler);
			this.socket.addEventListener(Event.CLOSE, socket_closeHandler);
		}
		public static function removeClientConnection(ccc:ClientConnection):void
		{
			var len:int=__socketArr.length;
			for (var i:int=0; i < len; i++)
			{
				if (__socketArr[i] == ccc)
				{
					__socketArr.splice(i, 1);
					break;
				}
			}
		}
		
		public static function broadcastOnlineUserCount():void
		{
			var bytes:ByteArray=new ByteArray;
			bytes.writeByte(PakHeadType.ONLINE_USER_COUNT);
			bytes.writeInt(__socketArr.length);
			sendMsgToAll(bytes);
		}
		
		public static function broadcastLogMsg(msg:String):void
		{
			var bytes:ByteArray=new ByteArray;
			bytes.writeByte(PakHeadType.LOG_MSG);
			bytes.writeUTFBytes(msg);
			sendMsgToAll(bytes);
		}
		
		public static function broadcastOnlineUserCountAndLogMsg(msg:String):void
		{
			var bytes:ByteArray=new ByteArray;
			bytes.writeByte(PakHeadType.ONLINE_USER_COUNT);
			bytes.writeInt(__socketArr.length);
			bytes.writeByte(PakHeadType.LOG_MSG);
			bytes.writeUTFBytes(msg);
			
			sendMsgToAll(bytes);
		}
		
		public static function addClientConnection(ccc:ClientConnection):void
		{
			__socketArr.push(ccc);
		}
		
		public static function sendMsgToAll(bytes:ByteArray):void
		{
			for each (var ccc:ClientConnection in __socketArr)
			{
				ccc.sendMsg(bytes);
			}
		}
		
		private var _socket:Socket;
		
		public function get socket():Socket
		{
			return _socket;
		}
		
		public function set socket(value:Socket):void
		{
			_socket=value;
		}
		
		private var _userName:String="";
		
		public function get userName():String
		{
			return _userName;
		}
		
		public function set userName(value:String):void
		{
			_userName=value;
		}
		
		
		public function sendMsg(bytes:ByteArray):void
		{
			socket.writeBytes(bytes);
			socket.flush();
		}
		
		private function socket_socketDataHandler(event:ProgressEvent):void
		{
			var socketBytes:ByteArray=new ByteArray;
			_socket.readBytes(socketBytes);
			socketBytes.position=0;
			
			if (socketBytes.readUTFBytes(socketBytes.bytesAvailable) == "<policy-file-request/>")
			{
				//向客户端返回授权文件
				_socket.writeUTFBytes(PolicyFile.FILE_CONTENT);
				_socket.flush();
				//trace(PolicyFile.FILE_CONTENT);
				return;
			}
			
			
			var bytes:ByteArray;
			var msg:String="";
			socketBytes.position=0;
			switch (socketBytes.readByte())
			{
				case PakHeadType.CHAT_MSG:
					msg=socketBytes.readUTFBytes(socketBytes.bytesAvailable);
					bytes=new ByteArray;
					bytes.writeByte(PakHeadType.CHAT_MSG);
					bytes.writeUTFBytes(userName + ":" + msg);
					sendMsgToAll(bytes);
					
					//ChatServer.mainApp.writeLine(userName + ":" + msg);
					break;
				case PakHeadType.LOGIN:
					this.userName=socketBytes.readUTFBytes(socketBytes.bytesAvailable);
					broadcastOnlineUserCountAndLogMsg(userName + "上线了");
					
					//ChatServer.mainApp.writeLine(this.userName + "登陆了");
					//ChatServer.mainApp.writeLine("当前在线人数:" + __socketArr.length);
					break;
			}
		}
		
		private function socket_closeHandler(event:Event):void
		{
			this.socket.removeEventListener(ProgressEvent.SOCKET_DATA, socket_socketDataHandler);
			this.socket.removeEventListener(Event.CLOSE, socket_closeHandler);
			
			removeClientConnection(this);
			
			broadcastOnlineUserCountAndLogMsg(userName + "离开了");
			
			//ChatServer.mainApp.writeLine(userName + " 离开了");
			//ChatServer.mainApp.writeLine("当前在线人数:" + __socketArr.length);
		}
	}
}