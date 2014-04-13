/*
 * Calcium - Domain Model Framework
 * See copyright notice in LICENSE.md
 */

#include "SQLite.h"
#include "sqlite3.h"
#include <sstream>

namespace fh {

/************************************************************************/
/* SQLiteResult                                                         */
/************************************************************************/

SQLiteResult::SQLiteResult( sqlite3_stmt* stmt )
{
	assert( stmt );
	_stmt = stmt;
}

SQLiteResult::SQLiteResult( const SQLiteResult& o )
{
	_stmt = o._stmt;
	o._stmt = NULL;
}

bool SQLiteResult::next()
{
	int status = sqlite3_step( _stmt );
	if( status == SQLITE_ERROR )
		throw co::Exception( "error getting next result in SQLiteResult" );

	return status != SQLITE_DONE;
}

void SQLiteResult::fetchRow()
{
	if( !next() )
		throw co::Exception( "unexpected empty SQLiteResult" );
}

bool SQLiteResult::hasData( int column )
{
	int count = sqlite3_data_count( _stmt );
	return count > 0 && column < count;
}

std::string SQLiteResult::getString( co::uint32 column )
{
	assert( hasData( column ) );
	return std::string( reinterpret_cast<const char*>( sqlite3_column_text( _stmt, column ) ) );
}

co::uint32 SQLiteResult::getUint32( co::uint32 column )
{
	assert( hasData( column ) );
	int v = sqlite3_column_int( _stmt, column );
	assert( v >= 0 );
	return static_cast<co::uint32>( v );
}

double SQLiteResult::getDouble( co::uint32 column )
{
	assert( hasData( column ) );
	double v = sqlite3_column_double( _stmt, column );
	return v;
}

/************************************************************************/
/* SQLiteStatement                                                      */
/************************************************************************/

SQLiteStatement::SQLiteStatement( sqlite3_stmt* stmt )
{
	assert( stmt );
	_stmt = stmt;
}

SQLiteStatement::SQLiteStatement( const SQLiteStatement& o)
{
	_stmt = o._stmt;
	o._stmt = NULL;
}

SQLiteStatement::~SQLiteStatement()
{
	finalize();
}

void SQLiteStatement::bind( co::uint32 index, co::int32 value )
{
	handleErrorCode( sqlite3_bind_int( _stmt, index, value ) );
}

void SQLiteStatement::bindDouble( co::uint32 index, double value )
{
	handleErrorCode( sqlite3_bind_double( _stmt, index, value ));
}

void SQLiteStatement::bind( co::uint32 index, const char* value )
{
	handleErrorCode( sqlite3_bind_text( _stmt, index, value, -1, NULL ));
}

void SQLiteStatement::bind( co::uint32 index, const char* value, int bytes )
{
	handleErrorCode( sqlite3_bind_text( _stmt, index, value, bytes, SQLITE_TRANSIENT ));
}

ISQLiteResult* SQLiteStatement::query()
{
	return new SQLiteResult( _stmt );
}

void SQLiteStatement::execute()
{
	handleErrorCode( sqlite3_step( _stmt ) );
}

void SQLiteStatement::reset()
{
	if( _stmt )
		sqlite3_reset( _stmt );
}

void SQLiteStatement::finalize()
{
	if( _stmt )
	{
		sqlite3_finalize( _stmt );
		_stmt = NULL;
	}
}

void SQLiteStatement::handleErrorCode( int errorCode )
{
	if( errorCode != SQLITE_OK && errorCode != SQLITE_DONE )
	{
		CORAL_THROW( co::Exception, "SQLite statement error "
			<< sqlite3_errmsg( sqlite3_db_handle( _stmt ) ) );
	}
}

/************************************************************************/
/* SQLiteConnection                                                     */
/************************************************************************/

SQLiteConnection::SQLiteConnection()
{
	_db = 0;
}

SQLiteConnection::~SQLiteConnection()
{
	close();
}

void SQLiteConnection::open( const std::string& fileName )
{
	if( isConnected() )
		throw co::Exception( "Open database failed. Database already opened" );

	if( sqlite3_open( fileName.c_str(), &_db ) != SQLITE_OK )
		throw co::Exception( "Open database failed" );

	fh::ISQLiteStatementRef stmt = prepare( "PRAGMA foreign_keys = ON" );

	stmt->execute();
}

void SQLiteConnection::close()
{
	if( !_db )
		return;

	if( sqlite3_close( _db ) != SQLITE_OK )
		throw co::Exception( "Could not close database. Check for unfinalized SQLiteResults" );

	_db = 0;
}

ISQLiteStatement* SQLiteConnection::prepare( const std::string& sql )
{
	if( !_db )
		throw co::Exception( "Database not connected. Cannot execute command" );

	sqlite3_stmt* stmt;

	int resultCode = sqlite3_prepare_v2( _db, sql.c_str(), -1, &stmt, 0 );
	if( resultCode != SQLITE_OK )
	{
		CORAL_THROW( co::Exception, "Query Failed: " << sqlite3_errmsg( _db ) );
	}

	return new SQLiteStatement( stmt );
}

void SQLiteConnection::checkConnection()
{
	if( !isConnected() )
		throw co::Exception( "Database not connected. Cannot execute command" );
}

}

