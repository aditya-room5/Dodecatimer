#include <msp430.h> 

#include <stdbool.h>
#include <stdint.h>

#include "LEDControl.h"

/*
 * ****************************************************************
 * Local Defines / Variables & Prototypes
 * ****************************************************************
 */

static bool patternRunOnce = false;

typedef enum Patterns {
	PATTERN_1 = 0,
	PATTERN_2,
	PATTERN_3,
	PATTERN_4,

	PATTERN_COUNT

}PATTERN;


void pattern1(void);
void pattern2(void);
void pattern3(void);
void pattern4(void);

void buzz(int time) ;

bool isThereNewData();

/*
 * ****************************************************************
 * Local Methods
 * ****************************************************************
 */

/* delay in mSec */
void delay(int num) {
	volatile int i = 0, j = 0;
	for (i = 0; i < num ; i++) {

		/* Polling as an alternative to interrupt  */
		isThereNewData();

		for (j = 0 ; j < 1430; j++) {
			;
		}
	}
}

void setupInputPins() {
	P1DIR |= BIT0;                            // Set P1.0 to output direction

	P2DIR |= BIT0;
	P2OUT &= ~ BIT0;
	P2REN |= BIT0;


	P1REN |= BIT3;                            // P1.4 pullup

	P1DIR &= ~BIT2;
	P1DIR &= ~BIT3;
	P1DIR &= ~BIT4;
	P1DIR &= ~BIT5;
}

/*
 * ****************************************************************
 * Main
 * ****************************************************************
 */

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	


    setupForLEDControl();

    setupInputPins();

	buzz(2); //announce we're awake!

	SetSliderValue(0);

    PATTERN pattern = PATTERN_1;

    patternRunOnce = false;


    while (1) {

    	if ( isThereNewData()  ) {
    		pattern = PATTERN_4;
    		patternRunOnce = true;
    	}

    		switch (pattern) {
			case PATTERN_1:
				pattern1();
				break;
			case PATTERN_2:
				pattern2();
				break;
			case PATTERN_3:
				pattern3();
				break;
			case PATTERN_4:
				pattern4();

				break;
			default:
				break;

			}



    }

	return 0;
}

/*
 * ****************************************************************
 * Methods
 * ****************************************************************
 */

void buzz(int time) {

	if (patternRunOnce) {
		patternRunOnce = false;

	P2OUT |= BIT0;

	delay(100);

	P2OUT &= ~ BIT0;
	}

}

void pattern1() {

	if (patternRunOnce) {
		patternRunOnce = false;

		int i = 0;
		for (i = 0; i < NUMBER_OF_LEDS; i ++ ) {
			SetHollowSliderValue(i);

			delay (100);
		}

		SetSliderValue(0);
	}
}



void pattern2() {

	if (patternRunOnce) {
		patternRunOnce = false;

		int i = 0;
		for (i = 0; i < NUMBER_OF_LEDS; i ++ ) {
			SetSliderValue(i);

			delay (100);
		}

		SetSliderValue(0);
	}
}

void pattern3() {

	if (patternRunOnce) {
		patternRunOnce = false;

		int i = 0;
		for (i = 0; i < NUMBER_OF_LEDS/3; i ++ ) {
			SetSphereValue(i);

			delay (1000);
		}

		SetSliderValue(0);
	}
}

void pattern4() {

	if (patternRunOnce) {
		patternRunOnce = false;

		RGBLED_t color = {0, 255, 0};
		int i = 0;
		for (i = 0; i < 25; i ++ ) {

			color.g = color.g - 10;
//			color.b = color.b + 10;
			SetAllColor(color);
			delay (500);
		}

		SetSliderValue(0);
	}
}

bool isThereNewData() {

	if((P1IN & BIT4)) {
		return true;
	}
	else {
		return false;
	}



}


