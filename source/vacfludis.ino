// VFD.cpp, vacuum fluroescent example for Samsung HCS-12SS59T, V0.5 171030 qrt@qland.de

#include <Arduino.h>

#include "VFD.h"

void setup()
{
    pinMode(Pin_RED_LED13, OUTPUT);
	digitalWrite(Pin_RED_LED13, HIGH);

	Serial.begin(9600);
	Vfd.init();

	Vfd.write("QRT@QLAND_DE * ABCDEFGHIJKLMNOPQRSTUVWXYZ * 0123456789 * ");		// display 
	Vfd.scroll(20);																// scroll left  (+) 20 * 0.01 chars per second
}																				//        right (-)
																				//        stop  (0)
void loop()
{
	digitalWrite(Pin_RED_LED13, !digitalRead(Pin_RED_LED13));	
	delay(1000);
}
