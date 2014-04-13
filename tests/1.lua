--
-- ProfiLua
-- Serial Interface Test
--
-- ***********************************
-- * REQUIRES AT LEAST PROFILUA_V10  *
-- ***********************************
--
-- (c) ASkr, www.askrprojects.net
--



-- SET YOUR COM PORT HERE
COMPORT = 6





gNumInputs  = 4
gNumOutputs = 3

InputNames  = {"NumTrig","StringTrig","TableTrig","ReadTrig"}
OutputNames = {"PortOpen","Count","O3"}



portMountedAndOpen = 0  -- 0 or 5 (for Profilab digital pin state compatibility)
portHandle = 0



------------------------------------------------------------------------------
function gInputName(channel)
	return InputNames[channel]
end

------------------------------------------------------------------------------
function gOutputName(channel)
	return OutputNames[channel]
end

------------------------------------------------------------------------------
function gSimStart(In)

	print("using COM port " .. COMPORT )

	portHandle = proserial.Mount()
	print( "portHandle: " .. portHandle )
	
	if portHandle > 0 then
		if proserial.Open( portHandle, COMPORT ) then
			portMountedAndOpen = 5 -- "5V" for a Profilab digital "HIGH" state
			print("COM port open")
		else
			print("ERROR opening COM port " .. COMPORT )
		end
	end

	
	return {0,0,0}
end

------------------------------------------------------------------------------
function gSimStop(In)
	if portMountedAndOpen then
		proserial.Close( portHandle )
		proserial.UnMount( portHandle )
		portHandle = 0
		portMountedAndOpen = 0
	end
end



butTrigNumber = 0
butTrigString = 0
butTrigTable  = 0
butTrigRead   = 0


------------------------------------------------------------------------------
-- Check if a button was pressed (once)
------------------------------------------------------------------------------
function CheckButtonPressed( button, butTrigName )

	if button > 0 then
		if _G[butTrigName] == 0 then
			_G[butTrigName] = 1
			return 1
		end
	else
		_G[butTrigName] = 0
	end
	
	return 0
end

------------------------------------------------------------------------------
-- gCalculate
--
-- 
------------------------------------------------------------------------------
function gCalculate(In)
  -- on top for better visibility
	local randLetter
	local randString
	local randTable
	local ch

	-- send number button
	if CheckButtonPressed( In[1], 'butTrigNumber' ) == 1 then
		randLetter = 64 + math.random(26)
		if proserial.Send( portHandle, randLetter ) then
			print("SENT " .. randLetter .. " -> " .. string.char(randLetter) )
		else
			print("ERROR sending " .. randLetter )
		end
	end

	-- send string button
	if CheckButtonPressed( In[2], 'butTrigString' ) == 1 then
		randString = '*burp*'
		if proserial.Send( portHandle, randString ) then
			print("SENT " .. randString )
		else
			print("ERROR sending " .. randString )
		end
	end

	-- send table button
	if CheckButtonPressed( In[3], 'butTrigTable' ) == 1 then
		randTable = { 65, 66, 67, 'a', 'b', 'c', "*burp*" }
		if proserial.Send( portHandle, randTable ) then
			io.write("SENT ")
			local i,v
			for i,v in pairs( randTable ) do
				io.write( v )
			end
		else
			print("ERROR sending the table")
		end
	end

	-- read button
	if CheckButtonPressed( In[4], 'butTrigRead' ) == 1 then
		print("RECEIVED ")
		while 1 do
			ch = proserial.ReadByte( portHandle )
			if ch < 0 then
				break
			else
				print("  " .. ch .. "  " .. string.char(ch) )
			end
		end
	end
	
	
	return { portMountedAndOpen, proserial.BufferCount( portHandle ), 0 }
end

