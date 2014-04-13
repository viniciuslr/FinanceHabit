#include <fh/ISQLiteConnection.h>
#include <fh/ISQLiteStatement.h>
#include <fh/ISQLiteResult.h>
#include <gtest/gtest.h>
#include <co/Coral.h>
#include <co/IObject.h>
#include <co/reserved/OS.h>
#include <cstdio>
#include <co/Exception.h>

namespace fh{

TEST( SQLiteConnectionTests, testOpenNonExistingDB )
{
	std::string fileName = "connTest.db";

	co::IObject* connObj = co::newInstance( "fh.SQLiteConnection" );

	fh::ISQLiteConnection* sqliteDBConn = connObj->getService<fh::ISQLiteConnection>();

	ASSERT_NO_THROW( sqliteDBConn->open( fileName ) );

	sqliteDBConn->close();

	EXPECT_TRUE( co::OS::isFile(fileName) );
	ASSERT_FALSE( remove( fileName.c_str() ) );

}

TEST( SQLiteConnectionTests, testExecuteWithoutOpen )
{
	std::string fileName = "connTest.db";

	co::IObject* connObj = co::newInstance( "fh.SQLiteConnection" );

	fh::ISQLiteConnection* sqliteDBConn = connObj->getService<fh::ISQLiteConnection>();

	EXPECT_THROW( sqliteDBConn->prepare("CREATE TABLE A (fieldX INTEGER)")->execute(), co::Exception );
	EXPECT_THROW( sqliteDBConn->prepare("SELECT * FROM A")->query(), co::Exception );
}

TEST( SQLiteConnectionTests, testCreateDatabase )
{
	std::string fileName = "connTest.db";

	co::IObject* connObj = co::newInstance( "fh.SQLiteConnection" );

	fh::ISQLiteConnection* sqliteDBConn = connObj->getService<fh::ISQLiteConnection>();

	EXPECT_NO_THROW( sqliteDBConn->open( fileName ) );
	
	EXPECT_TRUE( co::OS::isFile(fileName) );
	
	//attempt to create again the database
	EXPECT_THROW( sqliteDBConn->open( fileName ), co::Exception );
	
	sqliteDBConn->close();
	ASSERT_FALSE( remove( fileName.c_str() ) );
	EXPECT_FALSE( co::OS::isFile(fileName) );
}

TEST( SQLiteConnectionTests, testSuccessfullExecutes )
{
	std::string fileName = "connTest.db";

	co::IObject* connObj = co::newInstance( "fh.SQLiteConnection" );

	fh::ISQLiteConnection* sqliteDBConn = connObj->getService<fh::ISQLiteConnection>();

	EXPECT_NO_THROW( sqliteDBConn->open( fileName ) );

	fh::ISQLiteStatementRef stmt = sqliteDBConn->prepare( "CREATE TABLE A (fieldX INTEGER, fieldY TEXT)" );
	EXPECT_NO_THROW( stmt->execute() );
	
	stmt = sqliteDBConn->prepare( "INSERT INTO A VALUES (1, 'text1')" );
	EXPECT_NO_THROW( stmt->execute() );
	
	stmt = sqliteDBConn->prepare( "INSERT INTO A VALUES (2, 'text2')" );
	EXPECT_NO_THROW( stmt->execute() );
	
	stmt = sqliteDBConn->prepare( "INSERT INTO A VALUES (3, 'text3')" );
	EXPECT_NO_THROW( stmt->execute() );
	
	stmt = sqliteDBConn->prepare( "SELECT * FROM A" );
	EXPECT_NO_THROW( stmt->query() );
	stmt->finalize();
	sqliteDBConn->close();

	ASSERT_FALSE( sqliteDBConn->isConnected() );
	ASSERT_FALSE( remove( fileName.c_str() ) );
	EXPECT_FALSE( co::OS::isFile(fileName) );
}

TEST( SQLiteConnectionTests, openExistingDB )
{
	std::string fileName = "connTest.db";

	co::IObject* connObj = co::newInstance( "fh.SQLiteConnection" );

	fh::ISQLiteConnection* sqliteDBConn = connObj->getService<fh::ISQLiteConnection>();
	
	sqliteDBConn->open( fileName );

	EXPECT_TRUE( co::OS::isFile(fileName) );

	EXPECT_NO_THROW( sqliteDBConn->close() );

	EXPECT_NO_THROW( sqliteDBConn->open( fileName ) );
	
	//you cannot open an already opened database
	EXPECT_THROW( sqliteDBConn->open( fileName ), co::Exception );

	EXPECT_NO_THROW( sqliteDBConn->close() );

	//test harmless double close
	EXPECT_NO_THROW( sqliteDBConn->close() );

	ASSERT_FALSE( sqliteDBConn->isConnected() );
	ASSERT_FALSE( remove( fileName.c_str() ) );

}

TEST( SQLiteConnectionTests, closeDBFail )
{
	std::string fileName = "connTest.db";

	co::IObject* connObj = co::newInstance( "fh.SQLiteConnection" );

	fh::ISQLiteConnection* sqliteDBConn = connObj->getService<fh::ISQLiteConnection>();
	
	sqliteDBConn->open( fileName );

	fh::ISQLiteStatementRef stmt;
	stmt = sqliteDBConn->prepare("CREATE TABLE A (fieldX INTEGER, fieldY TEXT)");

	EXPECT_NO_THROW(stmt->execute());
	
	stmt = sqliteDBConn->prepare("INSERT INTO A VALUES (1, 'text1')");
	EXPECT_NO_THROW(stmt->execute());
	stmt->finalize();

	fh::ISQLiteStatement* stmtNakedPtr = sqliteDBConn->prepare( "SELECT * FROM A" );

	//not finalized IResultSet
	EXPECT_THROW(sqliteDBConn->close(), co::Exception);

	delete stmtNakedPtr;

	EXPECT_NO_THROW( sqliteDBConn->close() );

	ASSERT_FALSE( sqliteDBConn->isConnected() );
	ASSERT_FALSE( remove(fileName.c_str()) );

	
}

TEST( SQLiteConnectionTests, preparedStatementTest )
{
	std::string fileName = "testStatement.db";

	co::IObject* connObj = co::newInstance( "fh.SQLiteConnection" );

	fh::ISQLiteConnection* sqliteDBConn = connObj->getService<fh::ISQLiteConnection>();
	
	sqliteDBConn->open( fileName );

	fh::ISQLiteStatementRef stmt = sqliteDBConn->prepare("CREATE TABLE A (fieldX INTEGER, fieldY TEXT)");
	EXPECT_NO_THROW( stmt->execute() );
	stmt = sqliteDBConn->prepare("INSERT INTO A VALUES (?, ?)" );
	std::string str("value");

	EXPECT_NO_THROW( stmt->bindDouble( 1, 142 ));
	EXPECT_NO_THROW( stmt->bindString( 2, str ));
	EXPECT_NO_THROW( stmt->execute() );

	fh::ISQLiteStatementRef stmt2 = sqliteDBConn->prepare("SELECT * FROM A WHERE fieldX =  ?" );
	EXPECT_NO_THROW( stmt2->bindDouble( 1, 142 ) );
	fh::ISQLiteResultRef rs = stmt2->query();

	ASSERT_TRUE( rs->next() );

	EXPECT_EQ( 142, rs->getUint32( 0 ) );
	EXPECT_EQ( "value", rs->getString( 1 ) );

	// unfinalized IResultSet
	EXPECT_THROW( sqliteDBConn->close(), co::Exception );

	stmt->finalize();
	stmt2->finalize();

	EXPECT_NO_THROW( sqliteDBConn->close() );
	ASSERT_FALSE( sqliteDBConn->isConnected() );
	ASSERT_FALSE( remove(fileName.c_str()) );
}

}