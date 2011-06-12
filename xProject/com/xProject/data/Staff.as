package com.xProject.data
{
	[Bindable]
	public class Staff
	{
		public var id:int;
		public var name:String;
		public var description:String;
		
		public function Staff( id:int = -1, name:String = '', 
								  description:String = '')
		{
			this.id = id;
			this.name = name;
			this.description = description;
		}
		
		public function serialize():void
		{
			name = escape( name );
			description = escape( description );
		}
		
		public function deserialize():void
		{
			name = unescape( name );
			description = unescape( description );
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
	}
}