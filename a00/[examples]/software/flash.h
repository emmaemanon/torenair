/*
 * flash.h
 *
 * Created: 4/11/2012 12:30:00 PM
 * Authors: Michael Wu (myw9) and Garen Der-Khachadourian (gdd9)
 * Description: Library for SPI communication to perform FLASH memory operations
 */

#ifndef FLASH_H_
#define FLASH_H_

/******************************************************************************
 * Constants
 *****************************************************************************/

#define DUMMY_DATA			0

// Flash Instruction Codes
#define PAGE_PROG				0x02
#define READ_DATA				0x03
#define READ_SR1				0x05
#define WRITE_ENABLE			0x06
#define SECTOR_ERASE			0x20
#define READ_SR2				0x35
#define READ_MANUFACTURER	0x90
#define CHIP_ERASE			0xC7
#define BLOCK_ERASE_64KB	0xD8

/******************************************************************************
 * Function Prototypes
 *****************************************************************************/

void init_spi(void);
void flash_cmd_read_data(uint32_t addr, uint8_t* buffer, uint16_t size);
void flash_cmd_page_prog(uint32_t addr, uint8_t* buffer, uint16_t size);
void flash_cmd_chip_erase(void);
void flash_cmd_block_erase_64kb(uint32_t addr);
void flash_cmd_sector_erase(uint32_t addr);
void flash_cmd_write_enable(void);
uint8_t flash_cmd_read_sr1(void);
void flash_wait_ready(void);
void spi_tx_address(uint32_t addr);
uint8_t spi_tx(uint8_t data);
void enable_slave(void);
void disable_slave(void);

#endif /* FLASH_H_ */
