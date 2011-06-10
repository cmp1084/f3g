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

/*! Disable digital input buffer to reduce power consumption
 *
 * Disable the ADC pins which a potentiometer is connected to.
 * ADC0-ADC3 are defined as YES or NO (1 or 0)
 */
 /** \todo Decide if this is going to be moved to pm.c */
static void pmDigitalInputDisable(void)
{
	DIDR0 = (USE_ADC3 << ADC3D) |	//TODO: ADCx are not nice macros! :/
	        (USE_ADC2 << ADC2D) |
	        (USE_ADC1 << ADC1D) |
	        (USE_ADC0 << ADC0D);
}

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
	//Enable pull-up on all pins in order to have a definite level
	//~ DDRB = 0x00;	//Unnecessary operation, Default value = 0x00
	PORTB = 0xff;	//Enable pull-up. Pull-up goes active after 1 clk.

	//Do power reduction on ADC pins
	pmDigitalInputDisable();

	//Power reduction register. Turn off USI
	PRR = (0 << PRTIM1) |			//Keep the timers running
	      (0 << PRTIM0) |
	      (1 << PRUSI) |			//Power down the USI
	      (1 << PRADC);				//Do not power down the ADC before ADC	//TODO: Verify that it is safe to power down ADC here (adcInit() fixed)
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
	//Make sure interrupts are enabled before sleeping
	//~ enableInterrupt();	//TODO: Let someone else take care of this decision.
	MCUCR &= ~((1 << SM1) | (1 << SM0));
	MCUCR |= (1 << SE) | (0 << SM1) | (0 << SM0);	//Idle sleep mode
	asm volatile("sleep");							//Zzz...
	MCUCR &= ~(1 << SE);
}

/** \brief Idle sleep mode for power saving
 *
 * This function put the MCU in idle sleep mode.
 * ADC peripheral is left in whatever its current state is.
 * ADC and timers are running during idle sleep mode.
 *
 * \see pmSleep()
 * \see pmSleepAdcNoiceReduction()
 * \return Nothing
 * \bug If a ADC interrupt occur before going to sleep. We will be sleeping for a long while. This can be fixed by letting the ADC IRQ set the SE-bit.
 */
void pmSleepAdcOn(void)
{
	MCUCR &= ~((1 << SM1) | (1 << SM0));
	MCUCR |= (1 << SE) | (0 << SM1) | (0 << SM0);	//Idle sleep mode
	asm volatile("sleep");							//Zzz...
	MCUCR &= ~(1 << SE);
}

/** \brief ADC Noise Reduction sleep mode for better precision in the ADC measurement
 *
 * This function put the MCU in ADC Noise Reduction sleep mode.
 * ADC conversion is started when entering this sleep mode.
 *
 * \see pmSleepAdcOn()
 * \return Nothing
 */
void pmSleepAdcNoiseReduction(void)
{
	//Make sure the ADC peripheral is turned on
	//~ adcEnable();	//TODO: This is done in the adcStartConversions() function
	MCUCR &= ~((1 << SM1) | (1 << SM0));
	MCUCR |= (1 << SE) | (0 << SM1) | (1 << SM0);	//ADC Noise Reduction sleep mode
	asm volatile("sleep");							//sleep and ADC conversion  start, wake up in max 25 cycles
	MCUCR &= ~(1 << SE);
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

 /** \todo Correct ADC prescale and timer0 prescale to not affect these peripherals by
  * a faster system clk.
  * \todo Use a MHz frequency value as parameter instead.
  * Let the function find the closest prescale to give that CPU freq
  * and return the actual frequency set.
  *
  * \param prescale
  * legal values [0..8]
  * Prescale	Clock division factor
  * 	0				1
  * 	1				2
  * 	2				4
  * 	3				8
  * 	4				16
  * 	5				32
  * 	6				64
  * 	7				128
  * 	8				256
  */
void pmClkPrescale(unsigned char prescale)
{
	int interruptWasEnabled = 0;

	//Dont do anything if we are fed with faulty prescaler
	if(prescale > 8) return;
	//{
	//	prescale = 8;															//TODO: Consider setting prescale to 8 and continue
	//}

	//Turn interrupt off
	//Look at the I bit (nr. 7) in SREG to determine if global interrupt was enabled
	if(SREG & (1 << 7)) {
		interruptWasEnabled = 1;
	}
	interruptDisable();
	//This write sequency must be completed in four cycles
	CLKPR = (1 << CLKPCE);
	CLKPR = (0 << CLKPCE) | (prescale); //(0 << CLKPS3);
	if(interruptWasEnabled) {
		interruptEnable();
	}
}
//Ooh, the pmClkPrescale code compile just beautifully @ -Os.
//No way to optimize afaik. r24 is 0x00 from the callee
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

/** \brief the actual frequency the system is running at.
 *
 * Accessed through the getcpu() function
 */
static volatile unsigned int fcpu;

/** \brief get the current frequency of the system
 *
 * \return fcpu, the current frequency, SYSCLK divided by a prescale value
 * \see pmClkThrottle()
 */
int pmGetfcpu(void)
{
	return fcpu;
}

/** \brief Set the frequency of the system clock
 *
 * The prescaler to use on SYSCLK is calculated.
 * The actual frequency is calculated and #fcpu is updated.
 *
 * \param desiredFreq is the desired frequency. */
void pmClkThrottle(unsigned long desiredFreq)
{
	int clkdivisionfactor;
	unsigned long actualFreq = SYSCLK;
	char prescale = 0x00-1;				//Start at -1 due to preincrease in do-while loop

	//Find the required prescale to get the desired frequency
	do  {
		++prescale;
		if(prescale > 7) break;	//too low desiredFreq, break out.
		clkdivisionfactor = (1 << (prescale));
	} while(desiredFreq > (SYSCLK / clkdivisionfactor));

	//Set the actual prescale to use
	pmClkPrescale(prescale);
	fcpu = SYSCLK / clkdivisionfactor;
}

//~ __attribute__((always_inline))	//Sucky sucky, the compiler just ignore this.
//~ void inline interruptEnable(void)
//~ {
	//~ asm volatile("sei");
//~ }

