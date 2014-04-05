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



#include <conio.h>
#include <stdlib.h>
#include <math.h>

#define LUA_LIB

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "profilualib.h"
#include "profilua.h"



//**************************************************************************************
//*** getkey
//***
//***     !DO NOT USE THIS!
//***     It won't work after the console was closed and then reopened.
//***
//*** returns:
//***  keycode - if a key was pressed
//***  nil     - if no key was pressed
//**************************************************************************************
static int plGetkey(lua_State *L)
{
	if(_kbhit())
		lua_pushinteger(L,getch());
	else
		lua_pushnil(L);

	return 1;
}




static const luaL_Reg profilua[] =
{
  {"GetKey",		plGetkey},
  {NULL, NULL}
};

//**************************************************************************************
//*** luaopen_profilua
//***
//***
//**************************************************************************************
LUALIB_API int luaopen_profilua (lua_State *L)
{
//	int i;
//	char tmp[2];
  luaL_register(L, LUA_PROFILUALIBNAME, profilua);

	// add a variable for ProfiLua version
	lua_pushstring(L,PROFILUAVERSION);
	lua_setglobal(L,"PROFILUAVERSION");

	// add a variable for logic low and high outputs
	lua_pushnumber(L,LOGIC_HIGH);
	lua_setglobal(L,"gHIGH");
	lua_pushnumber(L,LOGIC_LOW);
	lua_setglobal(L,"gLOW");

	// add a variable for logic threshold
	lua_pushnumber(L,LOGIC_HIGH_THRES);
	lua_setglobal(L,"gISHIGH");


  return 1;
}
