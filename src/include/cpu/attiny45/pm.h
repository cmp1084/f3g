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

#ifndef __PM_H__
#define __PM_H__

//~ #include <avr/io.h>
//~ #include <avr/interrupt.h>

#include "adc.h"
#include "config.h"

void pmPowerReduction(void);
void pmSleep();
void pmSleepAdcNoiseReduction(void);
void pmAdcPowerOn(void);
void pmAdcPowerOff(void);
void pmClkPrescale(unsigned char prescale);
void pmClkThrottle(unsigned long desiredfcpu);

#endif /* __PM_H__ */
