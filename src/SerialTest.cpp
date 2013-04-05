#include <Arduino.h>
#include "PITimer.h"
#include "lib_crc.h"

/* HOTT Read, read serial data from Graupner HOTT GR-32 receiver
   by Cord Johannmeyer

   Date: 2013-04-05
 
   This example code is in the public domain.
*/


const int ledPin = 13;   // Teensy 3.0 on board LED

enum eSerialState { WaitStart, ReadHeader, ReadData, ReadCrc };
HardwareSerial Uart = HardwareSerial();

struct __attribute__((packed)) tRxFrameHott {
	unsigned char startByte;
	unsigned char rxStatus;
	unsigned char nChannels;
	unsigned short rxChannel[16];
	unsigned short crc;
};

const int bytesPerFrame = sizeof(tRxFrameHott);

static unsigned char rawIndex[] = {
		 0,1,2,
		 4, 3,	// channel 0
		 6, 5,	// channel 1
		 8, 7,	// channel 2
		10, 9,	// channel 3
		12,11,	// channel 4
		14,13,	// channel 5
		16,15,	// channel 6
		18,17,	// channel 7
		20,19,	// channel 8
		22,21,	// channel 9
		24,23,	// channel 10
		26,25,	// channel 11
		28,27,	// channel 12
		30,29,	// channel 13
		32,31,	// channel 14
		34,33,	// channel 15
		36,35	// crc
};

struct tSerData {
	eSerialState state;
	unsigned char cnt;
	tRxFrameHott rxFrame;
	tSerData() : state(WaitStart), cnt(0) {};
} serData;

unsigned char* pRawBuffer = (unsigned char* )&(serData.rxFrame);

void timerCallback0() {
	serData.cnt = 0;
	serData.state = WaitStart;
//	digitalWrite(ledPin, HIGH);   // set the LED on
}

void setup()   {
	pinMode(ledPin, OUTPUT);
	Serial.begin(38400);
	Uart.begin(115200);
	PITimer0.period(0.002); // 2 milliseconds timeout to synchronize frames
	PITimer0.start(timerCallback0);
}

unsigned short crc = 0;

void loop()
{
	if(Uart.available()) {
		PITimer0.reset();
//		digitalWrite(ledPin, LOW);    // set the LED off
		int ch = Uart.read();
		if(serData.cnt < bytesPerFrame-2) crc = update_crc_ccitt(crc, ch);

		if(ch == 0xA8 && serData.state == WaitStart) {
			serData.state = ReadHeader;
			pRawBuffer[rawIndex[serData.cnt++]] = ch;
		}
		else if(serData.cnt < bytesPerFrame){
			pRawBuffer[rawIndex[serData.cnt++]] = ch;
		}
	}

	if(serData.cnt == bytesPerFrame) {

		if(crc == serData.rxFrame.crc) {
			float ch0 = serData.rxFrame.rxChannel[0] * 0.2 - 900.0;
			Serial.print("ch0: ");
			Serial.print(ch0);
			if(ch0 > 1500.0) 	digitalWrite(ledPin, HIGH);   // set the LED on
			else	digitalWrite(ledPin, LOW);
			Serial.println(" ok");
		}
		else {
			Serial.println(" err");
		}
//		Serial.print(rxFrame->crc, HEX);
//		Serial.print("!");
//		Serial.println(crc,HEX);

		serData.cnt = 0;
		crc = 0;
	}
}
