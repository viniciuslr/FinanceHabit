local CSVTransactionStore = co.Component( "fh.CSVTransactionStore" )

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

function CSVTransactionStore:open( fileName )
	local file = io.open( fileName, "a" )
	local headers = "Date,Amount,Category,Subcategory,Payment Method,Description,Ref/Check No,Payee/Payer,Status,Receipt Picture,Account"
	if not file then
		io.open( fileName, "w" )
		io.write( headers )
	end
	self.file = file
	self.record = {}
	self.headers = {}
	for value in headers:gmatch("[^,]*") do -- note: doesn\'t work with strings that contain , values
	  self.headers[#self.headers+1] = value
    end
end

-- Date,Amount,Category,Subcategory,Payment Method,Description,Ref/Check No,Payee/Payer,Status,Receipt Picture,Account

function CSVTransactionStore:getNextTransaction()
	local row = extractNextCsvRow( self.file )
	local currentTransaction = nil
	currentTransaction = co.new( "fh.ITransaction" )
	if row then
		for i, value in ipairs( row ) do
			self.record[ self.headers[i] ] = value
		end
		currentTransaction.date = convertDate( self.record["Date"] )
		currentTransaction.identifier = self.record[ "Payee/Payer" ]
		currentTransaction.value = self.record[ "Amount" ]
		currentTransaction.record = self.record[ "Payment Method" ]
		currentTransaction.category = self.record[ "Category" ] .. ":" .. self.record[ "Subcategory" ]
	end
	return row ~= nil, currentTransaction
end

function CSVTransactionStore:insertTransaction( transaction )
	
	for i, value in ipairs( self.headers ) do
		self.record[ value ] = ""
	end
	
	self.record["Date"] = transaction.date
	self.record[ "Payee/Payer" ] = transaction.identifier
	self.record[ "Amount" ] = transaction.value
	self.record[ "Payment Method" ] = transaction.record or "Cash"
	self.record[ "Category" ] = transaction.category or "Uncategorized"
	
	for i, value in ipairs( self.headers ) do
		self.file:write( self.record[ value ] .. "," )
	end
	self.file:write( "\n" )
end
	
return CSVTransactionStore