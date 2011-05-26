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

#include "gpio.h"

/** \file gpio.c
 * General purpose input output

//~ unsigned int gpioButtonPressed;

/*! \brief Flag to see if a button is pressed or not */
/*! Legal values are YES and NO */
/*! \see ISR(PCINT0_vect) */
volatile unsigned int buttonPressed;	//variable that hold YES or NO

/*! \brief is the button pressed? */
/*! \return YES or NO */
/* \todo Rename to isPCINTButtonPressed() or similar, if both GPIO buttons and
 * PCINT buttons are used in future versions */
int isButtonPressed(void)
{
	return buttonPressed;
}

void gpioInit(const char value)
{
	DDRB |= value;
}

void gpioToggle(const char pin)
{
	cli();
	PORTB ^= (1 << pin);
	sei();
}

void gpioOn(const char pin)
{
	cli();
	PORTB |= (1 << pin);
	sei();
}

void gpioOff(const char pin)
{
	cli();
	PORTB &= ~(1 << pin);
	sei();
}

//TODO: If necessary
//~ int gpioInput(void)
//~ {
//~
//~ }

//~ void gpioCleanInput(void)
//~ {
	//~ gpioButtonPressed = 0;
//~ }

/** \brief Initialize the PCINT0 interrupt
 *
 * \return Nothing */
/** \todo Make this function nicer */
#define PCINTPIN PB2
void gpioPcint0Init(void)
{
	GIMSK |= (1 << PCIE);			//Enable Pin Change interrupt
	PCMSK |= (1 << PCINT2);			//for PCINT2
	DDRB &= ~(1 << PCINTPIN);		//Make the pin input
	PORTB |= (1 << PCINTPIN);		//with internal pullup
}

/** \todo Contact switch bouncing cases some problems here */
ISR(PCINT0_vect)
{
	//Detect button down
	if(!(PINB & (1 << PCINTPIN))) {
		//Set the buttonPressed  flag
		buttonPressed = YES;
		ledToggle();	//ledOn, usually
		return;
	}
	buttonPressed = NO;
	ledToggle();		//ledOff, usually
}
