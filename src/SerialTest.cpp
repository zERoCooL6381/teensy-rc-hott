#include <Arduino.h>
#include "PITimer.h"
#include "lib_crc.h"
#include "Servo.h"
#include "PID_v1.h"

/* HOTT Read, read serial data from Graupner HOTT GR-32 receiver
   by Cord Johannmeyer

   Date: 2013-04-05
 
   This example code is in the public domain.
*/


const int ledPin = LED_BUILTIN;   // Teensy 3.0 on board LED (13)
const int rpmPin = 3;    // for RPM sensor input

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

struct tPIDParam {
	double in, out, set;
	double kp, ki, kd;
	int dir;
	double rmin, rmax;
	tPIDParam(): in(0.0), out(0.0), set(0.0), kp(1.0), ki(0.0), kd(0.0), dir(0), rmin(1000.0), rmax(5000.0) {};
} PIDParam;

unsigned char* pRawBuffer = (unsigned char* )&(serData.rxFrame);
volatile uint32_t current;
volatile uint32_t lastPulseEvent;
volatile uint32_t diffTime;

Servo servo_ch0;
Servo servo_ch1;

PID rpmCtrl(&PIDParam.in, &PIDParam.out, &PIDParam.set, PIDParam.kp, PIDParam.ki, PIDParam.kd, PIDParam.dir);

void timerCallback0() {
	serData.cnt = 0;
	serData.state = WaitStart;
//	digitalWrite(ledPin, HIGH);   // set the LED on
}

void timerCallback1() {
}

double calcSetpoint(double in)
{
	return PIDParam.rmin + (PIDParam.rmax - PIDParam.rmin) * (in - 1000.0) / 1000.0 ;
}

int calcOutput(double out)
{
	return (int)1000 + 1000.0 * (out - PIDParam.rmin) / (PIDParam.rmax - PIDParam.rmin);
}

void rpm_isr()
{
	digitalWrite(2, HIGH);   // set the LED on
	digitalWrite(2, LOW);
	current = PITimer1.current();
	diffTime = lastPulseEvent - current;
	lastPulseEvent = current;
}

void setup()   {
	pinMode(ledPin, OUTPUT);
	pinMode(2, OUTPUT);
//	pinMode(4, OUTPUT);
	pinMode(rpmPin, INPUT);
	Serial.begin(38400);
	Uart.begin(115200);
	PITimer0.period(0.002); // 2 milliseconds timeout to synchronize frames
	PITimer0.start(timerCallback0);

	/// use PITimer1 as free running Timer
	PITimer1.value(0xFFFFFFFF);
	PITimer1.start(timerCallback1);

	attachInterrupt(rpmPin, rpm_isr, FALLING);

	rpmCtrl.SetSampleTime(10);
	rpmCtrl.SetOutputLimits(1000.0, 5000.0);
	rpmCtrl.SetMode(1);

	servo_ch0.attach(4);
	servo_ch1.attach(5);
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

		uint32_t rpm = (uint32_t(F_CPU)*60)/diffTime;

		double ch0 = 1000.0;

		if(crc == serData.rxFrame.crc) {
			ch0 = serData.rxFrame.rxChannel[0] * 0.2 - 900.0;
			Serial.print("$ ");
			Serial.print(rpm);
			Serial.print(" ");
			Serial.print(ch0);
			if(ch0 > 1500.0) 	digitalWrite(ledPin, HIGH);   // set the LED on
			else	digitalWrite(ledPin, LOW);

//			analogWrite(4,int(0.255*(ch0-1000.0)));
//			servo_ch0.writeMicroseconds(int(ch0));
			servo_ch1.writeMicroseconds(int(ch0));

//			Serial.println(" ok");
		}
//		else {
//			Serial.println(" err");
//		}

		serData.cnt = 0;
		crc = 0;

		// PID controller

		PIDParam.in = (double)rpm;
		PIDParam.set = calcSetpoint(ch0);
		rpmCtrl.Compute();
		int ch0_out = calcOutput(PIDParam.out);
		servo_ch0.writeMicroseconds(ch0_out);
		Serial.print(" ");
		Serial.print(PIDParam.set);
		Serial.print(" ");
		Serial.println(ch0_out);


	}
}
