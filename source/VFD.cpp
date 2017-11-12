// VFD.cpp, vacuum fluorescent display driver for Samsung HCS-12SS59T, V0.9 171112 qrt@qland.de

#include "VFD.h"
#include "TimerOne.h"

VFD Vfd;

SPISettings settingsA(SPIPARS);

void VFD::init()
{
	pinMode(Pin_VFD_RESET, OUTPUT);
	pinMode(Pin_VFD_CS, OUTPUT);

	digitalWrite(Pin_VFD_RESET, HIGH);		// VFD _RESET OFF
	digitalWrite(Pin_VFD_CS, HIGH);			//     _CS

	for(int16_t i=0; i<NUMDIGITS; i++)		// preset display buffer
		buf[i] = 0xff;						// with unused char

	buf[NUMDIGITS] = '\0';					// terminate buffer

#if SUPPLYMODE==0
	pinMode(Pin_VFD_VDON, OUTPUT);			// _VDON output
	digitalWrite(Pin_VFD_VDON, HIGH);		// Vdisp OFF
#else
	pinMode(Pin_VFD_SUCLK_A, OUTPUT);		// supply clock outputs
	pinMode(Pin_VFD_SUCLK_B, OUTPUT);
	//digitalWrite(Pin_VFD_SUCLK_A, LOW);	// filament supply bridge L:L
	//digitalWrite(Pin_VFD_SUCLK_B, LOW);
#endif

	supplyOn();								// supply on
	SPI.begin();

	digitalWrite(Pin_VFD_RESET, LOW);		// reset
	delayMicroseconds(1);					// tWRES
	digitalWrite(Pin_VFD_RESET, HIGH);
	delayMicroseconds(1);					// tRSOFF

	SPI.beginTransaction(settingsA);

	sendCmd(VFD_NUMDIGIT, NUMDIGITS);		// number of digits
	sendCmd(VFD_DUTY, 4);					// brightness 1..15
	sendCmd(VFD_LIGHTS, LINORM);			// lights normal

	SPI.endTransaction();
}

void VFD::supplyOn()
{
#if SUPPLYMODE==0
	digitalWrite(Pin_VFD_VDON, LOW);				// Vdisp ON
#else
	TCCR2A = 1<<COM2A1 | 1<<COM2B1 | 1<<COM2B0;		// OCR2A low, OCR2B high
	TCCR2B = 1<<FOC2A | 1<<FOC2B;					// force output compare to fix phase, filament supply bridge L:H

	TCCR2A = 1<<COM2A0 | 1<<COM2B0 | 1<<WGM21;		// OCR2A, OCR2B toggle, CTC

	OCR2A = SUPPLYCYC - 1;							// supply clock cycle
	OCR2B = SUPPLYCYC - 1;

	TCCR2B = 1<<CS21;								// DIV 8, start supply clock
#endif

	delay(1);
}

void VFD::supplyOff()
{
#if SUPPLYMODE==0
	digitalWrite(Pin_VFD_VDON, HIGH);					// Vdisp OFF
#else
	TCCR2B = 0;											// stop supply clock
	TCCR2A = 0;											// OCR2A, OCR2B normal port operation
	//digitalWrite(Pin_VFD_SUCLK_A, LOW);				// filament supply bridge L:L
	//digitalWrite(Pin_VFD_SUCLK_B, LOW);
#endif

	delay(1);
}

void VFD::write(char* text)
{
	scrLen = strlen(text);
	scrPos = NUMDIGITS-1;

	if(scrLen > BUFSIZE-1){
		scrLen = BUFSIZE-1;
		text[scrLen] = '\0';
	}

	strcpy(buf, text);
	display();
}

void doScroll()
{
	Vfd.display();

	if(Vfd.scrMode > 0){
		if(++Vfd.scrPos >= Vfd.scrLen)
			Vfd.scrPos = 0;
	}
	else{
		if(--Vfd.scrPos < 0)
			Vfd.scrPos = Vfd.scrLen-1;
	}
}

void VFD::scroll(int16_t mode)
{
	static bool tirat = false;

	scrMode = mode;

	if(mode == 0){
		Timer1.detachInterrupt();
	}
	else{
		Timer1.initialize((uint32_t)abs(mode) * 10000);

		if(!tirat){
			Timer1.attachInterrupt(doScroll);
			tirat = true;
		}
	}
}

void VFD::display()
{
	SPI.beginTransaction(settingsA);
	select(Pin_VFD_CS);

	sendCmdSeq(VFD_DCRAM_WR, 0);

	int16_t p = scrPos;

	for(int16_t i=0; i<NUMDIGITS; i++){
		sendChar(buf[p--]);

		if(p < 0)
			p = scrLen-1;
	}

	deSelect(Pin_VFD_CS);
	SPI.endTransaction();
}

void VFD::select(int pin)
{
	digitalWrite(pin, LOW);				// select
	delayMicroseconds(1);				// tCSS
}

void VFD::deSelect(int pin)
{
	delayMicroseconds(8);				// 1/2 tCSH
	digitalWrite(pin, HIGH);			// deselect
}

void VFD::sendCmd(char cmd, char arg)
{
	select(Pin_VFD_CS);					// select
	SPI.transfer(cmd | arg);			// send command and argument
	delayMicroseconds(8);				// 1/2 tCSH
	deSelect(Pin_VFD_CS);				// deselect
}

void VFD::sendCmdSeq(char cmd, char arg)
{
	SPI.transfer(cmd | arg);			// send command and argument
	delayMicroseconds(8);				// tDOFF
}

void VFD::sendChar(char c)
{
	SPI.transfer(getCode(c));			// send data
	delayMicroseconds(8);				// tDOFF and 1/2 tCSH for last data
}

char VFD::getCode(char c)
{
	if(c>='@' && c<='_')				// 64.. -> 16..
		c -= 48;
	else if(c>=' ' && c<='?')			// 32.. -> 48..
		c += 16;
	else if(c>='a' && c<='z')			// 97.. -> 17..
		c -= 80;
	else								// unvalid -> ?
		c = 79;

	return c;
}