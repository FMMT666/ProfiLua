//
// ProfiLua
//
// Lua for Profilab
//
// (c) ASkr, www.askrprojects.net
//
//
//
// Copyright © 2010, 2014 ASkr, www.askrprojects.net
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in the
// Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies
// or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
// FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.
//



#include "profilua.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "proficonsole.h"


#define PL_ERROR LOGIC_HIGH
#define PL_OK    LOGIC_LOW




// NOTE #1:
//   This does no contain many Lua sanity checks!
//   Be careful! ;-)



lua_State *L      = NULL;

uchar gNumInputs  = 0;
uchar gNumOutputs = 0;
uchar gERROR      = 0;
uchar gFileNum    = 0;
uchar gConsole    = 0;
uchar gConsoleTop = 0;
uchar gInStrMarker[100];
uchar gOutStrMarker[100];


// In version 13.10.2010 (d/m/y), or earlier, CSimStart has a bug:
// PInput array is completely set to "0.0".
// Its useless...
// This requires all the initialization to be done in CCalculate
// ONCE, after it started. This is the execution marker...
uchar gCSimStartOverride = 0;
void CSimStartFlaw(double *PInput, double *POutput, double *PUser);




//****************************************************************************
//*** OpenLuaScript
//***
//*** Initializes the Lua stack and opens the script file.
//*** Name of the script to execute is determined by tha number of the
//*** DLL inside Profilab. E.g.:
//***   DLL #1 -> "1.lua"
//***   DLL #2 -> "2.lua"
//***   ...
//****************************************************************************
uchar OpenLuaScript()
{
	uchar fname[300];

	if(L == NULL)
	{
		L = lua_open();
		if(L == NULL)
			return 0;
		luaL_openlibs(L);
	}

	sprintf((uchar *)&fname,"%d.lua",gFileNum);
	luaL_loadfile(L,fname);
	lua_pcall(L, 0, LUA_MULTRET, 0);

	return 1;
}



//****************************************************************************
//*** CloseLuaScript
//***
//***
//****************************************************************************
void CloseLuaScript()
{
	if( L!=NULL )
	{
		lua_close(L);
		L = NULL;
	}
}


//****************************************************************************
//*** CNumInputEx
//***
//*** Sets global file number 1..9, representing the number of the DLL
//*** inside Profilab. This file number is used in "OpenLuaScript()".
//*** Additionally, this one reads the number of inputs from the
//*** Lua environment.
//****************************************************************************
uchar _stdcall CNumInputsEx(double *PUser)
{
	gFileNum = (uchar)PUser[100];

	if( OpenLuaScript() )
	{
		lua_getglobal(L, "gNumInputs");
		if( !lua_isnil(L, -1) )
			gNumInputs = (uchar)lua_tonumber(L, -1) + 1;  // extra CONSOLE pin

		CloseLuaScript();
	}
	else
		gERROR = 1;

	return gNumInputs;
}



//****************************************************************************
//*** CNumOutputsEx
//***
//***
//****************************************************************************
uchar _stdcall CNumOutputsEx(double *PUser)
{
	gFileNum = (uchar)PUser[100];

	if( OpenLuaScript() )
	{
		lua_getglobal(L, "gNumOutputs");
		if( !lua_isnil(L, -1) )
			gNumOutputs = (uchar)lua_tonumber(L, -1) + 1; // extra ERROR pin

		CloseLuaScript();
	}
	else
		gERROR = 1;

	return gNumOutputs;
}



//****************************************************************************
//*** GetInputName
//***
//*** Reads input names from Lua environment (function "gInputName()").
//*** If anything goes wrong, input names are set to "!ERROR!".
//****************************************************************************
void _stdcall GetInputName(uchar Channel, uchar *Name)
{

	Name = strcpy(Name,"!ERROR!");

	if(Channel == 0)
		strcpy(Name,"CONSOLE");
	else
	{
		if( OpenLuaScript() )
		{
			lua_pushstring(L,"gInputName");
			lua_gettable(L, LUA_GLOBALSINDEX);

			if(lua_isfunction(L,-1))
			{
				lua_pushinteger(L,Channel);
				lua_call(L, 1, 1);

				if (lua_isstring(L, -1))
				{
					strcpy(Name,(uchar *)lua_tostring(L, -1));

					if( *Name == '$' )
						gInStrMarker[Channel] = 1;
				}

				lua_pop(L, 1);
			}

			CloseLuaScript();
		}
	}

}


//****************************************************************************
//*** GetOutputName
//***
//*** Reads output names from Lua environment (function "gOutputName()").
//*** If anything goes wrong, all output names are set to "!ERROR!".
//****************************************************************************
void _stdcall GetOutputName(unsigned char Channel, unsigned char *Name)
{

	Name = strcpy(Name,"!ERROR!");

	if( Channel == 0)
		strcpy(Name,"ERROR");
	else
	{
		if( OpenLuaScript() )
		{
			lua_pushstring(L,"gOutputName");
			lua_gettable(L, LUA_GLOBALSINDEX);

			if(lua_isfunction(L,-1))
			{
				lua_pushinteger(L,Channel);
				lua_call(L, 1, 1);

				if (lua_isstring(L, -1))
				{
					strcpy(Name,(uchar *)lua_tostring(L, -1));
					if ( *Name == '$' )
						gOutStrMarker[Channel] = 1;
				}

				lua_pop(L, 1);
			}

			CloseLuaScript();
		}// if OpenLuaScript
	}// else

}



//****************************************************************************
//*** CCalculateEx
//***
//*** This one passes all "PInput" inputs to the Lua function "gCalculate" and
//*** stores its results back in "POutput".
//*** Note: "PUser" is not fed forward from here, but available through
//*** the global Lua table <gPUser>.
//****************************************************************************
void _stdcall CCalculateEx(double *PInput, double *POutput, double *PUser, uchar **PString)
{
	int i;
//	size_t len;

	if(gCSimStartOverride)
	{
		CSimStartFlaw(PInput, POutput, PUser);
		gCSimStartOverride = 0;
	}


	// CSimStart flaw hack
	if( ( PInput[0] > LOGIC_HIGH_THRES ) && ( gConsole == 0) )
	{
		DebugConsoleOn();
		gConsole = 1;
		gConsoleTop = 0;
		DebugConsoleTop();
	}

	if(( gConsole == 1) && (gConsoleTop == 0) )
	{
		DebugConsoleTop();
		gConsoleTop++;
	}


	// CCalculateEX starts here...

	if( gERROR )
	{
		POutput[0] = PL_ERROR;
		return;
	}


	lua_pushstring(L,"gCalculate");
	lua_gettable(L, LUA_GLOBALSINDEX);

	if(lua_isfunction(L,-1))
	{
		// put arguments (table) onto stack
		lua_createtable(L,gNumInputs - 1,0);
		for(i=1;i<gNumInputs;i++)
		{
			lua_pushnumber(L,i);
			if( gInStrMarker[i] )
				lua_pushstring(L,PString[i]);
			else
				lua_pushnumber(L,PInput[i]);
	  	lua_settable(L,-3);
	  }

		// leaves exactly one table on stack
		lua_call(L,1,1);

		// read back results from table
		if (lua_istable(L, -1))
		{
			for(i=1;i<gNumOutputs;i++)
			{
				lua_pushnumber(L,i);
				lua_gettable(L,-2);

				if( gOutStrMarker[i] )
				{
					if( lua_isstring(L,-1) )
						strcpy(PString[i],(uchar *)lua_tostring(L,-1));
						// future upgrade:
						// ignore zeros
//							strncpy(PString[i],((uchar *)lua_tolstring (L,-1,&len)),len);

					else
					{
						*PString[i] = 0;
						lua_pop(L,1);
						gERROR = 1;
					}
				}
				else
				{
					if( lua_isnumber(L,-1) )
						POutput[i] = lua_tonumber(L,-1);
					else
					{
						POutput[i] = 0;
						lua_pop(L,1);
						gERROR = 1;
					}
				}
				lua_pop(L,1);
			} // end for

			lua_pop(L, 1);

		} // end if istable
		else
			gERROR = 1;


	} // end if isfunction
	else
		gERROR = 1;

}



//****************************************************************************
//*** CSimStartFlaw
//***
//***
//****************************************************************************
void CSimStartFlaw(double *PInput, double *POutput, double *PUser)
{
	int i;
	char dummy[100]; // might overflow!

	gERROR = 1;
	POutput[0] = PL_ERROR;


	if(PInput[0] > LOGIC_HIGH_THRES)
	{
		DebugConsoleOn();
		DebugConsoleTop();
		gConsole = 1;
		gConsoleTop = 0;
	}

	// required for setting string in- and output markers
	for(i=0;i<100;i++)
	{
		gInStrMarker[i]  = 0;
		gOutStrMarker[i] = 0;
	}
	for(i=0;i<gNumInputs;i++)
		GetInputName(i,(char *)&dummy);
	for(i=0;i<gNumOutputs;i++)
		GetOutputName(i,(char *)&dummy);


	if( OpenLuaScript() )
	{
		// create a global <gPUser> variable inside Lua, including PUser[100] (101 elements)
		lua_createtable(L,101,0);
		for(i=0;i<101;i++)
		{
			lua_pushnumber(L,i+1);
			lua_pushnumber(L,PUser[i]);
			lua_settable(L,-3);
		}
		lua_setglobal(L,"gPUser");

		// prepare call of gStartSim()
		lua_pushstring(L,"gSimStart");
		lua_gettable(L, LUA_GLOBALSINDEX);

		if(lua_isfunction(L,-1))
		{
			// put arguments (table) onto stack
			lua_createtable(L,gNumInputs - 1,0);
			for(i=1;i<gNumInputs;i++)
			{
				lua_pushnumber(L,i);
				if( gInStrMarker[i] )
					lua_pushnumber(L,0); // string dummy; no PString available in here...
				else
					lua_pushnumber(L,PInput[i]);
		  	lua_settable(L,-3);
		  }

			// call pStartSim; leaves exactly one table on stack
			lua_call(L,1,1);

			// read back results from table
			if (lua_istable(L, -1))
			{
				for(i=1;i<gNumOutputs;i++)
				{
					lua_pushnumber(L,i);
					lua_gettable(L,-2);

					if( gOutStrMarker[i] )
						; //	lua_pop(L,1);						// **************************************
					else
					{
						if( lua_isnumber(L,-1) )
							POutput[i] = lua_tonumber(L,-1);
						else
						{
							POutput[i] = 0;
							lua_pop(L, 1);
							gERROR = 1;
						}
					}
					lua_pop(L,1);
				} // end for

				lua_pop(L, 1);

			} // end if istable
			else
				gERROR = 1;

			POutput[0] = PL_OK;
			gERROR = 0;

		} // if isfunction
	} // if OpenLua
}




//****************************************************************************
//*** CSimStart
//***
//*** This one calls Lua function "gSimStart" with all input arguments and
//*** stores its results back in "POutput".
//*** "PUser" is available via a function inside Lua, hence it is not
//*** forwarded from here...
//****************************************************************************
void _stdcall CSimStart(double *PInput, double *POutput, double *PUser)
{

	// call init routine in CCalculate ONCE
	gCSimStartOverride = 1;

	// ALL THE OTHER STUFF MOVED TO CSimStartFlaw()

}



//****************************************************************************
//*** CSimStop
//***
//*** Forwards all inputs to Lua function "gSimStop" and stores back the
//*** results in "POutput".
//****************************************************************************
void _stdcall CSimStop(double *PInput, double *POutput, double *PUser)
{
	int i;

	if( !gERROR )
	{
		lua_pushstring(L,"gSimStop");
		lua_gettable(L, LUA_GLOBALSINDEX);

		if(lua_isfunction(L,-1))
		{
			// put arguments (table) onto stack
			lua_createtable(L,gNumInputs - 1,0);
			for(i=1;i<gNumInputs;i++)
			{
				lua_pushnumber(L,i);
				if( gInStrMarker[i] )
					lua_pushnumber(L,0); // string dummy; no PString available in here...
				else
					lua_pushnumber(L,PInput[i]);
		  	lua_settable(L,-3);
		  }

			// no return values here...
			lua_call(L,1,0);
		}
		else
			gERROR = 1;

		// *NEW* v0.9b
		// Read back global gPUser table and store results back in PUser.
		lua_pushstring(L,"gPUser");
		lua_gettable(L, LUA_GLOBALSINDEX);

		if (lua_istable(L, -1))
		{
			// read back 0-99 (1-100 inside ProfiLua); 101 skipped (# of DLL)
			for(i=0;i<100;i++)
			{
				lua_pushnumber(L,i+1);
				lua_gettable(L,-2);

				if( lua_isnumber(L,-1) )
					PUser[i] = lua_tonumber(L,-1);
				else
				{
					PUser[i] = 0;
					lua_pop(L, 1);
				}
				lua_pop(L,1);
			}// end for
			lua_pop(L,1);
		}// end istable

	}// end !gERROR

	CloseLuaScript();

	if( gConsole == 1)
	{
		DebugConsoleOff();
		gConsole = 0;
		gConsoleTop = 0;
	}

}




//****************************************************************************
//***
//***
//*** not used
//****************************************************************************
/*
void _stdcall CConfigure(double *PUser)
{
}
*/




//****************************************************************************
//*** DllMain
//***
//***
//****************************************************************************

BOOL APIENTRY DllMain (HINSTANCE hInst, DWORD reason, LPVOID reserved)
{
	return TRUE;
}


