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

#include "drivers/pot.h"

/** \file pot.c Potentiometer driver */

/*************************************************************************//**
 * \brief Globals
 ****************************************************************************/
static volatile  char jitterReductionValue;
static volatile  char jitterReductionInc;

 /*************************************************************************//**
 * \brief jitter reduction
 *
 * \see timer1SetFreq()
 * \see ISR(ADC_vect)
 * \todo Make the jitter reduction work better. Averaging of ADC values is
 * suggested. Averaging implemented adcStartConversions()
 ****************************************************************************/
void jitterReduction(void)
{
	//Reduce jitter from ADC by adding +1 to the jitterReductionValue until it reaches the JITTER_REDUCTION_VALUE_MAX
	if(jitterReductionInc++ >= JITTER_REDUCTION_INC_MAX) {			//Make the jitter reduction a bit slower, allow some jitter
		jitterReductionInc = 0;										//Reset the delay value
		if(jitterReductionValue < JITTER_REDUCTION_VALUE_MAX) { 	//Should we increase the jitterReductionValue more?
			jitterReductionValue++;									//ok, increase it
		}
	}
}

/** \brief return the jitter reduction value */
char potGetJitterReductionValue(void)
{
	return jitterReductionValue;
}

/** \brief set the jitter reduction value */
void potSetJitterReductionValue(unsigned char value)
{
	jitterReductionValue = value;
}


unsigned int potGetValue(void)
{

		//Timer0 potentially woke us up. Do some of the upkeep that timer0 should do.
		//Get a new ADC value from the pot as the average from 5 samples
		//adcStartConversions(5);
		adcStartConversion();

		//Run function to reduce jitter from ADC, incombination with the timer1SetFreq() function
		jitterReduction();

		return adcGetValue();
}

unsigned int potGetAverageValue(char nrOfConversions)
{

	adcStartConversions(nrOfConversions);
	jitterReduction();

	return adcGetAverageValue();
}
