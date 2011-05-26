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

#include "pm.h"

/** \file pm.c
 * Power management */

/** \brief Global power reduction
 *
 * Power reduction for all MCU peripherals that is not needed is handled here.
 * Power reduction for MCU peripherals that are used from time to time should be
 * handled elsewhere.
 *
 * \return Nothing
 */
 /** \todo Handle all powerReduction here. However, this make more code
  * execute everytime some peripheral is turned on/off */
void pmPowerReduction(void)
{
	//Do power reduction on ADC pins
	adcDigitalInputDisable();
	//Power reduction register. Turn off USI
	PRR = (0 << PRTIM1) |			//Keep the timers running
	      (0 << PRTIM0) |
	      (1 << PRUSI) |			//Power down the USI
	      (0 << PRADC);				//Do not power down the ADC before ADC
									//have been initialized (ADC power will
									//be toggled on/off during runtime)
}

/** \brief Idle sleep mode for power saving
 *
 * This function put the MCU in idle sleep mode.
 * ADC and timers are running during idle sleep mode.
 *
 * \return Nothing
 */
void pmSleep(void)
{
	//Make sure the ADC is disabled before entering sleep
	adcDisable();
	MCUCR |= (1 << SE) | (0 << SM1) | (0 << SM0);	//Idle sleep mode
	asm volatile("sleep");
}

/** \brief Turn ADC power on
 *
 * \return Nothing
 */
void pmAdcPowerOn(void)
{
	PRR &= ~(1 << PRADC);
}

/** \brief Turn ADC power on
 *
 * \return Nothing
 */
 /** \todo Verify if a conversion is running before powering off!
  * Make another function (or pass a param) that override waiting for a running converion. */
void pmAdcPowerOff(void)
{
	PRR |= (1 << PRADC);
}

/** \brief Prescale of the system clock
 *
 * This function act as a CPU throttle.
 * Prescaling is done on the internal 8 MHz RC oscillator,
 * if factory default fuse values are set.
 *
 * \param prescale the bit value to give a certain system clk prescale.
 *
 * \return Nothing */

 /** \todo Correct ADC and timer0 prescale to not affect these peripherals by
  * a faster system clk.
  * \todo Use a MHz frequency value as parameter instead.
  * Let the function find the closest prescale to give that CPU freq
  * and return the actual frequency set. */
void pmClkPrescale(unsigned char prescale)
{
	int interruptWasEnabled = 0;
	//Turn interrupt off
	//First look at the I bit in SREG to determine if global interrupt was enabled
	if(SREG & (1 << 7)) {
		interruptWasEnabled = 1;
	}
	cli();
	//This write sequency must be completed in four cycles
	CLKPR = (1 << CLKPCE);
	CLKPR = (0 << CLKPCE) | prescale; //(0 << CLKPS3);
	if(interruptWasEnabled) {
		sei();
	}
}
//Ooh, that code compile just beautifully @ -Os. No way to optimize. r24 is 0x00 from the callee
//~ 0000025e <pmClkPrescale>:
 //~ 25e:   2f b7           in      r18, 0x3f       ; 63
 //~ 260:   f8 94           cli
 //~ 262:   90 e8           ldi     r25, 0x80       ; 128
 //~ 264:   96 bd           out     0x26, r25       ; 38
 //~ 266:   86 bd           out     0x26, r24       ; 38
 //~ 268:   27 ff           sbrs    r18, 7
 //~ 26a:   01 c0           rjmp    .+2             ; 0x26e <pmClkPrescale+0x10>
 //~ 26c:   78 94           sei
 //~ 26e:   08 95           ret
