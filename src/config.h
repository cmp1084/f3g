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

#ifndef __CONFIG_H__
#define __CONFIG_H__

/*! \file config.h
 *  \brief Global configuration file
 *
 * Contains various #defines etc which are used in the code */

#include "avr/io.h"
#include "system.h"

/////////////////////////////////////////////////////////////////////////////
//General defines
/////////////////////////////////////////////////////////////////////////////
#define YES 1
#define NO 0
#define TRUE 1
#define FALSE 0

/////////////////////////////////////////////////////////////////////////////
// System defines
/////////////////////////////////////////////////////////////////////////////
/** \def FCPU */
/** The system clock speed, without prescaling */
/** Internal RC osc is 8 MHz. If external crystal is used this value must be
 * updated */
/** \todo Make this a variable instead, due to the future usage of CLKPR, see datasheet */
#define SYSCLK (8000000UL)


/////////////////////////////////////////////////////////////////////////////
// Button and LED defines
/////////////////////////////////////////////////////////////////////////////
/** \def BUTTON_PIN */
#define BUTTON_PIN PB2
#define BUTTON_DEBOUNCE_PERIOD 2	//This value represent the debounce strenght on the button. Higher value gives slower responsiveness from the button. Lower value give less debounceing.

/** \def LEDPIN *//** If a LED is present, this macro should be used  */
#define LEDPIN PB0	/// \todo: Set me to PB5, make PB5 IO

/////////////////////////////////////////////////////////////////////////////
//ADC defines
/////////////////////////////////////////////////////////////////////////////
/** \def USE_ADC0
 * Set to YES if this ADC channel is going to be used */
/** \def USE_ADC1
 * Set to YES if this ADC channel is going to be used */
/** \def USE_ADC2
 * Set to YES if this ADC channel is going to be used */
/** \def USE_ADC3
 * Set to YES if this ADC channel is going to be used */
#define USE_ADC0 NO
#define USE_ADC1 NO
#define USE_ADC2 YES
#define USE_ADC3 NO

/** \def JITTER_REDUCTION_VALUE_MAX
 * Used to minimize jitter from the pot. jitterReductionValue is used in the timer1SetFreq() function.
 * \see timer1SetFreq()
 * \see pot.c
 * \todo Remake the jitter reduction functions */
/** \def JITTER_REDUCTION_INC_MAX
 * Used to minimize jitter from the pot. */
#define JITTER_REDUCTION_VALUE_MAX 0x06
#define JITTER_REDUCTION_INC_MAX 0x10

#endif /* __CONFIG_H__ */
