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
 * General purpose input output */

//~ unsigned int gpiobuttonPressedFlag;


char gpioGet(const char pin)
{
	return PINB & (1 << pin);
}

void gpioInit(const char value)
{
	DDRB |= value;
}

/** \brief toggle a GPIO pin
 *
 * \todo cli()/sei() only necessary if IRQs are allowed to operate on the GPIO pins.
 */
void gpioToggle(const char pin)
{
	interruptDisable();
	PORTB ^= (1 << pin);
	interruptEnable();
}

void gpioOn(const char pin)
{
	interruptDisable();
	PORTB |= (1 << pin);
	interruptEnable();
}

void gpioOff(const char pin)
{
	interruptDisable();
	PORTB &= ~(1 << pin);
	interruptEnable();
}

void gpioIrqOn(void)
{
	GIMSK |= (1 << PCIE);
}

void gpioIrqOff(void)
{
	GIMSK &= ~(1 << PCIE);
}

__attribute__((always_inline))
int isGpioIrqOn(void)
{
	return GIMSK & (1 << PCIE);
}
//TODO: If necessary
//~ int gpioInput(void)
//~ {
//~
//~ }

//~ void gpioCleanInput(void)
//~ {
	//~ gpiobuttonPressedFlag = 0;
//~ }

//~ #define PCINTPIN PB2
//~
//~ /** \brief Initialize the PCINT0 interrupt
 //~ *
 //~ * \return Nothing */
//~ /** \todo Make this function nicer */
//~ void gpioPcint0Init(void)
//~ {
	//~ //GIMSK |= (1 << PCIE);			//Enable Pin Change interrupt
	//~ gpioIrqOn();
	//~ PCMSK |= (1 << PCINT2);			//for PCINT2
	//~ DDRB &= ~(1 << PCINTPIN);		//Make the pin input
	//~ PORTB |= (1 << PCINTPIN);		//with internal pullup
//~ }


/** \todo Contact switch bouncing causes some problems. The problem should not be solved here, but elsewhere
 * infact, never connect a button input to an interrupt, due to the contact bouncing
 * Sort the button out in the timer isr instead */
//~ ISR(PCINT0_vect)
//~ {
	//Just wake up


	//~ //Detect button down
	//~ if(!(PINB & (1 << PCINTPIN))) {
		//~ buttonPressedFlag = TRUE;
		//~ ledOn();
	//~ } else {
		//~ buttonPressedFlag = FALSE;
		//~ ledOff();		//ledOff, usually
	//~ }
//~ }

//~ ISR(PCINT0_vect)
//~ {
	//~ int thisSystick;
	//~ //Detect button down
	//~ if(!(PINB & (1 << PCINTPIN))) {
		//~
		//~ if(!buttonPressedStillBouncingFlag) {
			//~ buttonPressedAtSystick = getSystick();
			//~ buttonPressedStillBouncingFlag = TRUE;
		//~ }
		//~ ledOn();
	//~ } else {
		//~ if(buttonPressedAtSystick < (getSystick() - BUTTON_DEBOUNCE_PERIOD)) {
			//~ buttonPressedStillBouncingFlag = FALSE;
		//~ }
		//~ ledOff();		//ledOff, usually
	//~ }
//~ }
