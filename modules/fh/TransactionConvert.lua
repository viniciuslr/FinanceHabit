local TransactionConvert = co.Component( "fh.TransactionConvert" )

function TransactionConvert:convert( inputFile, outputFile )
	
	assert( inputFile )
	assert( outputFile )
	
	local csvStore = co.new( "fh.CSVTransactionStore" ).store
	csvStore:open( inputFile )
	
	local dbStore = co.new( "fh.SQLiteTransactionStore" ).store
	dbStore:open( outputFile )
	
	self:convertFromStore( csvStore, dbStore )
end

function TransactionConvert:convertFromStore( inputStore, outputStore )
	while true do
		local hasNext, readTransaction = inputStore:getNextTransaction()
		print( readTransaction )
		if not hasNext then
			break
		end
		
		outputStore:insertTransaction( readTransaction )
	end
end

return TransactionConvert