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

/* EEPROM layout
 * Available: 256 bytes
 *
 * 0x
 * 0x TWI master address, user selectable, if not set (0xff) use TWI_ADDR_MASTER_DEFAULT (0x45, or similar)
 * 0x TWI slave address, user selectable, if not set (0xff) use TWI_ADDR_SLAVE_DEFAULT (0x51, or similar)
 * 0x timer0prescale value
 * 0x timer1prescale value
 * 0x Flag, get OC1A settings from EEPROM or ADC (potentiometer)
 * 0x
 * 0x
 * 0x
 * 0x
 * 0x
 *
 */

//TODO: see DS page 19, prevent eeprom corruption

char eepromRead(char addr)
{
	//Sleep until the EEPROM is ready to be read
	while(EECR & (1 << EEPE)) { pmSleep(); }
	EEARL = addr;							//Set the EEPROM address
	EECR = (1 << EERE);						//Strobe a EEPROM read
	return EEDR;							//Return the EEPROM data
}

void eepromWrite(char addr, char byte)
{
	//Sleep until the EEPROM is ready to be read (this might be 3.4 ms, maximum)
	while(EECR & (1 << EEPE)) { pmSleep(); }
	EEARL = addr;							//Set the EEPROM address
	EEDR = byte;							//Set the data to write into EEPROM addr
	interruptDisable();
	EECR = (0 << EEPM1) | (0 << EEPM0) |	//Atomic write operation
			(0 << EERIE) |					//Writing a single byte, so there is no need to interrupt
			(1 << EEMPE)
	//Within four clocks
	EECR |= (1 << EEPE);
	interruptEnable();
}


ISR(EE_RDY_vect)
{

}
