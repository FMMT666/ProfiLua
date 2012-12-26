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

#define PROFILUAVERSION    "v0.9c"

#ifndef uchar
#define uchar unsigned char
#endif


#define LOGIC_HIGH         5.0	// a high state
#define LOGIC_LOW          0    // a low state
#define LOGIC_HIGH_THRES   2.5  // threshold level for high, including the value itself (HIGH >= LOGIC_HIGH_THRES)


#ifndef _DLL_H_
#define _DLL_H_

#define DLLIMPORT __declspec (dllexport)

//DLLIMPORT uchar _stdcall NumInputs();
//DLLIMPORT uchar _stdcall NumOutputs();
DLLIMPORT uchar _stdcall CNumInputsEx(double *PUser);
DLLIMPORT uchar _stdcall CNumOutputsEx(double *PUser);

DLLIMPORT void _stdcall GetInputName(uchar Channel, uchar *Name);
DLLIMPORT void _stdcall GetOutputName(uchar Channel, uchar *Name);

//DLLIMPORT void _stdcall CCalculate(double *PInput, double *POutput, double *PUser);
DLLIMPORT void _stdcall CCalculateEx(double *PInput, double *POutput, double *PUser, uchar **PStrings);

DLLIMPORT void _stdcall CSimStart(double *PInput, double *POutput, double *PUser);
DLLIMPORT void _stdcall CSimStop(double *PInput, double *POutput, double *PUser);

//DLLIMPORT void _stdcall CConfigure(double *PUser);                                   

#endif
