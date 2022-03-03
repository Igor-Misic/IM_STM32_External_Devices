/*
 * This program is driver for Winbond W25Q128x Serial flash memory.  
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

#include "w25q.h"
#include "quadspi.h"

#define W25Q_LINEAR_TO_PAGE(laddr) ((laddr) & 0xFFFFFF)
#define W25Q_LINEAR_TO_BLOCK(laddr) (W25Q_LINEAR_TO_PAGE(laddr) / W25Q_PAGES_PER_BLOCK)
#define W25Q_BLOCK_TO_PAGE(block) ((block) * W25Q_PAGES_PER_BLOCK)
#define W25Q_BLOCK_TO_LINEAR(block) (W25Q_BLOCK_TO_PAGE(block) * W25Q_PAGE_SIZE)

QSPI_HandleTypeDef *ptr_hqspi;

bool W25q_init(QSPI_HandleTypeDef *hqspi)
{
	bool success = true;
	ptr_hqspi = hqspi;
	uint8_t buffer[3];

	W25q_readJedec(buffer);
	if(
			(buffer[0] != W25Q_MANUFACTURER_ID) ||
			(buffer[1] != W25Q_DEVICE_ID_1_IQ && buffer[1] != W25Q_DEVICE_ID_1_IM) ||
			(buffer[2] != W25Q_DEVICE_ID_2)
			) {
		success = false;
	}

	if(success) {
		//success = W25q_writeStatusRegister(ptr_hqspi, W25Q_INSTR_WRITE_STATUS_REG1, W25Q_STATUS_REG_CLEAR_ALL);
	}

	return success;
}

void W25q_readJedec(uint8_t* idBuffer) {
	QuadSpiReceive1Line(ptr_hqspi, W25Q_INSTR_JEDEC_ID, 0, idBuffer, 3);
}

bool W25q_writeEnable(void)
{
	uint8_t statusReg;

	W25q_waitForReady();
	bool success = false;

	success = QuadSpiInstruction(ptr_hqspi, W25Q_INSTR_WRITE_ENABLE);

	if(success) {
		W25q_readStatusRegister(W25Q_INSTR_READ_STATUS_REG1, &statusReg);

		if(!(statusReg & W25Q_STATUS_REG1_WEL)) {
			success = false;
		}
	}

	return success;
}

bool W25q_quadEnable(void) {

	bool success = false;
	uint8_t dummyCycles = 0;
	uint16_t length = 1u;
	uint8_t statusRegister;

	success = W25q_writeEnable();

	if (success) {
		statusRegister = W25Q_STATUS_REG2_QE;
		success = QuadSpiTransmit1Line(ptr_hqspi, W25Q_INSTR_WRITE_STATUS_REG2,  dummyCycles, &statusRegister, length);
	}

	if(success) {
		success = W25q_readStatusRegister(W25Q_INSTR_READ_STATUS_REG2, &statusRegister);
	}

	if(success && (statusRegister & W25Q_STATUS_REG2_QE)) {
		success = true;
	} else {
		success = false;
	}

	return success;
}

bool W25q_readStatusRegister(uint8_t instruction, uint8_t* statusRegister)
{
	bool success = false;
	uint16_t length = 1u;

	success = QuadSpiReceive1Line(ptr_hqspi, instruction, W25Q_ZERO_DUMMY_CYCLES, statusRegister, length);

	return success;
}

bool W25q_writeStatusRegister(uint8_t instruction, uint8_t statusRegister)
{
	bool success = false;
	uint16_t length = 1u;

	success = W25q_writeEnable();
	if (success) {
		success = QuadSpiTransmit1Line(
				ptr_hqspi,
				instruction,
				W25Q_ZERO_DUMMY_CYCLES,
				&statusRegister,
				length
				);
	}

	return success;
}

void W25q_waitForReady(void)
{
	uint8_t statusReg;
	W25q_readStatusRegister(W25Q_INSTR_READ_STATUS_REG1, &statusReg);

	while (statusReg & W25Q_STATUS_REG1_BUSY) {
		W25q_readStatusRegister(W25Q_INSTR_READ_STATUS_REG1, &statusReg);
	}
}

bool W25q_readBytes(uint32_t address, uint8_t *buffer, uint32_t length)
{
	bool success = false;
	uint32_t pageAddress = W25Q_LINEAR_TO_PAGE(address);

	W25q_waitForReady();

	success = QuadSpiReceiveWithAddress4LINES(
			ptr_hqspi,
			W25Q_INSTR_FAST_READ_QUAD,
			W25Q_DUMMY_CYCLES_FAST_READ_QUAD,
			pageAddress,
			QSPI_ADDRESS_24_BITS,
			buffer,
			length
			);

	return success;
}

bool W25q_sectorErase(uint32_t address)
{
	bool success = false;

	uint32_t pageAddress = W25Q_LINEAR_TO_PAGE(address);

	success = W25q_writeEnable();
	W25q_waitForReady();

	if(success) {
		success = QuadSpiInstructionWithAddress(
				ptr_hqspi,
				W25Q_INSTR_SECTOR_ERASE,
				pageAddress,
				QSPI_ADDRESS_24_BITS
				);
	}

	return success;
}


bool W25q_blockErase32k(uint32_t address)
{
	bool success = false;

	uint32_t pageAddress = W25Q_LINEAR_TO_PAGE(address);

	success = W25q_writeEnable();
	W25q_waitForReady();

	if(success) {
		success = QuadSpiInstructionWithAddress(
				ptr_hqspi,
				W25Q_INSTR_32K_BLOCK_ERASE,
				pageAddress,
				QSPI_ADDRESS_24_BITS
				);
	}

	return success;
}

bool W25q_blockErase64k(uint32_t address)
{
	bool success = false;

	uint32_t pageAddress = W25Q_LINEAR_TO_PAGE(address);

	success = W25q_writeEnable();
	W25q_waitForReady();

	if(success) {
		success = QuadSpiInstructionWithAddress(
				ptr_hqspi,
				W25Q_INSTR_64K_BLOCK_ERASE,
				pageAddress,
				QSPI_ADDRESS_24_BITS
				);
	}

	return success;
}

bool W25q_flexibleSizeErase(uint32_t size, uint32_t address)
{
	bool success = true;

	if(size <= W25Q_32K_BLOCK_SIZE) {
		success = W25q_blockErase32k(address);

	} else if (size <= W25Q_64K_BLOCK_SIZE) {

		success = W25q_blockErase64k(address);

	} else if (size <= W25Q_CHIP_SIZE) {

		uint32_t numberOf64Blocks = size / W25Q_64K_BLOCK_SIZE;

		for(uint32_t block = 0; success && (block <= numberOf64Blocks); block++) {

			uint32_t block_address = address + (block * W25Q_64K_BLOCK_SIZE);
			success = W25q_blockErase64k(block_address);
		}

	} else {
		// FW can't fit in the chip
		success = false;
	}

	return success;
}

bool W25q_chipErase(void)
{
	bool success = false;

	success = W25q_writeEnable();
	W25q_waitForReady();

	if(success){
		success = QuadSpiInstruction(ptr_hqspi, W25Q_CHIP_ERASE);
	}

	return success;
}

bool W25q_dynamicErase(uint32_t firmwareSize, uint32_t flashAddress)
{
	bool success = true;

	if(firmwareSize <= W25Q_32K_BLOCK_SIZE) {

		success = W25q_blockErase32k(flashAddress);

	} else if (firmwareSize <= W25Q_64K_BLOCK_SIZE) {

		success = W25q_blockErase64k(flashAddress);

	} else if (firmwareSize <= FLASH_SIZE) {

		uint32_t numberOf64Blocks = firmwareSize / W25Q_64K_BLOCK_SIZE;

		for(uint32_t block = 0; success && (block <= numberOf64Blocks); block++) {

			uint32_t address = flashAddress + (block * W25Q_64K_BLOCK_SIZE);
			success = W25q_blockErase64k(address);
		}

	} else {
		// FW can't fit in the chip
		success = false;
	}

	return success;
}

bool W25q_quadPageProgram(uint32_t address, uint8_t *buffer, uint32_t length)
{
	bool success = false;

	if(length <= W25Q_PAGE_SIZE) {

		uint32_t pageAddress = W25Q_LINEAR_TO_PAGE(address);

		success = W25q_writeEnable();

		W25q_waitForReady();

		if(success) {
			success = QuadSpiTransmitWithAddress4Line(
					ptr_hqspi,
					W25_INSTR_QUAD_INPUT_PAGE_PROGRAM,
					W25Q_ZERO_DUMMY_CYCLES,
					pageAddress,
					QSPI_ADDRESS_24_BITS,
					buffer,
					length
					);
		}

	} else {
		success = false;
	}

	return success;
}

bool W25q_memoryMappedModeEnable(void)
{
	bool success = true;
	QSPI_CommandTypeDef cmd;
	QSPI_MemoryMappedTypeDef memMappedCfg;

	cmd.InstructionMode		= QSPI_INSTRUCTION_1_LINE;
	cmd.Instruction			= W25Q_INSTR_FAST_READ_QUAD;
	cmd.AlternateByteMode	= QSPI_ALTERNATE_BYTES_NONE;
	cmd.DataMode			= QSPI_DATA_4_LINES;
	cmd.DdrMode				= QSPI_DDR_MODE_DISABLE;
	cmd.DdrHoldHalfCycle	= QSPI_DDR_HHC_ANALOG_DELAY;
	cmd.AddressMode			= QSPI_ADDRESS_4_LINES;
	cmd.AddressSize			= QSPI_ADDRESS_24_BITS;
	cmd.DummyCycles			= W25Q_DUMMY_CYCLES_FAST_READ_QUAD;
	cmd.SIOOMode			= QSPI_SIOO_INST_EVERY_CMD;

	memMappedCfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
	memMappedCfg.TimeOutPeriod = 0;

	W25q_waitForReady();

	if (HAL_QSPI_MemoryMapped(ptr_hqspi, &cmd, &memMappedCfg) != HAL_OK)
	{
		success = false;
	}

	return success;
}
