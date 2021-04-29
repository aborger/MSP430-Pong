/* --COPYRIGHT--,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/

/*
 * UCS.c
 *
 *  Created on: May 19, 2020
 *      Author: joshuadgriffin
 *
 *
 *  The MSP430F5529 requires the following PMMCOREVx settings
 *  for different MCLK speeds
 *
 *  System Frequency      	PMMCOREVx
 *  0 < MLCK <= 8 MHz		0
 *  8 < MLCK <= 12 MHz		1
 *  12 < MLCK <= 20 MHz		2
 *  20 < MLCK <= 25 MHz		3
 *
 *  DCORSEL		DCO FREQ Range (MHz)
 *  0			0.7 - 1.7
 *  1			0.15 - 3.45
 *  2			0.32 - 7.38
 *  3			0.64 - 14
 *  4			1.3 - 28.2
 *  5			2.5 - 54.1
 *  6			4.6 - 88
 *  7			8.5	- 135
 *
 *  FLL Operating Equations:
 *
 *  fDCOCLK = FLLD × (FLLN + 1) × (fFLLREFCLK / FLLREFDIV)
 *  fDCOCLKDIV = (FLLN + 1) × (fFLLREFCLK / FLLREFDIV)
 */

#include "UCS.h"

void setVcoreUp (uint8_t level) {

	// Open PMM registers for write
	PMMCTL0_H = PMMPW_H;

	// Set SVS/SVM high side new level
	SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;

	// Set SVM low side to new level
	SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;

	// Wait till SVM is settled
	while ((PMMIFG & SVSMLDLYIFG) == 0);

	// Clear already set flags
	PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);

	// Set VCore to new level
	PMMCTL0_L = PMMCOREV0 * level;

	// Wait till new level reached
	if ((PMMIFG & SVMLIFG))
		while ((PMMIFG & SVMLVLRIFG) == 0);

	// Set SVS/SVM low side to new level
	SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;

	// Lock PMM registers for write access
	PMMCTL0_H = 0x00;
}

void initClocks(uint16_t sysClk) {

	P5SEL |= BIT4+BIT5;                       // Select XT1

	UCSCTL6 &= ~(XT1OFF);                     // XT1 On
	UCSCTL6 |= XCAP_3;                        // Internal load cap
	UCSCTL3 = SELREF_0;                       // FLL Reference Clock = XT1

	// Loop until XT1,XT2 & DCO stabilizes - In this case loop until XT1 and DCO settle
	do
	{
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
		// Clear XT2,XT1,DCO fault flags
		SFRIFG1 &= ~OFIFG;                      // Clear fault flags
	} while (SFRIFG1&OFIFG);                   // Test oscillator fault flag

	UCSCTL6 &= ~(XT1DRIVE_3);                 // Xtal is now stable, reduce drive strength

	UCSCTL4 |= SELA_0;                        // Set ACLK = XT1

	if (sysClk == 1000) {
		// ACLK = 32768 Hz from XT1
		// SMCLK = MCLK = 1048576 Hz using FLL with XT1 reference

		__bis_SR_register(SCG0);                  // Disable the FLL control loop
		UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
		UCSCTL1 = DCORSEL_2;                      // Select DCO range 1MHz operation
		UCSCTL2 = FLLD_1 + 31;                   // Set DCO Multiplier for 1MHz
		// (N + 1) * FLLRef = Fdco
		// (31 + 1) * 32768 = 1MHz
		// Set FLL Div = fDCOCLK/2
		__bic_SR_register(SCG0);                  // Enable the FLL control loop

		// Worst-case settling time for the DCO when the DCO range bits have been
		// changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
		// UG for optimization.
		// 32 x 32 x 1 MHz / 32,768 Hz ~ 33k MCLK cycles for DCO to settle
		__delay_cycles(33000);

		// Loop until XT1,XT2 & DCO stabilizes - In this case only DCO has to stabilize
		do
		{
			UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
			// Clear XT2,XT1,DCO fault flags
			SFRIFG1 &= ~OFIFG;                      // Clear fault flags
		} while (SFRIFG1&OFIFG);                  // Test oscillator fault flag

	} else if (sysClk == 8000) {
		// ACLK = 32768 Hz from XT1
		// SMCLK = MCLK = 8028160 Hz using FLL with XT1 reference

		// Increase Vcore setting to level 3 to support fsystem = 8MHz
		// NOTE: Change core voltage one level at a time...
		setVcoreUp (0x01);

		__bis_SR_register(SCG0);                  // Disable the FLL control loop
		UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
		UCSCTL1 = DCORSEL_4;                      // Select DCO range 1MHz operation
		UCSCTL2 = FLLD_1 + 244;                   // Set DCO Multiplier for 1MHz
		// (N + 1) * FLLRef = Fdco
		// (244 + 1) * 32768 = 8MHz
		// Set FLL Div = fDCOCLK/2
		__bic_SR_register(SCG0);                  // Enable the FLL control loop

		// Worst-case settling time for the DCO when the DCO range bits have been
		// changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
		// UG for optimization.
		// 32 x 32 x 8 MHz / 32,768 Hz ~ 251k MCLK cycles for DCO to settle
		__delay_cycles(251000);

		// Loop until XT1,XT2 & DCO stabilizes - In this case only DCO has to stabilize
		do
		{
			UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
			// Clear XT2,XT1,DCO fault flags
			SFRIFG1 &= ~OFIFG;                      // Clear fault flags
		} while (SFRIFG1&OFIFG);                  // Test oscillator fault flag

	} else if (sysClk == 16000) {
		// ACLK = 32768 Hz from XT1
		// SMCLK = MCLK = 16023552 Hz using FLL with XT1 reference

		// Increase Vcore setting to level 3 to support fsystem = 8MHz
		// NOTE: Change core voltage one level at a time...
		setVcoreUp (0x01);
		setVcoreUp (0x02);

		__bis_SR_register(SCG0);                  // Disable the FLL control loop
		UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
		UCSCTL1 = DCORSEL_5;                      // Select DCO range 1MHz operation
		UCSCTL2 = FLLD_1 + 488;                   // Set DCO Multiplier for 16MHz
		// (N + 1) * FLLRef = Fdco
		// (488 + 1) * 32768 = 16MHz
		// Set FLL Div = fDCOCLK/2
		__bic_SR_register(SCG0);                  // Enable the FLL control loop

		// Worst-case settling time for the DCO when the DCO range bits have been
		// changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
		// UG for optimization.
		// 32 x 32 x 16 MHz / 32,768 Hz ~ 501k MCLK cycles for DCO to settle
		__delay_cycles(501000);

		// Loop until XT1,XT2 & DCO stabilizes - In this case only DCO has to stabilize
		do
		{
			UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
			// Clear XT2,XT1,DCO fault flags
			SFRIFG1 &= ~OFIFG;                      // Clear fault flags
		} while (SFRIFG1&OFIFG);                  // Test oscillator fault flag

	} else if (sysClk == 25000) {
		// ACLK = 32768 Hz from XT1
		// SMCLK = MCLK = 25001984 Hz using FLL with XT1 reference

		// Increase Vcore setting to level 3 to support fsystem = 25MHz
		// NOTE: Change core voltage one level at a time...
		setVcoreUp (0x01);
		setVcoreUp (0x02);
		setVcoreUp (0x03);

		__bis_SR_register(SCG0);                  // Disable the FLL control loop
		UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
		UCSCTL1 = DCORSEL_7;                      // Select DCO range 50MHz operation
		UCSCTL2 = FLLD_1 + 762;                   // Set DCO Multiplier for 25MHz
		// (N + 1) * FLLRef = Fdco
		// (762 + 1) * 32768 = 25MHz
		// Set FLL Div = fDCOCLK/2

		__bic_SR_register(SCG0);                  // Enable the FLL control loop

		// Worst-case settling time for the DCO when the DCO range bits have been
		// changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
		// UG for optimization.
		// 32 x 32 x 25 MHz / 32,768 Hz ~ 780k MCLK cycles for DCO to settle
		__delay_cycles(782000);

		// Loop until XT1,XT2 & DCO stabilizes - In this case only DCO has to stabilize
		do
		{
			UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
			// Clear XT2,XT1,DCO fault flags
			SFRIFG1 &= ~OFIFG;                      // Clear fault flags
		} while (SFRIFG1&OFIFG);                  // Test oscillator fault flag

	}
}
