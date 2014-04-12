ProfiLua
========

A [Lua][4] extension for Profilab [http://www.abacom-online.de/html/profilab-expert.html][1].  
For binaries, examples and more, see [http://www.askrprojects.net/software/profilua/index.html][2]

Released parts of the serial interface as [BecauseHW!SerialLib][5].

-----------------------------------------------------------------------------------------------


## NEWS

#### 04/2014

  Serial interface is up and running.  
  Available functions, for now:

   - Mount()
   - UnMount()
   - CheckPort()
   - Open()
   - Close()
   - SendByte()
   - BufferCount()
   - Config()
   - ReadByte()
   - BufferFlush()

## TODO

  - documentation
  - update to (newest) Lua kernel 5.2.x
  - ...


## BUILDING

  As of version V1.0, ProfiLua now comes with only Code::Blocks project files for [TDM-GCC][3].  
  Due to some bugs in the TDM64 release, the 32 bit edition of TDM-GCC is required.

  This may change in the future...



## CHANGES

### 0.9c

    - CHG: renamed "prolua.getkey()" to "prolua.GetKey()"
    - NEW: added MIDI functionality in library "promidi"
        - function OpenDevice()
        - function CloseDevice()
        - function PlayNote()
        - variable MIDIIN
        - variable MIDIOUT

### 1.0

    - CHG: switched to TDM-GCC and Code::Blocks
    - NEW: serial interface



Have fun  
FMMT666(ASkr)


[1]: http://www.abacom-online.de/html/profilab-expert.html
[2]: http://www.askrprojects.net/software/profilua/index.html
[3]: http://tdm-gcc.tdragon.net/
[4]: http://www.lua.org/
[5]: https://github.com/FMMT666/BecauseHWSerialLib
