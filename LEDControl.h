/*
 * LEDControl.h
 *
 *  Created on: Mar 20, 2014
 *      Author: aditya.bhutada
 */

#ifndef LEDCONTROL_H_
#define LEDCONTROL_H_

#define NUMBER_OF_LEDS 		30


typedef unsigned char u_char;
typedef unsigned int u_int;

typedef struct {
	u_char r;
	u_char g;
	u_char b;
} RGBLED_t;

void setupForLEDControl (void);
void sendRGB(void);

void SetLEDPattern(int odd);

void SetSliderValue(int sliderValue);

void SetHollowSliderValue(int sliderValue);

void SetSoundBarrValue(int soundBarValue);

//void SetStripColor (LEDPatterns_Color color) ;
//
//void SetLEDColors (LEDPatterns_led_t led);

void SetSphereValue(int sphereValue);

void SetAllColor(RGBLED_t color );

void SetRolling (void);

#endif /* LEDCONTROL_H_ */
