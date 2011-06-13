package com.xProject.services
{
    import com.xProject.data.Query;
    import com.xProject.events.DBResultEvent;
    
    import flash.data.SQLConnection;
    import flash.data.SQLResult;
    import flash.data.SQLStatement;
    import flash.events.Event;
    import flash.events.EventDispatcher;
    import flash.events.SQLErrorEvent;
    import flash.events.SQLEvent;
    import flash.filesystem.File;

	[Event(name="creationComplete", type="flash.events.Event")]
	[Event(name="creationError", type="flash.events.Event")]
    public class DBService extends EventDispatcher
    {
		private var _connection:SQLConnection;
		private var _statement:SQLStatement;
		private var _dbFile:File;
		
		private var _isOpen:Boolean;
		private var _queryQueue:Array;
		private var _currentQuery:Query;
		private var _isRunningQuery:Boolean;
		private var test:Vector.<Query>;
		
		private static const DB:String = "xProjectConfig.db";

        public function DBService(DB:String)
        {
			_connection = new SQLConnection();
			_connection.addEventListener( SQLEvent.OPEN, onConnectionOpen );
			_connection.addEventListener( SQLEvent.CLOSE, onConnectionClose );
			_connection.addEventListener( SQLErrorEvent.ERROR, onConnectionError );
			
			_queryQueue = new Array();
			_dbFile = File.applicationStorageDirectory.resolvePath( DB );
			invalidate();
        }
		private function invalidate():void
		{
			_statement = new SQLStatement();
			_statement.sqlConnection = _connection;
			_statement.addEventListener(SQLEvent.RESULT, onCreationQueryResult);
			_statement.addEventListener(SQLErrorEvent.ERROR, onCreationQueryError);
			
			var sqlCreateQuest:String = "CREATE TABLE IF NOT EXISTS userconfig (" +
				"name    TEXT    PRIMARY KEY," +
				"pass    TEXT    NOT NULL," +
				"description    TEXT    NOT NULL," +
				"start_time    TEXT    NOT NULL," +
				"finish_time    TEXT    NOT NULL," +
				"deadline    TEXT    NOT NULL," +
				"percent    INTEGER    NOT NULL);";
			
			var sqlCreateStaff:String = "CREATE TABLE IF NOT EXISTS localconfig (" +
				"id    INTEGER PRIMARY KEY," +
				"auto_start    INTEGER    NOT NULL," +
				"description    TEXT    NOT NULL);";
			
			addQuery( new Query( CREATE_TABLE_QUEST, sqlCreateQuest ), false ); 
			addQuery( new Query( CREATE_TABLE_STAFF, sqlCreateStaff ), false );
			addQuery( new Query( CREATE_TABLE_MEMBER, sqlCreateMember ), true );
		}
		
		private function open():void
		{
			if( _isOpen ) return;
			_connection.open( _dbFile );
		}
		
		private function executeNextQuery():void
		{
			if( !_isOpen ) open();
			else
			{
				if( _queryQueue.length > 0 )
				{
					if( !_isRunningQuery ) _isRunningQuery = true;
					_currentQuery = _queryQueue.shift();
					_statement.text = _currentQuery.query;
					_statement.execute();
				}
				else
				{
					_isRunningQuery = false;
					_currentQuery = null;
					_connection.close();
				}
			}
		}
		
		private function onConnectionOpen( evt:SQLEvent ):void
		{
			_isOpen = true;
			if( _queryQueue.length > 0 ) executeNextQuery();
		}
		private function onConnectionClose( evt:SQLEvent ):void
		{
			_isOpen = false;
			if( _queryQueue.length > 0 )
			{
				_queryQueue.splice( 0, 0, _currentQuery );
				_currentQuery = null;
				_isRunningQuery = false;
			}
		}
		private function onConnectionError( evt:SQLErrorEvent ):void
		{
			trace( "Database connection error!" );
		}
		
		private function onCreationQueryResult( evt:SQLEvent ):void
		{
			if( _queryQueue.length > 0 )
			{
				executeNextQuery();
			}
			else
			{
				_statement = new SQLStatement();
				_statement.sqlConnection = _connection;
				_statement.addEventListener( SQLEvent.RESULT, onQueryResult );
				_statement.addEventListener( SQLErrorEvent.ERROR, onQueryError );
				dispatchEvent( new Event( CREATION_COMPLETE ) );
				_isRunningQuery = false;
			}
		}
		
		private function onCreationQueryError( evt:SQLErrorEvent ):void
		{
			dispatchEvent( new Event( CREATION_ERROR ) );
			_isRunningQuery = false;
		}
		
		private function onQueryResult( evt:SQLEvent ):void
		{
			var result:SQLResult = _statement.getResult();
			if( _currentQuery.type != FALL_THROUGH )
				dispatchEvent( new DBResultEvent( _currentQuery.type, result ) );
			
			executeNextQuery();
		}
		private function onQueryError( evt:SQLErrorEvent ):void
		{
			dispatchEvent( evt );
			_isRunningQuery = false;
		}
		
		public function saveQuest( quest:Quest ):void
		{
			quest.serialize();
			var sqlSave:String;
			var sqlSelect:String;
			if( quest.id == -1 )
			{
				// add new location...
				sqlSave = "INSERT INTO quest VALUES (" +
					"null," +
					"'" + quest.title + "'," +
					"'" + quest.description + "'," +
					"'" + quest.start_time + "'," +
					"'" + quest.finish_time + "'," +
					"'" + quest.deadline + "'," +
					"'" + quest.percent + "');";
				sqlSelect = "SELECT * FROM quest ORDER BY id DESC LIMIT 1;";
			}
			else
			{
				// update saved location...
				sqlSave = "UPDATE quest SET " +
					"title='" + quest.title + "'," +
					"description='" + quest.description + "'," +
					"start_time='" + quest.start_time + "'," +
					"finish_time='" + quest.finish_time + "'," +
					"deadline='" + quest.deadline + "'," +
					"percent='" + quest.percent + "' " +
					"WHERE id='" + quest.id + "';";
				sqlSelect = "SELECT * FROM quest WHERE id='" +
					quest.id + "';";
			}
			addQuery( new Query( FALL_THROUGH, sqlSave ), false );
			addQuery( new Query(DBResultEvent.QUEST_SAVED, sqlSelect), true );
			quest.deserialize();
		}
		
		public function getQuest(questId:int):void
		{
			var sqlSelect:String = "SELECT * FROM quest where id ='"+ questId +"';";
			addQuery( new Query(DBResultEvent.QUEST_GET, sqlSelect), true );
		}
		
		public function getQuests(evt:Event = null):void
		{
			var sqlSelect:String = "SELECT * FROM quest ORDER BY id DESC;";
			addQuery( new Query(DBResultEvent.QUEST_GET_ALL, sqlSelect), true );
		}
		
		public function saveStaff( staff:Staff ):void
		{
			staff.serialize();
			var sqlSave:String;
			var sqlSelect:String;
			if( staff.id == -1 )
			{
				// add new marker...
				sqlSave = "INSERT INTO staff VALUES (" +
					"null," +
					"'" + staff.name + "'," +
					"'" + staff.description + "');";
				sqlSelect = "SELECT * FROM staff ORDER BY id DESC LIMIT 1;";
			}
			else
			{
				// update saved marker...
				sqlSave = "UPDATE staff SET " +
					"name='" + staff.name + "'," +
					"description='" + staff.description + "'," +
					"WHERE id='" + staff.id + "';";
				sqlSelect = "SELECT * FROM markers WHERE id='" + staff.id + "';";
			}
			addQuery( new Query( FALL_THROUGH, sqlSave ), false );
			addQuery( new Query( DBResultEvent.STAFF_SAVED, sqlSelect ),true );
			staff.deserialize();
		}
		
		public function addQuery( query:Query, run:Boolean = true ):void
		{
			_queryQueue.push( query );
			if( !_isRunningQuery && run ) runQuery();
		}
		
		public function runQuery():void
		{
			if( _queryQueue.length > 0 ) executeNextQuery();
		}
		
		public function shutDown():void
		{
			if( _isRunningQuery )
			{
				_statement.removeEventListener(SQLEvent.RESULT, onQueryResult);
				_statement.removeEventListener(SQLErrorEvent.ERROR, onQueryError);
			}
			close();
		}
		
		public function close():void
		{
			if( _isOpen ) _connection.close();
		}
    }
}