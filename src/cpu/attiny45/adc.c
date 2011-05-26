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

//The value from the adc conversion
static unsigned int adcValue;

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


/*! Disable digital input buffer to reduce power consumption
 * Disable the ADC pins which a potentiometer is connected to.
 * ADC0-ADC3 are defined as YES or NO (1 or 0)
 */
 /** \todo Decide if this is going to be moved to pm.c */
void adcDigitalInputDisable(void)
{
	DIDR0 = (ADC3 << ADC3D) |	//TODO: ADCx are not nice macros! :/
	        (ADC2 << ADC2D) |
	        (ADC1 << ADC1D) |
	        (ADC0 << ADC0D);
}

/** \brief Return the last ADC reading
 * \return adcValue, the last read ADC value */
int adcGetValue(void)
{
	return adcValue;
}

void adcStartConversion(void)
{
	//Make sure the ADC peripheral is on
	adcEnable();

	//Start the conversion
	ADCSRA |= (1 << ADSC);
}


ISR(ADC_vect)
{
	//Read the sampled ADC value
	adcValue = ADCL | (ADCH << 8);

	//The ADC is done for now, disable it to save power.
	adcDisable();

	//Set the timer1 frequency with the fresh adcValue.
	timer1SetFreq(adcGetValue());	//TODO: timer1SetFreq(adcValue);
}
