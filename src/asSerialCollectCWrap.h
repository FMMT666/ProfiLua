//
// BecauseHW!SerialLib,
// derived from asSerial, a CSerial wrapper object
// FMMT666(ASkr), 2010, 2012, 2013, 2014; www askrprojects.net
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#ifdef __cplusplus


extern "C"
{
	int Mount( void );										// creates a serial object with the first, free number
	bool UnMount( int handle );						// unmounts and destroys the serial object referenced by "handle"
	bool Check( int handle, int port );		// checks if a serial port (COM port) exists
	bool Open( int handle, int port );		// opens the serial COM port "port" on the object referenced by "handle"
	bool Close( int handle );							// closes the serial port referenced by handle (if open)
	bool SendByte( int handle, int byte );// sends a byte over the serial port referenced by "handle"
	int BufferCount( int handle );				// count the number of bytes in the receive buffer
	bool Config( int handle, long baud, int bits, int parity, int stop); // configuration of the serial object references by "handle"
	int ReadByte( int handle );						// read an already received byte from the receive buffer
	bool BufferFlush( int handle );				// empties the complete receive buffer
	bool SendArray( int handle, int *pData, int length); // TODO! sends an array over the serial port referenced by "handle"
	int ReadArrayN( int handle, int length, int *pData);// TODO! reads an array (a number of bytes) from the receive buffer

/*
	int ReadPacket( int handle, int length, int *pData);// TODO! receive a data packet
	int CountPacket( int handle );				// count number of received, complete data packets
	bool SendPacket( int handle, int *pData, int length );// TODO! sends a data packet
	bool SetPacketStart( int handle, int *pData ); // set packet start sequence
	bool SetPacketEnd( int handle, int *pData ); // set packet end delimiter
	bool SetPacketChar( int handle, int specialChar ); // set special packet character
*/
}

#else

#include <stdbool.h>

extern int Mount( void );										// creates a serial object with the first, free number
extern bool UnMount( int handle );						// unmounts and destroys the serial object referenced by "handle"
extern bool Check( int handle, int port );		// checks if a serial port (COM port) exists
extern bool Open( int handle, int port );		// opens the serial COM port "port" on the object referenced by "handle"
extern bool Close( int handle );							// closes the serial port referenced by handle (if open)
extern bool SendByte( int handle, int byte );// sends a byte over the serial port referenced by "handle"
extern int BufferCount( int handle );				// count the number of bytes in the receive buffer
bool Config( int handle, long baud, int bits, int parity, int stop); // configuration of the serial object references by "handle"
int ReadByte( int handle );						// read an already received byte from the receive buffer
bool BufferFlush( int handle );				// empties the complete receive buffer
bool SendArray( int handle, int *pData, int length); // TODO! sends an array over the serial port referenced by "handle"
int ReadArrayN( int handle, int length, int *pData);// TODO! reads an array (a number of bytes) from the receive buffer

/*
int ReadPacket( int handle, int length, int *pData);// TODO! receive a data packet
int CountPacket( int handle );				// count number of received, complete data packets
bool SendPacket( int handle, int *pData, int length );// TODO! sends a data packet
bool SetPacketStart( int handle, int *pData ); // set packet start sequence
bool SetPacketEnd( int handle, int *pData ); // set packet end delimiter
bool SetPacketChar( int handle, int specialChar ); // set special packet character
*/

#endif // __cplusplus








