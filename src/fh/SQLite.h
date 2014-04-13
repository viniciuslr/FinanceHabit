/*
 * Calcium - Domain Model Framework
 * See copyright notice in LICENSE.md
 */

#ifndef _CA_SQLITE_H_
#define _CA_SQLITE_H_

#include <co/Log.h>
#include <co/Exception.h>

#include "SQLiteStatement_Base.h"
#include "SQLiteResult_Base.h"
#include "SQLiteConnection_Base.h"

// Forward Declarations:
extern "C"
{
	typedef struct sqlite3 sqlite3;
	typedef struct sqlite3_stmt sqlite3_stmt;
}

namespace fh {

/*!
	Class to iterate through SQLite query results (a list of rows).
	No data is available until the first call to next().
 */
class SQLiteResult : public SQLiteResult_Base
{
public:
	SQLiteResult(){}
	SQLiteResult( sqlite3_stmt* stmt );
	SQLiteResult( const SQLiteResult& o );

	/*!
		Moves to the next row in the result set.
		\return true if the next row was fetched, false if there are no more rows.
	*/
	bool next();

	/*!
		Use this instead of next() when the expected result is a single row.
		\throw IOException if the query did not result in a single row.
	 */
	void fetchRow();

	//! Retrieves the value at \a column as a string.
	std::string getString( co::uint32 column );

	//! Retrieves the value at \a column as an uint32.
	co::uint32 getUint32( co::uint32 column );

	//! Retrieves the value at \a column as an uint32.
	double getDouble( co::uint32 column );


private:
	// forbid assignments
	SQLiteResult& operator=( const SQLiteResult& o );

	inline bool hasData( int column );

private:
	mutable sqlite3_stmt* _stmt;
};

/*!
	\brief Abstraction for a SQLite Prepared Statement.
	Values are assigned to parameters using the bind() methods.
 */
class SQLiteStatement : public SQLiteStatement_Base
{
public:
	SQLiteStatement(){}
	SQLiteStatement( sqlite3_stmt* stmt );
	SQLiteStatement( const SQLiteStatement& o);
	~SQLiteStatement();

	//! Bind an int32.
	void bind( co::uint32 index, co::int32 value );

	//! Bind an uint32.
	inline void bind( co::uint32 index, co::uint32 value )
	{
		assert( value <= co::MAX_INT32 );
		bind( index, static_cast<co::int32>( value ) );
	}

	//! Bind a double.
	void bindDouble( co::uint32 index, double value );

	//! Bind a literal/C-string.
	void bind( co::uint32 index, const char* value );

	//! Bind a literal/C-string.
	void bind( co::uint32 index, const char* value, int bytes );

	//! Bind a string.
	inline void bindString( co::uint32 index, const std::string& value )
	{
		bind( index, value.data(), value.length() );
	}

	inline void bindDate( co::uint32 index, const std::string& value )
	{
		bindString( index, value );
	}

	/*!
		Executes a SELECT statement.
		This statement must be valid as long as the SQLiteResult is being consulted.
	 */
	ISQLiteResult* query();

	/*!
		Executes a non-SELECT statement.
	 */
	void execute();

	//! Resets the statement so it can be reused. Parameters must be bound again.
	void reset();

	/*!
		Releases the statement. Called automatically when the object dies.
		Unfinalized statements prevent their SQLiteConnection from being closed.
	 */
	void finalize();

private:
	// forbid assignments
	SQLiteStatement& operator=( const SQLiteStatement& o );

	void handleErrorCode( int errorCode );

private:
	mutable sqlite3_stmt* _stmt;
};

/*!
	Connection to a SQLite database file.
 */
class SQLiteConnection : public SQLiteConnection_Base
{
public:
	SQLiteConnection();
	~SQLiteConnection();

	inline bool isConnected() { return _db != NULL; }

	/*!
		Opens a connection to the database with the given \a fileName.
		If the database does not exist yet, it is created.
	 */
	void open( const std::string& fileName );

	fh::ISQLiteStatement* prepare( const std::string& sql );

	void close();

private:
	void checkConnection();

private:
	sqlite3* _db;
};

CORAL_EXPORT_COMPONENT( SQLiteResult, SQLiteResult );
CORAL_EXPORT_COMPONENT( SQLiteStatement, SQLiteStatement );
CORAL_EXPORT_COMPONENT( SQLiteConnection, SQLiteConnection );

}

#endif // _CA_SQLITE_H_
