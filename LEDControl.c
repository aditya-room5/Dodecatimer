/*
 * LEDControl.c
 *
 *  Created on: Mar 20, 2014
 *      Author: aditya.bhutada
 */
//#include "LEDPatterns.h"
#include "LEDControl.h"
//#include "Hal.h"
#include <msp430.h>


#define DATA_OUT_PIN 		BIT7




RGBLED_t RGBLEDS[NUMBER_OF_LEDS] = { 0, };




static void setupSPIMaster(void);



void setupForLEDControl (void) {
	/*
	 * Set up DCO Clock to 16 MHz
	 * & change the BLE Uart speed accordingly
	 *
	 * (Could be done already in Hal_init(), but simply to not modify it much,
	 * this code below overrides standard clock settings.
	 */


	/* DCO , MCLK, SMCLK - set to 16 MHz */
	BCSCTL1 = CALBC1_16MHZ;      /* Set DCO to 16MHz */
	DCOCTL = CALDCO_16MHZ;

	/*
	 * MSP430 Programming Guide: Page 424
	 * For a baud rate speed of 115200:
	 *
	 * With 1 MHz clock rate (SMCLK)
	 * UCBRx = 8, CSBRSx = 6, UCBRFx = 0, UCOS16 = 0;
	 *
	 * With 8 MHz clock rate (SMCLK)
	 * UCBRx = 69, CSBRSx = 4, UCBRFx = 0, UCOS16 = 0;
	 *
	 * With 16 MHz clock rate (SMCLK)
	 * UCBRx = 138, CSBRSx = 7, UCBRFx = 0, UCOS16 = 0;
	 */


	/* Change UART1 Speed Devider to match with new DCO Clock rate */
	/* 1,000,000  : 115200 : 8    6  0 */
	/* 16,000,000 : 115200 : 138  7  0 */
	UCA0CTL1 = UCSSEL_2 + UCSWRST;
	UCA0MCTL = UCBRF_0 + UCBRS_7;
	UCA0BR0 = 138;
	UCA0CTL1 &= ~UCSWRST;


	setupSPIMaster();

}

/*
 * Set P1.7 to SPI Master
 *
 * Refer:
 * MSP430 Programming Guide: Page 444
 * for UCB0BR0 SPI mode settings
 *
 */
static void setupSPIMaster(void)
{
	// setup USIB, needed for the driver
	/* P1.7 - SPI Mode */
	P1SEL |= DATA_OUT_PIN;
	P1SEL2 |= DATA_OUT_PIN;
	UCB0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC; // 3-pin, 8-bit SPI master
	UCB0CTL1 |= UCSSEL_2; // SMCLK
	UCB0BR0 |= 0x04; // 1:4 - 16MHz/4 = 0.25us
	UCB0BR1 = 0;
	UCB0CTL1 &= ~UCSWRST;

}

// this is driver section, depends on LED strip
/*
 * sendRGB()
 *
 * This function uses global / local file variables:
 * 		- RGBLED_t RGBLEDS[]
 * 		- NUMBER_OF_LEDS
 *
 * This function is responsible to drive number of LEDs (NUMBER_OF_LEDS) in LEDS array (RGBLEDS)
 * with SPI mode, pre-initialized at clock rate @16MHz
 *
 */
void sendRGB() {
	u_int LEDIndex = 0;
	u_char singleColorByte = 0;
	u_char colorBytes[3];
	u_char aByte;
	while (LEDIndex < NUMBER_OF_LEDS) {
		colorBytes[0] = RGBLEDS[LEDIndex].g;
		colorBytes[1] = RGBLEDS[LEDIndex].r;
		colorBytes[2] = RGBLEDS[LEDIndex].b;
		while (singleColorByte < 3) {
			u_char bitsCount = 0;
			aByte = colorBytes[singleColorByte];
			while (bitsCount < 8) {
				while (!(IFG2 & UCB0TXIFG))
					;
				(aByte & 0x80) ? (UCB0TXBUF = 0xF0) : (UCB0TXBUF = 0xC0);
				aByte <<= 1;
				bitsCount++;
			}
			singleColorByte++;
		}
		singleColorByte = 0;
		LEDIndex++;
	}
//	_delay_cycles(800); // delay 50us to latch data, may not be necessary
}


/*
 * LED Animation
 *
 * ?
 */

void SetLEDPattern(int odd)
{
	int even = odd ? 1 : 0;

	// animation section
	const RGBLED_t onLED = { 0xFF, 0xFF, 0 }; // default color
	const RGBLED_t offLED = { 0, 0, 0 };

	unsigned int i = 0;

	for(i = 0 ;i < NUMBER_OF_LEDS; i++)
	{
		if(i % 2 == even)
		{
			RGBLEDS[i] = onLED;
		}
		else
		{
			RGBLEDS[i] = offLED;
		}

	}

	sendRGB();
}


void SetSliderValue(int sliderValue)
{
	if (sliderValue >= NUMBER_OF_LEDS)
		sliderValue = NUMBER_OF_LEDS;

	// animation section
	const RGBLED_t onLED = { 0xF1, 0x2F, 0x68 }; // default color
	const RGBLED_t offLED = { 0, 0, 0 };

	unsigned int i = 0;

	for(i = 0 ;i < NUMBER_OF_LEDS; i++)
	{
		if(i < sliderValue)
		{
			RGBLEDS[i] = onLED;
		}
		else
		{
			RGBLEDS[i] = offLED;
		}

	}

	sendRGB();

}


void SetHollowSliderValue(int sliderValue)
{
	if (sliderValue >= NUMBER_OF_LEDS)
		sliderValue = NUMBER_OF_LEDS;

	// animation section
	const RGBLED_t centerLED = { 0xFF, 0x0F, 0x00 }; // default color
	const RGBLED_t adjuctLED = { 0xFF, 0xFF, 0x00 }; // default color
	const RGBLED_t farLED = { 0x0F, 0xFF, 0x00 }; // default color


	const RGBLED_t offLED = { 0, 0, 0 };

	unsigned int i = 0;

	for(i = 0 ;i < NUMBER_OF_LEDS; i++)
	{
		if(i == sliderValue)
		{
			RGBLEDS[i] = centerLED;
		}
		else if ( ( (i - sliderValue) == 1)  || ((sliderValue - i) == 1) )
		{
			RGBLEDS[i] = adjuctLED;
		}
		else if ( ( (i - sliderValue) == 2)  || ((sliderValue - i) == 2) )
		{
			RGBLEDS[i] = farLED;
		}
		else
		{
			RGBLEDS[i] = offLED;
		}

	}

	sendRGB();

}

void SetSoundBarrValue(int soundBarValue)
{
	if (soundBarValue >= NUMBER_OF_LEDS)
		soundBarValue = NUMBER_OF_LEDS;

	// animation section
	const RGBLED_t greenLED = { 0x10, 0xFF, 0x20 }; // Green color
	const RGBLED_t yellowLED = { 0xFF, 0xFF, 0x10 }; // Yellow color
	const RGBLED_t redLED = { 0xFF, 0x00, 0x00 }; // Red color
	const RGBLED_t offLED = { 0, 0, 0 };

	unsigned int i = 0;

	for(i = 0 ;i < NUMBER_OF_LEDS; i++)
	{
		if(i < soundBarValue)
		{
			RGBLEDS[i] = (i < 10) ? greenLED : yellowLED;
		}
		else if (i == soundBarValue)
		{
			RGBLEDS[i] = redLED;
		}
		else
		{
			RGBLEDS[i] = offLED;
		}

	}

	sendRGB();

}

void SetSphereValue(int sphereValue)
{
	if (sphereValue >= NUMBER_OF_LEDS / 3)
		sphereValue = NUMBER_OF_LEDS /3;

	// animation section
	const RGBLED_t greenLED = { 0x10, 0xFF, 0x20 }; // Green color
	const RGBLED_t yellowLED = { 0xFF, 0xFF, 0x10 }; // Yellow color
	const RGBLED_t redLED = { 0xFF, 0x00, 0x00 }; // Red color
	const RGBLED_t offLED = { 0, 0, 0 };

	unsigned int i = 0;

	for(i = 0 ;i < NUMBER_OF_LEDS/3; i++)
	{
		if(i < sphereValue)
		{
			RGBLEDS[i] = (i < 5) ? greenLED : yellowLED;
			RGBLEDS[i+10] = (i < 5) ? greenLED : yellowLED;
			RGBLEDS[i+20] = (i < 5) ? greenLED : yellowLED;

		}
		else if (i == sphereValue)
		{
			RGBLEDS[i] = redLED;
			RGBLEDS[i+10] = redLED;
			RGBLEDS[i+20] = redLED;
		}
		else
		{
			RGBLEDS[i] = offLED;
			RGBLEDS[i+10] = offLED;
			RGBLEDS[i+20] = offLED;
		}

	}

	sendRGB();

}

void SetAllColor(RGBLED_t color ) {

	int i = 0;
	for (i = 0; i <NUMBER_OF_LEDS; i++) {
		RGBLEDS[i] = color;
	}

	sendRGB();
}





//
//void SetStripColor (LEDPatterns_Color color) {
//	RGBLED_t LEDColor;
//	LEDColor.r = color.R;
//	LEDColor.g = color.G;
//	LEDColor.b = color.B;
//
//	unsigned int i = 0;
//	for(i = 0 ;i < NUMBER_OF_LEDS; i++)
//	{
//		RGBLEDS[i] = LEDColor;
//	}
//	sendRGB();
//}
//
//void SetLEDColors (LEDPatterns_led_t led) {
//	RGBLED_t LEDColor;
//
//	unsigned int i = 0;
//	for(i = 0 ;i < NUMBER_OF_LEDS; i++)
//	{
//		LEDColor.r = led[i].color.R;
//		LEDColor.g = led[i].color.G;
//		LEDColor.b = led[i].color.B;
//
//		RGBLEDS[i] = LEDColor;
//	}
//	sendRGB();
//}


void SetRolling (void) {

	static int rollerCount = 5;

	rollerCount ++;
	if(rollerCount == NUMBER_OF_LEDS)
		rollerCount = 0;

	// animation section
	const RGBLED_t onLED = { 0xFF, 0xFF, 0 }; // default color
	const RGBLED_t offLED = { 0, 0, 0 };

	unsigned int i = 0;

	for(i = 0 ;i < NUMBER_OF_LEDS; i++)
	{
		if(i == rollerCount)
		{
			RGBLEDS[i] = onLED;
		}
		else
		{
			RGBLEDS[i] = offLED;
		}

	}

	sendRGB();
}
