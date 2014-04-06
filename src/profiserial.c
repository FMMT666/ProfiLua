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


#include <windows.h>
#include <stdint.h>
#include <mem.h>

#define LUA_LIB

#include "lua.h"
#include "lauxlib.h"


//**************************************************************************************
//*** plSerialModuleTest
//***
//*** Returns string to test the new serial module...
//*** LUA STACK IN:
//***  L1 -> number of device
//*** LUA STACK OUT:
//***  "string" : name of device
//***  nil      : ERROR
//**************************************************************************************
static int plSerialModuleTest( lua_State *L )
{
	if( lua_gettop(L) != 0 )
	{
		lua_pushnil(L);
		return 1;
	}

	lua_pushstring( L, "HERE'S THE SERIAL MODULE!" );

	return 1;
}




static const struct luaL_Reg serial_funcs[] = {
  { "GetTestString",		plSerialModuleTest },
  { NULL, NULL }
};

//**************************************************************************************
//*** luaopen_profiserial
//***
//***
//**************************************************************************************
LUALIB_API int luaopen_profiserial( lua_State *L )
{
  luaL_register(L, "proserial", serial_funcs);

  return 1;
}

