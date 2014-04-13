local TransactionStore = co.Component( "fh.SQLiteTransactionStore" )

local getMappingString = "SELECT category FROM category_mapping WHERE identifier = ?"
local selectTransactionString = "SELECT * FROM transaction_record"

local insertTransactionString = "INSERT INTO transaction_record VALUES ( ?, ?, ?, ?, ? )"
local insertMappingString = "INSERT OR IGNORE INTO category_mapping VALUES ( ?, ? )"

local tableMappingString = "CREATE TABLE IF NOT EXISTS [category_mapping]  ( \
								[identifier] VARCHAR(40) NOT NULL, \
								[category] VARCHAR(50) NOT NULL, \
								PRIMARY KEY (identifier, category) ) \
							"
							
local tableTransactionString = "CREATE TABLE IF NOT EXISTS [transaction_record]  (\
								[date] DATE DEFAULT 'CURRENT_TIME' NOT NULL,\
								[identifier] VARCHAR(40)  NOT NULL,\
								[value] FLOAT  NOT NULL,\
								[record] VARCHAR(30) DEFAULT 'CASH' NOT NULL,\
								[category] VARCHAR(50) DEFAULT 'UNCATEGORIZED' NULL\
							)"
							

function findpattern(text, pattern, start)
	return string.sub(text, string.find(text, pattern, start))
end

function convertDate( dateText )
	local i, j, day, month, year = string.find( dateText, "(.*)/(.*)/(.*)", 1 )
	return year .. "-" .. month .. "-" .. day
end

local function extractNextCsvRow( file )
	local currentLineFn = file:lines()
	local currentLine = currentLineFn()
	if not currentLine then
		return nil
	end

	local row = {}
	for value in currentLine:gmatch("[^,]*") do -- note: doesn\'t work with strings that contain , values
	  row[#row+1] = value
    end
	return row
end

function TransactionStore:open( fileName )
	self.conn = co.new("fh.SQLiteConnection").connection
	self.conn:open( fileName )
	
	assert( self.conn:isConnected() )
	
	local stmt = self.conn:prepare( tableMappingString )
	stmt:execute()
	stmt:finalize()
	
	stmt = self.conn:prepare( tableTransactionString )
	stmt:execute()
	stmt:finalize()
	
	self.selectTransactionStmt = self.conn:prepare( selectTransactionString )
	self.resultTransaction = self.selectTransactionStmt:query()
end

function TransactionStore:getNextTransaction()
	
	local hasNext = self.resultTransaction:next()
	local transaction = co.new "fh.ITransaction"
	if hasNext then
		transaction.date = self.resultTransaction:getString( 0 )
		transaction.identifier = self.resultTransaction:getString( 1 )
		transaction.value = self.resultTransaction:getDouble( 2 )
		transaction.record = self.resultTransaction:getString( 3 )
		transaction.category = self.resultTransaction:getString( 4 )
	end
	return hasNext, transaction
end

function TransactionStore:getMappedCategory( identifier )
	
	local stmt = self.conn:prepare( getMappingString )
	stmt:bindString( 1, identifier )
	
	local rs = stmt:query()
	
	if rs:next() then
		return rs:getString( 1 )
	end
	stmt:finalize()
	return nil
end

function TransactionStore:insertMappedCategory( identifier, category )
	if self.mappingStmt == nil then
		self.mappingStmt = self.conn:prepare( insertMappingString )
	end
	
	self.mappingStmt:bindString( 1, identifier )
	self.mappingStmt:bindString( 2, category )
	
	self.mappingStmt:execute()
	self.mappingStmt:reset()
	print( "Mapping inserted", identifier, category )
	
end
	
function TransactionStore:insertTransaction( transaction )
	
	local transactionDate = transaction.date
	local identifier = transaction.identifier
	local value = transaction.value
	local record = transaction.record
	if record == "" then
		record = "CASH"
	end
	
	local category = transaction.category
	if category == "" then
		category = self:getMappedCategory( identifier ) or "UNCATEGORIZED"
	end
	
	if self.insertTransactionStmt == nil then
		self.insertTransactionStmt = self.conn:prepare( insertTransactionString )
	end

	self.insertTransactionStmt:bindDate( 1, transactionDate )
	self.insertTransactionStmt:bindString( 2, identifier )
	self.insertTransactionStmt:bindDouble( 3, value )
	self.insertTransactionStmt:bindString( 4, record )
	self.insertTransactionStmt:bindString( 5, category )
	
	self.insertTransactionStmt:execute()
	self.insertTransactionStmt:reset()
	print( "Record inserted", record, transactionDate, identifier, value, category )
	
end

return TransactionStore