#include "nissanconsultcom.h"

nissanconsultcom::nissanconsultcom()
{

}


// Nissan Consult

//Request Sensor Data

/*
Read any Register Parameter (Live sensor data stream)
(0x5A )<parameter code to read>(0xf0) for single byte stream
(0x5A )<parameter>(0x5A )<next parameter>(0x5A )<next parameter>(0xf0)
for a multi-byte response stream. Maximum of 20 bytes.
terminate command with (0xf0) to start data stream, stop with (0x30).
*/
/*
Initialize Communication :

(FF FF EF)

Request Data Stream
(5A  0B 5A  01 5A  08 5A 0C 5A 0D 5A 03 5A 05 5A 09 5A 13 5A 16 5A 17 5A 1A 5A 1C 5A 21 F0)


Stream all available Sensors
0x9F
*/
