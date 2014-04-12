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

#include "asSerialCollectCWrap.h"

#define LUA_LIB

#include "lua.h"
#include "lauxlib.h"



//**************************************************************************************
//*** plSerialMount
//***
//***
//*** LUA STACK IN:
//***  none
//*** LUA STACK OUT:
//***  number   : handle of serial object ( 0 == error; >0 handle of serial port)
//**************************************************************************************
static int plSerialMount( lua_State *L )
{
	int ret = 0;

	if( lua_gettop(L) == 0 )
		ret = Mount();

	lua_pushnumber( L, ret );

	return 1;
}



//**************************************************************************************
//*** plSerialUnMount
//***
//***
//*** LUA STACK IN:
//***  L1: handle
//*** LUA STACK OUT:
//***  number   : 0  ->  error
//***             1  ->  success
//**************************************************************************************
static int plSerialUnMount( lua_State *L )
{
	int ret = 0;

	if( lua_gettop(L) == 1 )
		ret = UnMount( luaL_checkinteger( L, 1 ) );

	lua_pushnumber( L, ret );

	return 1;
}



//**************************************************************************************
//*** plSerialCheck
//***
//***
//*** LUA STACK IN:
//***  L1 -> handle
//***  L2 -> port
//*** LUA STACK OUT:
//***  number   : 0  ->  port not available
//***             1  ->  port available and free
//**************************************************************************************
static int plSerialCheck( lua_State *L )
{
	if( lua_gettop(L) != 2 )
	{
		lua_pushnil(L);
		return 1;
	}

	lua_pushnumber( L, Check( luaL_checkinteger(L,1), luaL_checkinteger(L,2) ) );

	return 1;
}



//**************************************************************************************
//*** plSerialOpen
//***
//***
//*** LUA STACK IN:
//***  L1 -> handle
//***  L2 -> port
//*** LUA STACK OUT:
//***  number   : 0  ->  ERROR
//***             1  ->  OK
//**************************************************************************************
static int plSerialOpen( lua_State *L )
{
	int ret = 0;

	if( lua_gettop(L) == 2 )
		ret = Open( luaL_checkinteger(L,1), luaL_checkinteger(L,2) );

	lua_pushnumber( L, ret );

	return 1;
}



//**************************************************************************************
//*** plSerialClose
//***
//***
//*** LUA STACK IN:
//***  L1 -> handle
//*** LUA STACK OUT:
//***  number   : 0  ->  ERROR
//***             1  ->  OK
//**************************************************************************************
static int plSerialClose( lua_State *L )
{
	int ret = 0;

	if( lua_gettop(L) == 1 )
		ret = Close( luaL_checkinteger(L,1) );

	lua_pushnumber( L, ret );

	return 1;
}



//**************************************************************************************
//*** plSerialSendByte
//***
//***
//*** LUA STACK IN:
//***  L1 -> handle
//***  L2 -> byte to send
//*** LUA STACK OUT:
//***  number   : 0  ->  ERROR
//***             1  ->  OK
//**************************************************************************************
static int plSerialSendByte( lua_State *L )
{
	int ret = 0;

	if( lua_gettop(L) == 2 )
		ret = SendByte( luaL_checkinteger(L,1), luaL_checkinteger(L,2) );

	lua_pushnumber( L, ret );

	return 1;
}



//**************************************************************************************
//*** plSerialBufferCount
//***
//***
//*** LUA STACK IN:
//***  L1 -> handle
//*** LUA STACK OUT:
//***  number   :  <0  ->  ERROR (OVERFLOW or WHATEVER)
//***             >=0  ->  number of bytes in the receive buffer
//**************************************************************************************
static int plSerialBufferCount( lua_State *L )
{
	int ret = -1;

	if( lua_gettop(L) == 1 )
		ret = BufferCount( luaL_checkinteger(L,1) );

	lua_pushnumber( L, ret );

	return 1;
}



//**************************************************************************************
//*** plSerialConfig
//***
//***
//*** LUA STACK IN:
//***  L1 -> handle
//***  L2 -> baud rate
//***  L3 -> bits
//***  L4 -> parity
//***  L5 -> stop
//*** LUA STACK OUT:
//***  number   :  0  ->  ERROR
//***              1  ->  OK
//**************************************************************************************
static int plSerialConfig( lua_State *L )
{
	int ret = 0;

	if( lua_gettop(L) == 5 )
		ret = Config( luaL_checkinteger(L,1),
									luaL_checkinteger(L,2),
									luaL_checkinteger(L,3),
									luaL_checkinteger(L,4),
									luaL_checkinteger(L,5) );

	lua_pushnumber( L, ret );

	return 1;
}



//**************************************************************************************
//*** plSerialReadByte
//***
//***
//*** LUA STACK IN:
//***  L1 -> handle
//*** LUA STACK OUT:
//***  number   : <0  ->  ERROR
//***            >=0  ->  byte
//**************************************************************************************
static int plSerialReadByte( lua_State *L )
{
	int ret = -1;

	if( lua_gettop(L) == 1 )
		ret = ReadByte( luaL_checkinteger(L,1) );

	lua_pushnumber( L, ret );

	return 1;
}



//**************************************************************************************
//*** plSerialBufferFlush
//***
//***
//*** LUA STACK IN:
//***  L1 -> handle
//*** LUA STACK OUT:
//***  number   : 0  ->  ERROR
//***             1  ->  OK
//**************************************************************************************
static int plSerialBufferFlush( lua_State *L )
{
	int ret = 0;

	if( lua_gettop(L) == 1 )
		ret = BufferFlush( luaL_checkinteger(L,1) );

	lua_pushnumber( L, ret );

	return 1;
}




static const struct luaL_Reg serial_funcs[] = {
  { "Mount",						plSerialMount },
  { "UnMount",					plSerialUnMount },
  { "CheckPort",				plSerialCheck },
  { "Open",							plSerialOpen },
  { "Close",						plSerialClose },
  { "SendByte",					plSerialSendByte },
  { "BufferCount",			plSerialBufferCount },
  { "Config",						plSerialConfig },
  { "ReadByte",					plSerialReadByte },
  { "BufferFlush",			plSerialBufferFlush },


  { NULL, NULL }
};

//**************************************************************************************
//*** luaopen_profiserial
//***
//***
//**************************************************************************************
LUALIB_API int luaopen_profiserial( lua_State *L )
{
  luaL_register( L, "proserial", serial_funcs );

	lua_pushinteger( L, 0 );
	lua_setglobal( L, "PARITY_NONE" );

	lua_pushinteger( L, 1 );
	lua_setglobal( L, "PARITY_ODD" );

	lua_pushinteger( L, 2 );
	lua_setglobal( L, "PARITY_EVEN" );

  return 1;
}

