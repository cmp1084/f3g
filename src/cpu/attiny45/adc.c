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

#include "adc.h"

/** \file adc.c
 * Analog to digital converter */

/** The value from the adc conversion
 *
 * The value is updated from the interrupt, ISR(ADC_vect)
 */
static volatile unsigned int adcValue;

/** Average value from ADC conversion
 *
 * This value is updated by adcStartConversions()
 */
static volatile unsigned int adcAverageValue;

/*! \brief ADC initialization
 * Initializes ADC, then power it off.
 */
void adcInit(void)
{
	pmAdcPowerOn();
	ADMUX = (0 << REFS2) | (0 << REFS1) << (0 << REFS0) |
	        (0 << ADLAR) |
	        (0 << MUX3) | (0 << MUX2) |(1 << MUX1) |(0 << MUX0);	//ADC2 on PB4

	ADCSRA = (0 << ADEN) | 		//Dont enable ADC yet
	         (0 << ADSC) | 		//Dont start conversion yet
	         (0 << ADATE) |		//Dont autotrigger
	         (1 << ADIE) |		//Enable ADC interrupts
	         (0 << ADPS2) | (0 << ADPS1) | (1 << ADPS0);	//ADC prescale clk(sys) / 2, keep  50 kHz < F(adc) < 200 kHz for precision measurement. Below 1 MHz for crude measurement.

	ADCSRB = (0 << ADTS2) | (0 << ADTS1) | (0 << ADTS0);	//Select autotrigger source = freerunning if ADCSRA.ATATE = 1
	pmAdcPowerOff();
}

/*! ADC enable
 */
static void adcEnable(void)
{
	//Make sure the ADC peripheral is powered
	pmAdcPowerOn();
	//Make sure the ADC IRQ is allowed
	ADCSRA |= (1 << ADIE);					//TODO: IE and EN in one go.
	//Turn the ADC peripheral on
	ADCSRA |= (1 << ADEN);
}

/*! ADC halt
 * Halt the ADC and power the ADC unit off to save power
 */
static void adcHalt(void)
{
	ADCSRA &= ~(1 << ADEN);
	pmAdcPowerOff();
}

/*! is ADC running?
 * Check if a ADC conversion is running
 *
 * Return values:
 * TRUE if a conversion is currently running
 * FALSE if no conversion is currently running
 */
int isAdcRunning(void)
{
	//Return TRUE if ADSC bit in ADCSRA is set (= conversion is running)
	//Return FALSE if the conversion is done.
	return (ADCSRA & (1 << ADSC));
}

void adcWaitUntilConversionDone(void)
{
	while(isAdcRunning());
}

void adcDisable(void)
{
	adcWaitUntilConversionDone();
	adcHalt();
}

/** \brief Return the last ADC reading
 * \return adcValue, the last read ADC value */
unsigned int adcGetValue(void)
{
	return adcValue;
}

/** \brief Return the last averaged ADC reading
 * \return adcAverageValue, the last read averaged ADC value */
unsigned int adcGetAverageValue(void)
{
	return adcAverageValue;
}

/** \brief Start a single ADC conversion.
 *
 * Start a single ADC conversion. When the conversion is done, an ADC interrupt
 * occur. The result from the conversion is written to the global variable
 * #adcValue.
 *
 * The MCU is kept in running mode.
 * \see adcStartConversions()
 * \return Nothing is returned
 */
void adcStartConversion(void)
{
	//Make sure the ADC peripheral is on
	adcEnable();
	adcWaitUntilConversionDone();
	//Start the conversion, stay in running mode
	ADCSRA |= (1 << ADSC);
	//TODO: Sleep here until ADC conversion done. Waste of flash?
	//~ pmSleep(); //TODO: pmSleep() disables ADC!
	adcWaitUntilConversionDone();	//TODO: This is not optimal for sinle conversions. But necessary to optimize reading times when reading several consecutive ADC values with adcStartConversions()

	adcDisable();
}

/** \brief Start several consequtive ADC convertions and put the average in adcAverageValue
 *
 * 'nrOfConversions' consequtive ADC conversions are started. The average value
 * is put into the variable #adcAverageValue when done.
 * This functions utilizes the ADC Noise Reduction sleep mode.
 *
 * \see adcStartConversion()
 *
 * \param nrOfConversions The number of conversions that the ADC should do before averaging their value.
 *
 * \bug Bigger nrOfConversion values than 0x40 can lead to overflow in the unsigned int total. Quick fix applied. No warnings given.
 */
void adcStartConversions(char nrOfConversions)
{
	char conversion;
	unsigned int total = 0;
	//~ char wasTimer0IrqOn, wasTimer1IrqOn, wasGpioIrqOn;

	//Quick fix of 'total' overflow bug
	nrOfConversions &= 0x3f;



	//IRQs will be disabled to allow ADC conversion to run as noise free as possible.
	//First remember which IRQs we want to reenable later
	//~ wasTimer0IrqOn = isTimer0IrqOn();
	//~ wasTimer1IrqOn = isTimer1IrqOn();
	//~ wasGpioIrqOn = isGpioIrqOn();

	//Wait until we are allowed to use the ADC //TODO: Mutex, RTOS and all that.
		adcWaitUntilConversionDone();

	//Start the conversion, enter ADC Noise Reduction sleep mode
	for(conversion = 1; conversion <= nrOfConversions; conversion++) {
		//Disable selected IRQs
		//~ timer0IrqOff();	//Unnecessary, clkIO is not running, clkTM is not generated.
		//~ timer1IrqOff();
		//~ gpioIrqOff();

		adcEnable();	//TODO: We should not have to enable ADC, pmSleepAdcNoiseReduction have to do it, so just let it.
		//Start the ADC conversion
		pmSleepAdcNoiseReduction();	//WARNING TIMER CLKIO IS STOPPED!!! o_O

		//If we come here due to other interrupts, we have to wait before
		//continuing,  else the ADC value might be read too soon.
		//Only these IRQs can wake up the MCU from ADC Noise
		//Reduction mode:
		//ADC Conversion Complete interrupt, External Reset, Watchdog Reset,
		//Brown-out Reset, SPM/EEPROM ready interrupt, external level interrupt
		//on INT0 or a pin change interrupt.
		adcWaitUntilConversionDone();
		//Execution continue here after 25 cycles for the first conversion,
		//after 13 cycles for the rest of the conversion
		//No other interrupts may occur,
		total += adcGetValue();	//TODO: adcGetValue()
		//Update the global adcAverageValue every loop if someone want to use
		//it even before we are completely done with the averaging series.
		adcAverageValue = total / conversion;

		//Reenable IRQs
		//Allow anyone who want to interrupt to do so now.
		//This is done every loop to allow better responsiveness from the system
		//at long conversion series.
	//~ if(wasTimer0IrqOn == TRUE) { timer0IrqOn(); }
	//~ if(wasTimer1IrqOn == TRUE) { timer1IrqOn(); }
	//~ if(wasGpioIrqOn   == TRUE) { gpioIrqOn();   }
	//~ }
	}
	adcDisable();
}

/** \brief ADC interrupt service routine
 *
 * Read the sampled ADC value.
 */
ISR(ADC_vect)
{
	//Read the sampled ADC value
	adcValue = ADCL | (ADCH << 8);

	//TODO: Dont disable ADC, since we are likely to read it again soon.
	//The ADC is done for now, disable it to save power.
	//adcDisable();

	//Set the timer1 frequency with the fresh adcValue.
	//~ timer1SetFreq(adcGetValue());	//TODO: timer1SetFreq(adcValue);

	//If someone tried to start a sleep sequence, but didnt make it to
	//asm("sleep") before ADC interrupt occured, we help them to stay awake
	//by not allowing the MCU to sleep.
	MCUCR &= ~(1 << SE);
}
