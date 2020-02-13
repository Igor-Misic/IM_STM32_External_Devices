/*
 * This program is driver for Winbond W25N1Gxx Serial flash memory.  
 * Copyright (C) 2020  Igor Misic, igy1000mb@gmail.com
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * 
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __W25N01G_H
#define __W25N01G_H

#include <stdbool.h>
#include <stdint.h>

#include "stm32h7xx_hal.h"

// Device size parameters
#define W25N01G_PAGE_SIZE 			2048
#define W25N01G_PAGES_PER_BLOCK		64
#define W25N01G_BLOCKS_PER_DIE		1024

#define W25N01G_STATUS_REGISTER_SIZE			8
#define W25N01G_STATUS_PAGE_ADDRESS_SIZE		16
#define W25N01G_STATUS_COLUMN_ADDRESS_SIZE		16

#define W25N01G_INSTR_DEVICE_RESET					0xFF
#define W25N01G_INSTR_JEDEC_ID						0x9F
#define W25N01G_INSTR_READ_STATUS_REG				0x05
#define W25N01G_INSTR_READ_STATUS_ALTERNATE_REG		0x0F
#define W25N01G_INSTR_WRITE_STATUS_REG				0x01
#define W25N01G_INSTR_WRITE_STATUS_ALTERNATE_REG	0x1F
#define W25N01G_INSTR_WRITE_ENABLE					0x06
#define W25N01G_INSTR_WRITE_DISABLE					0x04
#define W25N01G_INSTR_BB_MANAGEMENT					0xA1
#define W25N01G_INSTR_READ_BBM_LUT					0xA5
#define W25N01G_INSTR_LAST_ECC_FAIL_PAGE_ADR		0xA9
#define W25N01G_INSTR_BLOCK_ERASE					0xD8
#define W25N01G_INSTR_PROGRAM_DATA_LOAD				0x02
#define W25N01G_INSTR_RANDOM_PROGRAM_DATA_LOAD		0x84
#define W25N01G_INSTR_QUAD_PROGRAM_DATA_LOAD		0x32
#define W25N01G_INSTR_QUAD_RANDOM_PROGRAM_DATA_LOAD	0x34
#define W25N01G_INSTR_PROGRAM_EXECUTE				0x10
#define W25N01G_INSTR_PAGE_DATA_READ				0x13
#define W25N01G_INSTR_READ							0x03
#define W25N01G_INSTR_FAST_READ						0x0B
#define W25N01G_INSTR_FAST_READ_QUAD_OUTPUT			0x6B
#define W25N01G_INSTR_FAST_READ_QUAD				0xEB

#define W25N01G_DUMMY_CYCLES_FAST_READ_QUAD					4
#define W25N01G_DUMMY_CYCLES_FAST_READ_QUAD_OUTPUT_BUFFER	8
#define W25N01G_DUMMY_CYCLES_FAST_READ_QUAD_OUTPUT_CONT		12

#define W25N01G_DUMMY_CYCLES_FAST_READ_QUAD_BUFFER			4
#define W25N01G_DUMMY_CYCLES_FAST_READ_QUAD_CONT			12

// Config/status register addresses
#define W25N01G_PROT_REG 0xA0
#define W25N01G_CONF_REG 0xB0
#define W25N01G_STAT_REG 0xC0

// Bits in config/status register 1 (W25N01G_PROT_REG)
typedef struct protRegBits_structure {
	uint8_t srp1	: 1;
	uint8_t wp_e	: 1;
	uint8_t tb		: 1;
	uint8_t pb0		: 1;
	uint8_t	pb1		: 1;
	uint8_t	pb2		: 1;
	uint8_t	pb3		: 1;
	uint8_t	srp2	: 1;
}protRegBits;

// Bits in config/status register 1 (W25N01G_PROT_REG)
#define W25N01G_PROT_CLEAR					(0)
#define W25N01G_PROT_SRP1_ENABLE			(1 << 0)
#define W25N01G_PROT_WP_E_ENABLE			(1 << 1)
#define W25N01G_PROT_TB_ENABLE				(1 << 2)
#define W25N01G_PROT_PB0_ENABLE				(1 << 3)
#define W25N01G_PROT_PB1_ENABLE				(1 << 4)
#define W25N01G_PROT_PB2_ENABLE				(1 << 5)
#define W25N01G_PROT_PB3_ENABLE		 		(1 << 6)
#define W25N01G_PROT_SRP0_ENABLE			(1 << 7)

// Bits in config/status register 2 (W25N01G_CONF_REG)
#define W25N01G_CONFIG_ECC_ENABLE			(1 << 4)
#define W25N01G_CONFIG_BUFFER_READ_MODE		(1 << 3)

// Bits in config/status register 3 (W25N01G_STATREG)
#define W25N01G_STATUS_BBM_LUT_FULL			(1 << 6)
#define W25N01G_STATUS_FLAG_ECC_POS			4
#define W25N01G_STATUS_FLAG_ECC_MASK		((1 << 5)|(1 << 4))
#define W25N01G_STATUS_FLAG_ECC(status)		(((status) & W25N01G_STATUS_FLAG_ECC_MASK) >> 4)
#define W25N01G_STATUS_PROGRAM_FAIL			(1 << 3)
#define W25N01G_STATUS_ERASE_FAIL			(1 << 2)
#define W25N01G_STATUS_FLAG_WRITE_ENABLED	(1 << 1)
#define W25N01G_STATUS_FLAG_BUSY			(1 << 0)

bool W25n01g_deviceRestart(QSPI_HandleTypeDef *hqspi);
void W25n01g_readJedec(QSPI_HandleTypeDef *hqspi, uint8_t* idBuffer);
bool W25n01g_writeEnable(QSPI_HandleTypeDef *hqspi);
bool W25n01g_blockErase(QSPI_HandleTypeDef *hqspi, uint32_t address);
void W25n01g_writeStatusRegister(QSPI_HandleTypeDef *hqspi, uint8_t reg, uint8_t data);
uint8_t W25n01g_readStatusRegister(QSPI_HandleTypeDef *hqspi, uint8_t reg);
void W25n01g_waitForReady(QSPI_HandleTypeDef *hqspi);
//bool W25n01g_memoryMappedModeEnable(QSPI_HandleTypeDef *hqspi, bool bufferRead); // This memory can't work in the memory-mapped mode
bool W25n01g_programDataLoad(QSPI_HandleTypeDef *hqspi, uint16_t columnAddress, const uint8_t *data, uint32_t length);
bool w25n01g_pageProgram(QSPI_HandleTypeDef *hqspi, uint32_t address, const uint8_t *data, uint32_t length);
bool w25n01g_flashProgram(QSPI_HandleTypeDef *hqspi, uint32_t address, const uint8_t *data, uint32_t length);
uint32_t W25n01g_readBytes(QSPI_HandleTypeDef *hqspi, uint32_t address, uint8_t *buffer, uint32_t length, bool bufferMode);

#endif /* __W25N01G_H */
