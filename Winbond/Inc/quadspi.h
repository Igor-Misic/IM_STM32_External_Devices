/*
 * This program is driver for QUADSPI.  
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

#ifndef __QUADSPI_H
#define __QUADSPI_H

#include <stdbool.h>
#include <stdint.h>

#include "stm32h7xx_hal.h"

bool QuadSpi_Init(QSPI_HandleTypeDef *hqspi, uint8_t flashSize);
bool QuadSpiInstruction(QSPI_HandleTypeDef *hqspi, uint8_t instruction);
bool QuadSpiInstructionWithAddress(QSPI_HandleTypeDef *hqspi, uint8_t instruction, uint32_t address, uint32_t addressSize);
bool QuadSpiReceive1Line(QSPI_HandleTypeDef *hqspi, uint8_t instruction, uint8_t dummyCycles, uint8_t *in, uint16_t length);
bool QuadSpiReceive4Lines(QSPI_HandleTypeDef *hqspi, uint8_t instruction, uint8_t dummyCycles, uint8_t *in, uint16_t length);
bool QuadSpiTransmit1Line(QSPI_HandleTypeDef *hqspi, uint8_t instruction, uint8_t dummyCycles, const uint8_t *out, uint16_t length);
bool QuadSpiTransmitWithAddress1Line(QSPI_HandleTypeDef *hqspi, uint8_t instruction, uint8_t dummyCycles, uint32_t address, uint32_t addressSize, const uint8_t *out, int length);
bool QuadSpiTransmitWithAddress4Line(QSPI_HandleTypeDef *hqspi, uint8_t instruction, uint8_t dummyCycles, uint32_t address, uint32_t addressSize, const uint8_t *out, int length);

bool QuadSpiReceiveWithAddress1Line(QSPI_HandleTypeDef *hqspi, uint8_t instruction, uint8_t dummyCycles, uint32_t address, uint8_t addressSize, uint8_t *in, int length);
bool QuadSpiReceiveWithAddress4Lines(QSPI_HandleTypeDef *hqspi, uint8_t instruction, uint8_t dummyCycles, uint32_t address, uint8_t addressSize, uint8_t *in, int length);

bool QuadSpiInstructionWithAddress1LINE(QSPI_HandleTypeDef *hqspi, uint8_t instruction, uint8_t dummyCycles, uint32_t address, uint8_t addressSize);

bool QuadSpiReceiveWithAddress4LINES(QSPI_HandleTypeDef *hqspi, uint8_t instruction, uint8_t dummyCycles, uint32_t address, uint32_t addressSize, uint8_t *in, int length);



#endif /* __QUADSPI_H */
