//
// ProfiLua
//
// Lua for Profilab
//
// (c) ASkr, www.askrprojects.net
//
//
//
// Copyright © 2010 ASkr, www.askrprojects.net
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
#include <mmsystem.h>
#include <stdint.h>
#include <mem.h>

#define LUA_LIB

#include "lua.h"
#include "lauxlib.h"


#define MIDIIN	0
#define MIDIOUT	1

// MIDI EVENTS (high nibble)
#define NOTEON			0x90
#define NOTEOFF			0x80
#define CTRL				0xB0
#define PITCH				0xE0



// MIDIOUT device handle
typedef struct tMidiOutDev tMidiOutDev;
struct tMidiOutDev
{
	unsigned Number;
	HMIDIOUT Handle;
	tMidiOutDev *next;
};


// MIDIIN event handle
typedef struct tMidiInEvent tMidiInEvent;
struct tMidiInEvent
{
	unsigned char event;
	unsigned char dat1;
	unsigned char dat2;
	tMidiInEvent *next;
};

// MIDIIN device handle
typedef struct tMidiInDev tMidiInDev;
struct tMidiInDev
{
	unsigned Number;
	HMIDIIN Handle;
	tMidiInEvent *pMidiInEventList;   // list of events for this device
	tMidiInDev *next;
};

// List of opened MIDIOUT devices
tMidiOutDev *pMidiOutDevList = NULL;

// List of opened MIDIIN devices
tMidiInDev *pMidiInDevList = NULL;



//**************************************************************************************
//*** dbgShowMidiInEventList
//*** 
//*** Prints out the contents of *dev's *event list.
//**************************************************************************************
void dbgShowMidiInEventList(tMidiInDev *dev)
{
	tMidiInEvent *p;
	if( dev == NULL )
		return;
		
	p=dev->pMidiInEventList;
	
	fprintf(stdout,"DEV %d:",dev->Number);
	while( p != NULL)
	{
		fprintf(stdout,"%02x ",p->event);
		p = p->next;
	}
	fprintf(stdout,"\n");
}



//**************************************************************************************
//*** AddMidiInEvent
//*** 
//*** Adds a MIDI event to the start of the corresponding IN device list.
//**************************************************************************************
void AddMidiInEvent(tMidiInDev *dev, tMidiInEvent *event)
{
	if( ( dev == NULL ) ||( event == NULL ) )
		return;

	event->next =	dev->pMidiInEventList;
	dev->pMidiInEventList = event;
}


//**************************************************************************************
//*** RemoveMidiInEvent
//*** 
//*** Removes and returns the last event of the list (if any).
//*** Event and data bytes will be passed to *event.
//*** If *event is NULL, this functions can be used to clear the event list,
//*** by continually calling until the return value <0;
//*** RETURNS:
//***  -1 : no event or invalid parameters
//***   0 : event valid and passed to *event
//**************************************************************************************
int RemoveMidiInEvent(tMidiInDev *dev, tMidiInEvent *event)
{
	tMidiInEvent *p, *pprev = NULL;
	
	if( dev == NULL )
		return -1;

	if( (p=dev->pMidiInEventList) == NULL )
		return -1;
		
	while( p->next != NULL )
	{
		pprev = p;
		p = p->next;
	}

	// unchain event
	if( pprev == NULL )
		dev->pMidiInEventList = NULL;
	else
		pprev->next = NULL;

	if( event != NULL)
	{		
		event->event = p->event;
		event->dat1  = p->dat1;
		event->dat2  = p->dat2;
		event->next  = NULL;	// just in case...
	}
	
	free(p);
	
	return 0;
}


//**************************************************************************************
//*** AddMidiOut
//*** 
//*** Adds a tMidiOutDev handle to the list of opened OUT devices.
//**************************************************************************************
void AddMidiOut(tMidiOutDev *tmp)
{
	if( tmp == NULL )
		return;
	tmp->next = pMidiOutDevList;
	pMidiOutDevList = tmp;
}



//**************************************************************************************
//*** AddMidiIn
//*** 
//*** Adds a tMidiInDev handle to the list of opened IN devices.
//**************************************************************************************
void AddMidiIn(tMidiInDev *tmp)
{
	if( tmp == NULL )
		return;
	tmp->next = pMidiInDevList;
	pMidiInDevList = tmp;
}



//**************************************************************************************
//*** RemoveMidiOut
//*** 
//*** Removes a tMidiOutDev handle from the list of opened OUT devices.
//**************************************************************************************
void RemoveMidiOut(tMidiOutDev *tmp)
{
	tMidiOutDev *p = pMidiOutDevList;
	if( tmp == NULL )
		return;

	if( p == tmp )
	{
		pMidiOutDevList = tmp->next;
	}
	else
		while( p != NULL )
		{
			if( p->next == tmp )
			{
				p->next = tmp->next;
				break;
			}
			p = p->next;
		}	

	free(tmp);

}



//**************************************************************************************
//*** RemoveMidiIn
//*** 
//*** Removes a tMidiInDev handle from the list of opened IN devices.
//**************************************************************************************
void RemoveMidiIn(tMidiInDev *tmp)
{
	tMidiInDev *p = pMidiInDevList;
	if( tmp == NULL )
		return;

	if( p == tmp )
	{
		pMidiInDevList = tmp->next;
	}
	else
		while( p != NULL )
		{
			if( p->next == tmp )
			{
				p->next = tmp->next;
				break;
			}
			p = p->next;
		}	
	
	while( RemoveMidiInEvent(tmp,NULL) >= 0 )
	{;}
	free(tmp);
}



//**************************************************************************************
//*** GetMidiOutEntry
//*** 
//*** Returns the valid tMidiOutDev handler for the opened device number <num>
//**************************************************************************************
tMidiOutDev *GetMidiOutEntry(unsigned num)
{
	tMidiOutDev *p = pMidiOutDevList;

	if( p == NULL )
		return NULL;

	for(;;)
	{
		if( p->Number == num )
			break;
		if( p->next == NULL )
			return NULL;
		p = p->next;
	}

	return p;	
}



//**************************************************************************************
//*** GetMidiInEntry
//*** 
//*** Returns the valid tMidiInDev handler for the opened device number <num>
//**************************************************************************************
tMidiInDev *GetMidiInEntry(unsigned num)
{
	tMidiInDev *p = pMidiInDevList;

	if( p == NULL )
		return NULL;

	for(;;)
	{
		if( p->Number == num )
			break;
		if( p->next == NULL )
			return NULL;
		p = p->next;
	}

	return p;	
}



//**************************************************************************************
//*** MidiSend
//*** 
//*** Sends a MIDI message
//**************************************************************************************
static int MidiSend(tMidiOutDev *dev, unsigned char msg, unsigned char key, unsigned char vel)
{
	union
	{
		unsigned long wval;
		unsigned char cval[4];
	} mmsg;

	if(( dev == NULL ) || ( dev->Handle == NULL) )
		return -1;

	mmsg.cval[0] = msg;
	mmsg.cval[1] = key & 0x7f;
	mmsg.cval[2] = vel & 0x7f;
	mmsg.cval[3] = 0;

	// no error checks, right now...
	midiOutShortMsg(dev->Handle, mmsg.wval);
	
	return 0; 
}


//**************************************************************************************
//*** MidiReceive CALLBACK FUNCTION
//*** 
//*** Saves any NOTEON/OFF, CONTROL CHANGE or PITCH BEND in the device buffer event list.
//**************************************************************************************
void CALLBACK MidiReceive(HMIDIIN dev, UINT msg, DWORD_PTR instdat, DWORD_PTR msgpar, DWORD_PTR time)
{
	union
	{
		unsigned long wval;
		unsigned char cval[4];
	} mmsg;
	tMidiInEvent *ev;
	
	mmsg.wval = msgpar;

	// only handle these events...
	if(  ( mmsg.cval[0] == NOTEON)
		 ||( mmsg.cval[0] == NOTEOFF)
		 ||( mmsg.cval[0] == CTRL)
		 ||( mmsg.cval[0] == PITCH) )
	{
		if( (ev=malloc(sizeof(tMidiInEvent))) == NULL )
			return;
		
		ev->event = mmsg.cval[0];
		ev->dat1  = mmsg.cval[1];
		ev->dat2  = mmsg.cval[2];
		ev->next  = NULL;
		
		AddMidiInEvent(GetMidiInEntry(instdat) ,ev);
	}
}



//**************************************************************************************
//*** plOpenMidiOut
//*** 
//*** Opens a MIDIOUT device
//*** LUA STACK IN:
//***  L1 -> number of device
//*** LUA STACK OUT:
//***   0: OK
//***  <0: ERROR
//**************************************************************************************
static int plOpenMidiOut(lua_State *L)
{
	unsigned i;
	tMidiOutDev *tmp;
	HMIDIOUT handle;

	if( lua_gettop(L) != 1 )
	{
		lua_pushnumber(L, -1);
		return 1; 
	}

	// do not open the same device a second time
	if( GetMidiOutEntry(luaL_checkinteger(L,1)) != NULL )
	{
		lua_pushnumber(L, -2);
		return 1; 
	}

	if( midiOutOpen(&handle, (i=luaL_checkinteger(L,1)), 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR)
		lua_pushnumber(L, -3);
	else
	{
		tmp = (tMidiOutDev *)malloc(sizeof(tMidiOutDev));
		if( tmp == NULL )
			lua_pushnumber(L, -4);
		else
		{		
			tmp->Number = i;
			tmp->Handle = handle;
			AddMidiOut(tmp);
			lua_pushnumber(L,0);
		}
	}

	return 1;
}



//**************************************************************************************
//*** plOpenMidiIn
//*** 
//*** Opens a MIDIIN device
//*** LUA STACK IN:
//***  L1 -> number of device
//*** LUA STACK OUT:
//***   0: OK
//***  <0: ERROR
//**************************************************************************************
static int plOpenMidiIn(lua_State *L)
{
	unsigned i;
	tMidiInDev *tmp;
	HMIDIIN handle;

	if( lua_gettop(L) != 1 )
	{
		lua_pushnumber(L, -1);
		return 1; 
	}

	// do not open the same device a second time
	if( GetMidiInEntry(luaL_checkinteger(L,1)) != NULL )
	{
		lua_pushnumber(L, -2);
		return 1; 
	}

	if( midiInOpen(&handle, (i=luaL_checkinteger(L,1)), (DWORD_PTR)&MidiReceive, i, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
		lua_pushnumber(L, -3);
	else
	{
		tmp = (tMidiInDev *)malloc(sizeof(tMidiInDev));
		if( tmp == NULL )
		{
			midiInClose(handle);
			lua_pushnumber(L, -4);
		}
		else
		{		
			tmp->Number = i;
			tmp->Handle = handle;
			tmp->pMidiInEventList = NULL;
			AddMidiIn(tmp);
			midiInStart(tmp->Handle);
			lua_pushnumber(L,0);
		}
	}

	return 1;
}



//**************************************************************************************
//*** plCloseMidiOut
//*** 
//*** Closes a MIDIOUT device
//*** LUA STACK IN:
//***  L1 -> number of device
//*** LUA STACK OUT:
//***   0 : OK
//***  <0 : ERROR
//**************************************************************************************
static int plCloseMidiOut(lua_State *L)
{
	tMidiOutDev *tmp;

	if( lua_gettop(L) != 1 )
	{
		lua_pushnumber(L,-1);
		return 1;
	}

	if( (tmp=GetMidiOutEntry(luaL_checkinteger(L, 1))) != NULL )
	{
		midiOutReset(tmp->Handle);
		midiOutClose(tmp->Handle);
		RemoveMidiOut(tmp);
		lua_pushnumber(L,0);
	}
	else
		lua_pushnumber(L,-2);

	return 1; 
	
}



//**************************************************************************************
//*** plCloseMidiIn
//*** 
//*** Closes a MIDIIN device
//*** LUA STACK IN:
//***  L1 -> number of device
//*** LUA STACK OUT:
//***   0 : OK
//***  <0 : ERROR
//**************************************************************************************
static int plCloseMidiIn(lua_State *L)
{
	tMidiInDev *tmp;

	if( lua_gettop(L) != 1 )
	{
		lua_pushnumber(L,-1);
		return 1;
	}

	if( (tmp=GetMidiInEntry(luaL_checkinteger(L, 1))) != NULL )
	{
		midiInReset(tmp->Handle);
		midiInClose(tmp->Handle);
		RemoveMidiIn(tmp);
		lua_pushnumber(L,0);
	}
	else
		lua_pushnumber(L,-2);

	return 1; 
	
}






//**************************************************************************************
//*** plPlayNote
//*** 
//*** Plays a note
//*** LUA STACK IN:
//***  L1 -> number of MidiIn device
//***  L2 -> MIDI channel (1..16)
//***  L3 -> key
//***  L4 -> velocity
//***  L5 -> NOTEON/NOTEOFF
//*** LUA STACK OUT:
//***  0  -> OK
//*** <0  -> ERROR
//**************************************************************************************
static int plPlayNote(lua_State *L)
{
	tMidiOutDev *tmp;
	int channel;

	if( lua_gettop(L) != 5 )
	{
		lua_pushnumber(L,-1);
		return -1;
	}

	if( (tmp=GetMidiOutEntry(luaL_checkinteger(L,1))) == NULL )
	{
		lua_pushnumber(L,-2);
		return 1; 
	}

	channel = luaL_checkinteger(L,2);
	if(( channel < 1 )||( channel > 16 ) )
	{
		lua_pushnumber(L,-2);
		return 1; 
	}

	--channel;
	
	MidiSend(tmp,
	 (unsigned char)(luaL_checkinteger(L,5) & 0xf0) | (unsigned char)channel,
	 (unsigned char)luaL_checkinteger(L,3),
	 (unsigned char)luaL_checkinteger(L,4));

	lua_pushnumber(L,0);
	return 1; 
	
}


//**************************************************************************************
//*** plCountDevices
//*** 
//*** Counts number of attached MIDIIN or MIDIOUT devices
//*** LUA STACK IN:
//***  L1 -> MIDIIN/MIDIOUT
//*** LUA STACK OUT:
//***  number of devices
//**************************************************************************************
static int plCountDevices(lua_State *L)
{
	if( lua_gettop(L) != 1 )
	{
		lua_pushinteger(L,0);
		return 1;
	}
	switch( luaL_checkinteger(L,1) )
	{
		case MIDIIN:  lua_pushinteger(L, midiInGetNumDevs()); break;
		case MIDIOUT: lua_pushinteger(L, midiOutGetNumDevs()); break;
		default: lua_pushinteger(L, 0);
	}
	
	return 1;
}



//**************************************************************************************
//*** plGetMidiOutName
//*** 
//*** Returns the name of an attached MIDIOUT device.
//*** LUA STACK IN:
//***  L1 -> number of device
//*** LUA STACK OUT:
//***  "string" : name of device
//***  nil      : ERROR
//**************************************************************************************
static int plGetMidiOutName(lua_State *L)
{
	MIDIOUTCAPS mcaps;
	
	if( lua_gettop(L) != 1 )
	{
		lua_pushnil(L);
		return 1;
	}

	if( midiOutGetDevCaps(luaL_checkinteger(L,1),&mcaps,sizeof(mcaps)) != MMSYSERR_NOERROR )
		lua_pushnil(L);
	else
		lua_pushstring(L,(char *)&mcaps.szPname);
	
	return 1;
}



//**************************************************************************************
//*** plGetMidiInName
//*** 
//*** Returns the name of an attached MIDIIN device.
//*** LUA STACK IN:
//***  L1 -> number of device
//*** LUA STACK OUT:
//***  "string" : name of device
//***  nil      : ERROR
//**************************************************************************************
static int plGetMidiInName(lua_State *L)
{
	MIDIINCAPS mcaps;
	
	if( lua_gettop(L) != 1 )
	{
		lua_pushnil(L);
		return 1;
	}

	if( midiInGetDevCaps(luaL_checkinteger(L,1),&mcaps,sizeof(mcaps)) != MMSYSERR_NOERROR )
		lua_pushnil(L);
	else
		lua_pushstring(L,(char *)&mcaps.szPname);
	
	return 1;
}


//**************************************************************************************
//*** plGetEvent
//*** 
//*** Returns the oldest MIDI event of a device (L,1).
//*** LUA STACK IN:
//***  L1 -> number of MidiIn device
//*** LUA STACK OUT:
//***  nil    -> no event available
//*** {a,b,c} -> a table: a=EVENT, b=data1, c=data2
//**************************************************************************************
static int plGetEvent(lua_State *L)
{
	tMidiInEvent ev;
	
	if( lua_gettop(L) != 1 )
	{
		lua_pushnil(L);
		return 1;
	}

	if( RemoveMidiInEvent(GetMidiInEntry(luaL_checknumber(L,1)), &ev) >= 0 )
	{
		lua_createtable(L, 4, 0); // 4 elements: EVENT, CHANNEL, DATA1, DATA2
		lua_pushinteger(L,1); lua_pushinteger(L,ev.event & 0xf0); lua_settable(L,-3);
		lua_pushinteger(L,2); lua_pushinteger(L,(ev.event & 0x0f)+1); lua_settable(L,-3);
		lua_pushinteger(L,3); lua_pushinteger(L,ev.dat1); lua_settable(L,-3);
		lua_pushinteger(L,4); lua_pushinteger(L,ev.dat2); lua_settable(L,-3);
	}
	else
		lua_pushnil(L);
	
	return 1;
}




static const struct luaL_Reg midi_funcs[] = {
  { "OpenMidiOut",		plOpenMidiOut },
  { "CloseMidiOut",		plCloseMidiOut },
  { "OpenMidiIn",			plOpenMidiIn },
  { "CloseMidiIn",		plCloseMidiIn },
  { "GetMidiOutName",	plGetMidiOutName },
  { "GetMidiInName",	plGetMidiInName },
  { "SendMessage",		plPlayNote },
  { "GetMessage",     plGetEvent },
  { "CountDevices",		plCountDevices },
  { NULL, NULL }
};

//**************************************************************************************
//*** luaopen_profimidi
//*** 
//*** 
//**************************************************************************************
LUALIB_API int luaopen_profimidi(lua_State *L)
{
  luaL_register(L, "promidi", midi_funcs);
  
	lua_pushinteger(L,MIDIIN);
	lua_setglobal(L,"MIDIIN");
  
	lua_pushinteger(L,MIDIOUT);
	lua_setglobal(L,"MIDIOUT");

	lua_pushinteger(L,NOTEOFF);
	lua_setglobal(L,"NOTEOFF");

	lua_pushinteger(L,NOTEON);
	lua_setglobal(L,"NOTEON");

	lua_pushinteger(L,CTRL);
	lua_setglobal(L,"CTRL");

	lua_pushinteger(L,PITCH);
	lua_setglobal(L,"PITCH");
  
  return 1;
}

