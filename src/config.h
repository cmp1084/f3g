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

/////////////////////////////////////////////////////////////////////////////
//General defines
/////////////////////////////////////////////////////////////////////////////
#define YES 1
#define NO 0

/** \def FCPU */
/** The system clock speed */
/** \todo Make this a variable instead, due to the future useage of CLKPR, see datasheet */
#define FCPU 8000000UL

/** \def LEDPIN *//** If a LED is present, this macro should be used  */
#define LEDPIN PB0	/// \todo: Set me to PB5, make PB5 IO

/////////////////////////////////////////////////////////////////////////////
//ADC defines
/////////////////////////////////////////////////////////////////////////////
/** \def ADC0
 * Set to YES if this ADC channel is going to be used */
/** \def ADC1
 * Set to YES if this ADC channel is going to be used */
/** \def ADC2
 * Set to YES if this ADC channel is going to be used */
/** \def ADC3
 * Set to YES if this ADC channel is going to be used */
#define ADC0 NO
#define ADC1 NO
#define ADC2 YES
#define ADC3 NO

/** \def JITTERREDUCTIONVALUEMAX
 * Used to minimize jitter from the pot. jitterReductionValue is used in the timer1SetFreq() function.
 * \see timer1SetFreq()
 * \todo Remake the jitter reduction functions */
/** \def JITTERREDUCTIONINCMAX
 * Used to minimize jitter from the pot. */
#define JITTERREDUCTIONVALUEMAX 0x08
#define JITTERREDUCTIONINCMAX 0x80

#endif /* __CONFIG_H__ */
