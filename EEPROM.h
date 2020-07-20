/*
 * EEPROM.h
 *
 * Created: 18-07-2020 8.19.07 PM
 *  Author: Ramiz
 */ 

/*
	//when using Read or Write methods we must disable global interrupts. If interrupts are not disabled then Reading/Writing may produce error
	//Here we have manually disabled global interrupts when reading/writing EEPROM so that interrupt issue do not occur
*/


#ifndef EEPROM_H_
#define EEPROM_H_

#include<avr/interrupt.h>

extern uint16_t startAddress = 0x0005;	
extern uint16_t endAddressLocation = 0x01;


void EEPROM_erase_Address(uint16_t address)
{
	//this method erases the EEPROM data from the provided address. Erasing any address will set its value to 0xFF
	
	while(EECR & (1 << EEPE)); //wait if there is any write operation is in progress.
	
	EEARH = (address & 0xFF00) >> 8;
	EEARL = (address & 0x00FF);
	
	EECR = (0 << EEPM1) | (1 << EEPM0); 
	
	EECR |= (1 << EEMPE);
	EECR |= (1 << EEPE);
	
}

char EEPROM_write_To_Address(uint16_t ucAddress, unsigned char ucData, char statusRegisterValue) //returns the old status register value, useful when handling interrupts
{
	//This method first erases the location then writes. If you only need to erase/write then use the other methods. 
	//This method is preferable over the other write method but this method will require more time than other methods to complete the action.
	
	char cSREG;
	cSREG =  statusRegisterValue/* store SREG value */
	/* disable interrupts during timed sequence */
	cli();
	
	/* Set Programming mode */
	//Erase and Write in one Operation
	EECR = (0<<EEPM1)|(0<<EEPM0);
	
	EECR |= (0 << EERIE); //disable EEPROM ready interrupt
	
	/* Set up address and data registers */
	EEARH = (ucAddress & 0xFF00) >> 8; //mask the higher 8 bits
	EEARL = (ucAddress & 0x00FF); //mask the lower 8 bits
	EEDR = ucData; //data storage capacity in attiny is 8 bit 
	
	/* Write logical one to EEMPE */
	EECR |= (1<<EEMPE);
	
	/* Start eeprom write by setting EEPE */
	EECR |= (1<<EEPE);
	
	/* Wait for completion of write */
	while(EECR & (1<<EEPE));
	
	return statusRegisterValue;
}

unsigned char EEPROM_read_from_Address(uint16_t ucAddress) 
{
	/* Wait for completion of previous write if any*/
	while(EECR & (1<<EEPE));
	
	EECR |= (0 << EERIE); //disable EEPROM ready interrupt
	
	/* Set up address register */
	EEARH = (ucAddress & 0xFF00) >> 8;
	EEARL = (ucAddress & 0x00FF);
	
	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);
	
	/* Return data from data register */
	return EEDR;
}


#endif /* EEPROM_H_ */