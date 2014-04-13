local App = co.Component( "fh.App" )

local lfs = require "lfs"

function App:main( args )
	local transactionConvert = co.new("fh.TransactionConvert").tc
	
	local csvStore = co.new( "fh.CSVTransactionStore" ).store
	csvStore:open( "testConvert.csv" )
	
	local dbStore = co.new( "fh.SQLiteTransactionStore" ).store
	dbStore:open( "test2.db" )
	
	transactionConvert:convertFromStore( dbStore, csvStore )
	
	return 0
end