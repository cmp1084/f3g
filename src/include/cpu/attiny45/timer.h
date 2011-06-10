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

#ifndef __TIMER_H__
#define __TIMER_H__

//~ #include <avr/io.h>
#include <avr/interrupt.h>

//~ #include "adc.h"
#include "drivers/button.h"
#include "config.h"
//~ #include "drivers/led.h"

void timer0Init(void);
void timer1Init(void);
void timer0IrqOn(void);
void timer0IrqOff(void);
void timer1IrqOn(void);
void timer1IrqOff(void);
int isTimer0IrqOn(void);
int isTimer1IrqOn(void);
void timer1PllInit(void);
char timer1SetFreq(unsigned int value);
void timer1TogglePrescale(void);

#endif /* __TIMER_H__  */
