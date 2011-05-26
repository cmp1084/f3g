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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "adc.h"
#include "config.h"
#include "pm.h"
#include "timer.h"

#include "drivers/led.h"

PROGMEM const char programname[] = "F3G v0.5 (" __DATE__ ")";

int main(void)
{
	//Throttle system clock to 8 MHz
	pmClkPrescale(0);

	//Reduce power consumption
	pmPowerReduction();

	//Initialize GPIOs, LEDs, Buttons etc. if necessary
	//~ gpioOn(PB0);	//Turn debug LED off
	//~ gpioOn(PB1);
	//~ gpioOn(PB2);
	//~ gpioInit((1 << PB0) | (1 << PB1)); //TODO: Note, PB1 must be set to output OC1A signal. This is also done by timer1init()

	ledOff();
	ledInit();

	//ADC init
	adcInit();

	//Initialize timers and PLL
	timer0Init();
	timer1PllInit();
	timer1Init();

	//Init PCINT0 interrupt from GPIO lines (used for buttons)
	gpioPcint0Init();

	//Initialization done. Start interrupts
	sei();
	while(1) {
		//Put MCU in idle sleep mode
		pmSleep();

		if(isButtonPressed() == YES) {
			timer1TogglePrescale();
			while(isButtonPressed()) {
				pmSleep();
			}
		}
	};
}

