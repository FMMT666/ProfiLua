--
-- ProfiLua
-- Saving gPUser values
--
-- ***********************************
-- * REQUIRES AT LEAST PROFILUA_V10  *
-- ***********************************
--
-- (c) ASkr, www.askrprojects.net
--

gNumInputs  = 3
gNumOutputs = 3

InputNames  = {"I1","I2","I3"}
OutputNames = {"O1","O2","O3"}

function gInputName(channel)
	return InputNames[channel]
end

function gOutputName(channel)
	return OutputNames[channel]
end




-- start with "old" values (stored during last shutdown phase)
function gSimStart(In)
	return {gPUser[1],gPUser[2],gPUser[3]}
end


-- Save gPUser values on exit.
function gSimStop(In)
	gPUser[1] = In[1]
	gPUser[2] = In[2]
	gPUser[3] = In[3]
end




CHECK_COMPORT = 6

state = 0
serial = 0



------------------------------------------------------------------------------
-- gCalculate
--
-- Just slam out gPUser[1..3]
------------------------------------------------------------------------------
function gCalculate(In)

  -----
	if state == 0 then
		print( proserial.GetTestString() )
		state = state + 1
  -----
	elseif state == 1 then
		serial = proserial.Mount()
		print( "Mount(): " .. serial )
		state = state + 1
  -----
	elseif state == 2 then
		print("Check(): " .. proserial.Check(serial, CHECK_COMPORT) )
		state = state + 1
  -----
	elseif state == 3 then
		print("3")
		state = state + 1
	end

	
	return {gPUser[1],gPUser[2],gPUser[3]}
end

