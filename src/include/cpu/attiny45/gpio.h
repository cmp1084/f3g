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

#ifndef __GPIO_H__
#define __GPIO_H__

//~ #include <avr/io.h>
//~ #include <avr/interrupt.h>
//~ #include "pm.h"
#include "config.h"
//~ #include "system.h"

char gpioGet(const char pin);
void gpioInit(const char value);
void gpioToggle(const char pin);
void gpioOn(const char pin);
void gpioOff(const char pin);

//TODO: Not used? remove
void gpioPcint0Init(void);
void gpioIrqOn(void);
void gpioIrqOff(void);
int isGpioIrqOn(void);

#endif /* __GPIO_H__ */
