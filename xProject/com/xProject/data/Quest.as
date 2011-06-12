package com.xProject.data
{
	import mx.formatters.DateFormatter;
	import mx.formatters.NumberFormatter;

	[Bindable]
	public class Quest
	{
		public var id:int;
		public var title:String;
		public var description:String;
		public var start_time:String;
		public var finish_time:String;
		public var deadline:String;
		public var percent:int;
		public var members:Array;
		
		public function Quest( id:int = -1, title:String = '', description:String = '', 
							   start_time:String = '', finish_time:String = '', deadline:String= '',
							   percent:int = 0)
		{
			this.id = id;
			this.title = title;
			this.description = description;
			this.start_time = start_time;
			this.finish_time = finish_time;
			this.deadline = deadline;
			this.percent = percent;
		}
		public function serialize():void
		{
			title = escape( title );
			description = escape( description );
			start_time = escape( start_time );
			finish_time = escape( finish_time );
			deadline = escape( deadline );
		}
		public function deserialize():void
		{
			title = unescape( title );
			description = unescape( description );
			start_time = unescape( start_time );
			finish_time = unescape( finish_time );
			deadline = unescape( deadline );
		}
		
		public static function create( value:Object ):Quest
		{
			var quest:Quest = new Quest();
			for( var prop:String in value )
			{
				quest[prop] = value[prop];
			}
			quest.deserialize();
			return quest;
		}
		
		public function AddMember(member:Staff):void
		{
			this.members.push(member);
		}
		
		public function updatePercent():void
		{
			var _percent:Number = 0;
			for(var i:int = 0;i < this.members.length;i++){ 
				_percent +=  (this.members[i].assign_percent / 100 ) * (this.members[i].percent / 100 ) * 100;
			}
			var e:NumberFormatter;
			this.percent = _percent;
		}
	}
}