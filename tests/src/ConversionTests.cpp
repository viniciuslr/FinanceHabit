#include <fh/ITransactionConvert.h>
#include <gtest/gtest.h>
#include <co/Coral.h>
#include <co/IObject.h>
#include <co/reserved/OS.h>
#include <cstdio>
#include <co/Exception.h>

namespace fh{

TEST( ConverstionTests, testOpenNonExistingDB )
{
	std::string fileName = "connTest.db";

	co::IObject* connObj = co::newInstance( "fh.TransactionConvert" );

	fh::ITransactionConvert* convert = connObj->getService<fh::ITransactionConvert>();

	ASSERT_NO_THROW( convert->convert( "expenseManager.csv", fileName ) );

	EXPECT_TRUE( co::OS::isFile(fileName) );
	ASSERT_FALSE( remove( fileName.c_str() ) );

}

}