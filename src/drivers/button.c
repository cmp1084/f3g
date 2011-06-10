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

/** \file button.c Button driver */

#include "drivers/button.h"


/** \def DOUBLE_CLICK_TIME_FRAME
 * The number of sysTicks that a double click must be registred within
 */
#define DOUBLE_CLICK_TIME_FRAME 0x10


/*! \brief Flag to see if a button is pressed or not */
/*! Legal values are YES and NO */
/*! \see ISR(PCINT0_vect) */
volatile unsigned char buttonPressedFlag;	//variable that hold TRUE or FALSE
volatile unsigned char doubleClickFlag;		//variable that hold TRUE or FALSE

/** \brief for how long have the button been pressed
 *
 * Used to debounce the button and determine a positive button press from
 * glitches
 * \see file button.c */
static unsigned int presscount;
static unsigned int clickAtTick;

//~ /** \todo consider changing order of the name to CLICK_FIRST, etc. */
//~ enum multiclick {
	//~ NO_CLICK = 0,
	//~ FIRST_CLICK = 1,
	//~ SECOND_CLICK,
	//~ THIRD_CLICK,
	//~ SINGLE_CLICK,
	//~ DOUBLE_CLICK,
	//~ TRIPPLE_CLICK,
//~ };


/** Get the raw state of the button
 * \return TRUE or FALSE
 */
char getButton(void)
{
	return !gpioGet(BUTTON_PIN);
}

/*! \brief is the button pressed? */
/*! \return YES or NO */
/*! \todo Rename to isPCINTbuttonPressed() or similar, if both GPIO buttons
 * and PCINT buttons are used in future versions */
char isButtonPressed(void)
{
	return buttonPressedFlag;
}

/** \brief set the doubleClickFlag to a value
 *
 * Legal values can be considered to be {TRUE, FALSE }
 */
static void doubleClickFlagSet(char value) {
	doubleClickFlag = value;
}

/** \brief Return the doubleClickFlag */
static char getDoubleClickFlag(void) {
	return doubleClickFlag;
}

/** \brief check if a double click have been registred */
char isDoubleClick(void)
{
	switch(getDoubleClickFlag()) {
		case TRUE:
			doubleClickFlagSet(FALSE);		//Reset this double click, since it have been read now.
			return TRUE;
			break;
		case FALSE:
			return FALSE;					//Do nothing, no double click have been registred.
			break;
		default:
			return getDoubleClickFlag();	//Someone set a strange value for doubleClickFlag, just return it and hope the callee know how to handle.
	}
}

/** \brief Evaluate if a double click have been performed. */
static void doubleClickEvaluate(void)
{
	unsigned int tick;
	tick = getSystick();
	if(tick < (clickAtTick + DOUBLE_CLICK_TIME_FRAME)) {
		//This click was within the time frame for a double click
		doubleClickFlagSet(TRUE);
		return;
	} else {
		//This click was too late to be a double click
		doubleClickFlagSet(FALSE);
	}
}

/** \brief basic debouncing of the button
 *
 * This function manages the debouncing of the button.
 * The function is called from ISR(TIMER0_OVF_vect)
 *
 * \todo: Optionally Add PCINT interrupt to get better responsiveness from the
 * button. Suggestion is to set presscount to BUTTON_DEBOUNCE_PERIOD / 2 or
 * similar in the PCINT ISR
 */
void updateButtonStatus(void)
{
	//Test if the button is pressed
	if(getButton()) {
		//The button might be pressed.
		if(presscount == BUTTON_DEBOUNCE_PERIOD) {
			//If this is the first time the button is registred as pressed, remember its 'clickAtTick'-time and evaluate possible doubleClick
			if(!buttonPressedFlag) {
				//Evaluate if this was a double click
				/** \todo unregister doubleClickFlag after some time has passed if no one read the flag??? */
				doubleClickEvaluate();
				//Update this click
				clickAtTick = getSystick();
			}
			//The button is pressed.
			buttonPressedFlag = TRUE;
		} else {
			//Count up 'presscount' and try to reach BUTTON_DEBOUNCE_PERIOD
			presscount++;
			//~ ledOn();							//Optional visual indication of the button press //TODO: Consider removing visual feedback
		}
	} else {
		//The button is not pressed, count down to 0 to see if the button have been released.
		if(presscount == 0) {
			buttonPressedFlag = FALSE;
		} else {
			presscount--;
			//~ ledOff();							//TODO: Consider removing
		}
	}
}

/** \brief Initialize the buttons used in the f3g */
void buttonInit(void)
{
	DDRB &= ~(1 << BUTTON_PIN);
	PORTB |= (1 << BUTTON_PIN);
}
