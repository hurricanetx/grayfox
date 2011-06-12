package com.xProject.events
{    
    import flash.data.SQLResult;
    import flash.events.Event;

    public class DBResultEvent extends Event
    {
        private var _result:SQLResult;
        
        public static const QUEST_SAVED:String = "QuestSaved";
		public static const QUEST_GET:String = "QuestGet";
		public static const QUEST_GET_ALL:String = "QuestGetAll";
        public static const STAFF_SAVED:String = "StaffSaved";
		public static const MEMBER_SAVED:String = "MemberSaved";

        
        public function DBResultEvent( type:String, sqlResult:SQLResult )
        {
            super( type );
            _result = sqlResult;
        }
        
        public function get result():SQLResult
        {
            return _result;
        }
    }
}