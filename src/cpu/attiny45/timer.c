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

#include "timer.h"

#include "adc.h"
#include "drivers/led.h"
#include "config.h"


/** \file timer.c */

/*************************************************************************//**
 * \brief Globals
 ****************************************************************************/
volatile unsigned char jitterReductionValue;
volatile unsigned char jitterReductionInc;

/*! systick is increased from timer0 ovf interrupt
 * \see ISR(TIMER0_OVF_vect) */
volatile unsigned int systick;	//System time keeping

/*****************************************************************************
 * General timer and relative functions
 ****************************************************************************/
 unsigned int getSystick(void)
 {
	 return systick;
 }

 /*************************************************************************//**
 * \brief jitter reduction
 *
 * \see timer1SetFreq()
 * \see ISR(ADC_vect)
 * \todo Make the jitter reduction work better. Averaging of ADC values is
 * suggested.
 ****************************************************************************/
static void jitterReduction(void)
{
	//Reduce jitter from ADC by adding +1 to the jitterReductionValue until it reaches the JITTERREDUCTIONVALUEMAX
	if(jitterReductionInc++ < JITTERREDUCTIONINCMAX) {
		jitterReductionInc = 0;
		if(jitterReductionValue < JITTERREDUCTIONVALUEMAX) {
			jitterReductionValue++;
		}
	}
}

/*************************************************************************//**
 * Timer0
 * Timer0 will be used for general timing activities,
 * waking up from sleep mode to see if the ADC need to be read etc.
 ****************************************************************************/

 /*************************************************************************//**
 * \brief Timer0 init
 *
 * Timer0 will be used for general timing activities,
 * waking up from sleep mode to see if the ADC need to be read etc.
 ****************************************************************************/
void timer0Init(void)
{
	//General timer/counter control register
	//TMS - timer synchronization mode
	//PSR0 - prescaler reset timer0
	//GTCCR = (0 << TMS) | (0 << PSR0);

	//Timer interrupt mask
	TIMSK |= (1 << TOIE0);						//Allow overflow0 interrupt

	//OCR0A, set to highest value to get slowest OVF0 frequency.
	OCR0A = 0xff;

	//Timer/counter control register 1 A
	TCCR0A = (0 << COM0A1) | (0 << COM0A0) |	//oc0a disconnected
	         (0 << COM0B1) | (0 << COM0B0) |	//oc0b disconnected
	         (0 << WGM01)  | (1 << WGM00);		//Phase corrected PWM wgm with tccr0b.wgm02
												//Phase corrected PWM with OCR0A = 0xff give the
												//Slowest OVF0 frequency.

	//Timer/counter control register 1 B
	TCCR0B = (1 << WGM02) |						//with tccr0a.wgm0x
	         (1 << CS02) |
	         (0 << CS01) |
	         (0 << CS00);						//Prescale clk(io) / 256
}

/*****************************************************************************
 * Timer1
 *
 * \brief Timer 1 will be used to generate the high frequency functions on PB1
 * (OC1A)
 ****************************************************************************/


/*! \brief timer1 set frequency
 *
 * Use jitterReduction() from the ISR to reduce jitter from poor potentiometer/ADC value reading
 *
 * \param value - the value of OCR1C. OCR1A = OCR1C / 2.
 * \see ISR(ADC_vect)
 * \todo Make the response from the ADC reading that come into this function logarithmic somehow.
 * To allow smaller values of 'value'
 */
int timer1SetFreq(const unsigned int value)
{
	if((OCR1C > ((value >> 2) + jitterReductionValue) || (OCR1C < ((value >> 2) - jitterReductionValue)))) {
		OCR1C = (value >> 2); 						//Throw away 2 LSB from the value. //TODO: ADLAR is more clever.
		OCR1A = (value >> 3);						//50/50 duty cycle. TODO: Get this value from potentiometer.
		jitterReductionValue = 1;
		return 1;								//ok, OCR1x values were changed
	}
	return 0;									//OCR1x values were not changed
}

/*!
 * \brief timer1 toggle prescale
 * Toggle the prescale in a cyclic fascion, 2^N, where N = {0..14}
 */
void timer1TogglePrescale(void)
{
	char nextPrescale, tccr1value;

	nextPrescale = (TCCR1 + 1) & 0x0f;			//Calc next prescale value from the current value.
	if(nextPrescale == 0x00) {					//0 equals timer1 off. We dont want that.
		nextPrescale = 1;						//1 equals PCK / 2.
	}
	tccr1value = TCCR1 & 0xf0;
	TCCR1 = tccr1value | nextPrescale;
	return;
}

/** \brief Initialize the PLL of timer 1 to run in PWM mode
 */
 /** \todo Timeout for the wait loop */
void timer1PllInit(void)
{
	unsigned int i;

	//Initiate PLL
	PLLCSR = (1 << PLLE);

	//Wait 100 us for PLL to stabilise
	/** \def PLLWAITTIME
	 * The required waittime before the PLL is stable */
	#define PLLWAITTIME 0.000100 * FCPU
	for(i = 0; i <  PLLWAITTIME; i++) {
		asm volatile ("nop");
	}

	//Wait for the PLL to lock
	while(!(PLLCSR && (1 << PLOCK)));	//TODO: Activate wdt before this wait, or make timeout

	//Enable PLL
	PLLCSR = (1 << PCKE) | (1 << PLLE);

}

/** \brief Initialize timer1 to run in PWM mode
 *
 * OCR1A output must be enabled, it is this functions responsibility to do that.
 * Initial OCR1C and OCR1A values are not important, as they are soon set
 * from the ADC reading.
 */
 /** \todo Get default values from EEPROM
 */
void timer1Init(void)
{
	//Allow OCR1A signal to be output on PB1
	DDRB |= (1 << PB1);

	//General timer/counter control register, also see timer0Init()
	//GTCCR = 0;

	//OCR1C, set at top value
	OCR1C = 0xff;
	//OCR1A compare register
	OCR1A = OCR1C >> 1;												//intial 50/50 duty cycle, hardcoded TODO: ADC value or EEPROM preset

	//~ TIMSK |= (1 << TOIE1); // | (1 << OCIE1A);					//Overflow and output compare interrupts enabled

	//Timer/counter control register 1
	TCCR1 = (1 << COM1A1) | (1 << COM1A0) |							//OC1A set on compare match. Cleared on TCN1 = 0x00 (/OC1A not connected)
	        (0 << CS13) |(0 << CS12) |(0 << CS11) | (1 << CS10) |	// PCK / 2	 TODO: ADC value or EEPROM
	        (1 << PWM1A);											//Enable PWM

}

/*!
 ****************************************************************************
 * Interrupt Service Routines
 ****************************************************************************
 */
ISR(TIM0_OVF_vect)
{
	//Reset watchdog timer
	//asm volatile("wdr");		//TODO: add WDT module

	//Get a new value from the pot.
	adcStartConversion();

	//Indicate we are alive
	//~ ledToggle();

	//Timekeeping
	systick++;

	//Run function to reduce jitter from ADC, incombination with the timer1SetFreq() function
	jitterReduction();
}

//Not in use
//~ ISR(TIM1_OVF_vect)
//~ {
//~
//~ }

//Not in use
//~ ISR(TIM1_COMPA_vect)
//~ {
//~
//~ }
