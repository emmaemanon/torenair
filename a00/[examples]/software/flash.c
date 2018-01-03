/*
 * flash.c
 *
 * Created: 4/11/2012 12:30:00 PM
 * Authors: Michael Wu (myw9) and Garen Der-Khachadourian (gdd9)
 * Description: Library for SPI communication to perform FLASH memory operations
 */

#include <avr/io.h>
#include "flash.h"

/******************************************************************************
 * SPI Functions
 *****************************************************************************/

// Initialize SPI for Flash memory operations
// Pin B4 <-> SS_BAR
// Pin B5 <-> MOSI
// Pin B6 <-> MISO
// Pin B7 <-> SCLK
void init_spi(void) {
	// SPI init
	DDRB |= (1<<PB4) + (1<<PB5) + (1<<PB7);	// set SS_BAR, MOSI, and SCLK as outputs
	DDRB &= ~(1<<PB6);								// set MISO to input
	PORTB |= (1<<PB4);								// disable ss_bar
	SPCR = (1<<SPE) + (1<<MSTR) + (1<<SPR0);	// enable SPI, Master, set clock rate fck/16 = 1MHz
}

/******************************************************************************
 * Flash Memory Functions
 *****************************************************************************/

// Send "Read Data" Command
void flash_cmd_read_data(uint32_t addr, uint8_t* buffer, uint16_t size) {
	uint16_t i=0;

	flash_wait_ready();
	enable_slave();
	spi_tx(READ_DATA);
	spi_tx_address(addr);
	for (i=0; i<size; i++) {
		buffer[i] = spi_tx(DUMMY_DATA);
	}
	disable_slave();
}

// Send "Page Program" Command
void flash_cmd_page_prog(uint32_t addr, uint8_t* buffer, uint16_t size) {
	uint16_t i=0;

	flash_cmd_write_enable();
	flash_wait_ready();
	enable_slave();
	spi_tx(PAGE_PROG);
	spi_tx_address(addr);
	for (i=0; i<size; i++) {
		spi_tx(buffer[i]);
	}
	disable_slave();
}

// Send "Chip Erase" Command
void flash_cmd_chip_erase(void) {
	flash_cmd_write_enable();
	flash_wait_ready();
	enable_slave();
	spi_tx(CHIP_ERASE);
	disable_slave();
}

// Send "64KB Block Erase" Command
void flash_cmd_block_erase_64kb(uint32_t addr) {
	flash_cmd_write_enable();
	flash_wait_ready();
	enable_slave();
	spi_tx(BLOCK_ERASE_64KB);
	spi_tx_address(addr);
	disable_slave();
}

// Send "Sector Erase" Command
void flash_cmd_sector_erase(uint32_t addr) {
	flash_cmd_write_enable();
	flash_wait_ready();
	enable_slave();
	spi_tx(SECTOR_ERASE);
	spi_tx_address(addr);
	disable_slave();
}

// Send "Write Enable" Command
void flash_cmd_write_enable(void) {
	flash_wait_ready();
	enable_slave();
	spi_tx(WRITE_ENABLE);
	disable_slave();
}

// Send "Read Status Register-1" Command
uint8_t flash_cmd_read_sr1(void) {
	uint8_t sr1=0;;
	enable_slave();
	spi_tx(READ_SR1);
	sr1 = spi_tx(DUMMY_DATA);
	disable_slave();
	return sr1;
}

// Wait until Flash is not busy
void flash_wait_ready(void) {
	uint8_t sr1=0x01;
	while (sr1 & 0x01) {
		sr1 = flash_cmd_read_sr1();
	}
}

// Send 24-bit address
void spi_tx_address(uint32_t addr) {
	spi_tx((uint8_t) ((addr & 0x00ff0000)>>16));
	spi_tx((uint8_t) ((addr & 0x0000ff00)>>8));
	spi_tx((uint8_t) (addr & 0x000000ff));
}

// Transmit byte via SPI
uint8_t spi_tx(uint8_t data) {
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}

// Enable slave (pin B4: ss_bar)
void enable_slave(void) {
	PORTB &= 0xef;
}

// Disable slave (pin B4: ss_bar)
void disable_slave(void) {
	PORTB |= 0x10;
}
