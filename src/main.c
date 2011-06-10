/*****************************************************************************
                                ___
                         __    |   |    __
                    _   |  |   |   |   |  |   _
                _._| |__|  |___|   |___|  |__| |_._

                 Fast Frequency Function Generator
                _ _   __    ___     ___    __   _ _
                 ' |_|  |  |   |   |   |  |  |_| '
                        |__|   |   |   |__|
                               |___|

     Copyright 2011 Marcus Jansson <mjansson256@yahoo.se>


     This is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/
/* Tab size: 4 */

//////////////////////////////////////////////////////////////////////////////
//
// Fast Frequency Function Generator using tiny45 PLL to achive 32 MHz signal.
// The generated function is limited to square wave with 50/50 duty cycle in
// this version.
//
/////////////////////////////////////////////////////////////////////////////
//
// AVR ATtiny45 stats:
// -------------------
// 8 MHz internal RC oscillator
// 64 MHz PLL
// 4 kB flash, 10.000 write/erase cycles
// 256 bytes EEPROM 100.000 write/erase cycles
// 256 bytes internal SRAM
// 5 I/O pins
//
/////////////////////////////////////////////////////////////////////////////

/** \file main.c main program */

//~ #include <avr/io.h>
//~ #include <avr/interrupt.h>
#include <avr/pgmspace.h>


#include "adc.h"
#include "config.h"
//#include "gpio.h"
#include "pm.h"
//~ #include "system.h"
#include "timer.h"


#include "drivers/button.h"
#include "drivers/led.h"
#include "drivers/pot.h"

#define FOREVER 1

PROGMEM const char programname[] = "F3G v0.5.1 (" __DATE__ ")";

//~ __attribute__((naked))
int main(void)
{
	unsigned int pav;

	//Throttle system clock to 8 MHz
	/** \bug Setting other system clock speeds affect the timer speed, naturally. Prescaling to compensate? */
	pmClkThrottle(SYSCLK);	//Throttle to 1 MHz system clock
	//~ pmClkThrottle(SYSCLK);	//Throttle to 8 MHz system clock

	//Reduce power consumption
	pmPowerReduction();

	//ADC init
	adcInit();

	//Initialize GPIOs, LEDs, Buttons etc. if necessary
	//Enable the pushbutton internal pull-up resistor
	buttonInit();

	//Init LED
	ledOff();
	ledInit();

	//Initialize timers and PLL
	timer0Init();
	timer1PllInit();
	timer1Init();

	//Init PCINT0 interrupt from GPIO lines (used for buttons)
	//~ gpioPcint0Init();

	//Initialization done. Allow interrupts.
	interruptEnable();
	do {
		//Put MCU in idle sleep mode
		pmSleep();

		//Detect single click
		if(isButtonPressed() == TRUE) {
			//timer1TogglePrescale();
			ledOn();
			//Sleep until button is released
			while(isButtonPressed()) {
				pmSleep();
			}
			ledOff();
		}
		//Test case for double click:
		if(isDoubleClick() == TRUE) {
			timer1TogglePrescale();
		}


		pav = potGetAverageValue(5);
		//~ pav = potGetValue();
		timer1SetFreq(pav);	//TODO: timer1SetFreq(adcValue);

	} while(FOREVER);
}

//Something woke the MCU up, was it a PCINT from the button? //TODO: PCINT not used any longer, we come here anyway after each timer0 irq
