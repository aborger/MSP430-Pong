/*
 * UCS.h
 *
 *  Created on: May 19, 2020
 *      Author: joshuadgriffin
 */

#ifndef UCS_H_
#define UCS_H_

#include <msp430.h>
#include <stdint.h>

/*
 * Set SYS_CLOCK_FREQ_KHZ to 1000, 8000, 16000, or 250000
 */

#define SYS_CLOCK_FREQ_1000_KHZ 1000
#define SYS_CLOCK_FREQ_8000_KHZ 8000
#define SYS_CLOCK_FREQ_16000_KHZ 16000
#define SYS_CLOCK_FREQ_25000_KHZ 25000

void setVcoreUp (uint8_t level);
void initClocks(uint16_t sysClk);


#endif /* UCS_H_ */
