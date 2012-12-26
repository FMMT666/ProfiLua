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
#include <stdio.h>
#include <io.h>
#include <fcntl.h>


#define _PROFICONSOLE


#include "proficonsole.h"


long StdInHandle  = 0;
long StdOutHandle = 0;
long StdErrHandle = 0;

FILE *fin  = NULL;
FILE *fout = NULL;
FILE *ferr = NULL;



//****************************************************************************
//*** DebugConsoleTop
//***
//*** Bring the console debug window to top.
//****************************************************************************
void DebugConsoleTop()
{
	BringWindowToTop(FindWindow(NULL,PROFICONSOLENAME));
}



//****************************************************************************
//*** DebugConsoleOff
//***
//*** Closes the debug console.
//****************************************************************************
void DebugConsoleOff()
{
/*
	SetStdHandle(STD_ERROR_HANDLE, StdErrHandle);
	SetStdHandle(STD_INPUT_HANDLE, StdInHandle);
	SetStdHandle(STD_OUTPUT_HANDLE, StdOutHandle);
*/

	fflush(fout);
	fclose(fout);
	fout = NULL;
	
	fclose(fin);
	fin = NULL;

	fflush(ferr);
	fclose(ferr);
	ferr = NULL;

	FreeConsole();
}



//****************************************************************************
//*** DebugConsoleHandler
//***
//*** Only here to catch CTRL-C, etc... events.
//****************************************************************************
DWORD DebugConsoleHandler(DWORD ctrltype)
{
	// not used
	switch(ctrltype)
	{
		case CTRL_C_EVENT					: break;
		case CTRL_BREAK_EVENT			: break;
		case CTRL_CLOSE_EVENT			: break;
		case CTRL_LOGOFF_EVENT		: break;
		case CTRL_SHUTDOWN_EVENT	: break;
	}

	MessageBox(NULL,"DO NOT TOUCH THAT BUTTON!",PROFICONSOLEMSGNAME,MB_OK | MB_ICONSTOP | MB_SETFOREGROUND);
	BringWindowToTop(FindWindow(NULL,PROFICONSOLEMSGNAME));

	return TRUE;
}



//****************************************************************************
//*** DebugConsole
//***
//*** Opens a console window and redirects all stdin, -out and -err
//*** pipes.
//****************************************************************************
void DebugConsoleOn()
{
	int hConHandle;
//	long StdHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *f;

	AllocConsole();
	
	SetConsoleTitle(PROFICONSOLENAME);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE),ENABLE_PROCESSED_INPUT);

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&coninfo);
	coninfo.dwSize.Y = PROFICONSOLE_LINES;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),coninfo.dwSize);

	SetConsoleCtrlHandler((void *)&DebugConsoleHandler,TRUE);

	
	StdOutHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(StdOutHandle, _O_TEXT);
	fout = stdout;
	f = _fdopen( hConHandle, "w" );
	*stdout = *f;
	setvbuf( stdout, NULL, _IONBF, 0 );

	StdInHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(StdInHandle, _O_TEXT);
	fin = stdin;
	f = _fdopen( hConHandle, "r" );
	*stdin = *f;
	setvbuf( stdin, NULL, _IONBF, 0 );

	StdErrHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(StdErrHandle, _O_TEXT);
	ferr = stderr;
	f = _fdopen( hConHandle, "w" );
	*stderr = *f;
	setvbuf( stderr, NULL, _IONBF, 0 );

}

