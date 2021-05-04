/* Tiny TFT Graphics Library v2 - see http://www.technoblogy.com/show?L6I

   David Johnson-Davies - www.technoblogy.com - 14th January 2020
   ATtiny85 @ 8 MHz (internal oscillator; BOD disabled)

   CC BY 4.0
   Licensed under a Creative Commons Attribution 4.0 International license:
   http://creativecommons.org/licenses/by/4.0/
 */

/*************************************************************************
 * The code below is a modification of the Tiny TFT Library listed above
 * ported for an MSP430F5529LP driving a 128 x 128 TFT on the BOOSTXL EDUMKII
 * booster pack from Texas Instruments.
 *
 * Joshua Griffin -- Northwest Nazarene University -- April 30, 2020
 **************************************************************************/

#ifndef TINYTFT_H_
#define TINYTFT_H_

#include "msp430.h"
#include "UCS.h"
#include <stdint.h>

#define ONE_MS_DELAY (SYS_CLOCK_FREQ_1000_KHZ)

#define LCD_RS_SEL_REG P8SEL
#define LCD_RS_OUT_REG P8OUT
#define LCD_RS_DIR_REG P8DIR
#define LCD_RS BIT2

#define LCD_CS_SEL_REG P2SEL
#define LCD_CS_OUT_REG P2OUT
#define LCD_CS_DIR_REG P2DIR
#define LCD_CS BIT6

#define LCD_RST_SEL_REG P7SEL
#define LCD_RST_OUT_REG P7OUT
#define LCD_RST_DIR_REG P7DIR
#define LCD_RST BIT4

#define LCD_SCL_SDA_SEL_REG P3SEL
#define LCD_SCL_SDA_OUT_REG P3OUT
#define LCD_SCL_SDA_DIR_REG P3DIR
#define LCD_SCL BIT2
#define LCD_SDA BIT0

#define LCD_BL_SEL_REG P2SEL
#define LCD_BL_OUT_REG P2OUT
#define LCD_BL_DIR_REG P2DIR
#define LCD_BL BIT4

// Display dimensions -- Adafruit 1.44" 128x128 display
#define Y_SIZE 128
#define X_SIZE 128
#define Y_OFF 3
#define X_OFF 2
#define ROTATE 0x07
#define TOTAL_PIXEL_CNT 16384

// Display Addresses in ST7735S display driver
#define CASET 0x2A // Define column address
#define RASET 0x2B // Define row address
#define RAMWR 0x2C // Write to display RAM
#define SWRESET 0x01 // software reset
#define SLPOUT 0x11 // out of sleep mode
#define COLMOD 0x3A // color mode
#define COLOR_MODE_16_BIT 0x05
#define COLOR_MODE_12_BIT 0x03
#define INVOFF 0x20 // display inversion off
#define MADCTL 0x36	// memory data access control
#define DISPON 0x29	// display on

#define WRITECOMM(command) {		\
	LCD_RS_OUT_REG &= ~LCD_RS;		\
	while(!(UCB0IFG & UCTXIFG)){}	\
	UCB0TXBUF = command;			\
	while(UCB0STAT & UCBUSY){}		\
	LCD_RS_OUT_REG |= LCD_RS;		\
}

// uint8_t dummy = UCB0RXBUF;		\

#define WRITEDATA(data) {			\
	while(!(UCB0IFG & UCTXIFG)){}	\
	UCB0TXBUF = data;				\
	while(UCB0STAT & UCBUSY){}		\
}

//	uint8_t dummy = UCB0RXBUF;		\

// Current plot position and colors
typedef struct _displayContext {
	uint8_t x0;
	uint8_t y0;
	uint16_t foreColor;
	uint16_t backColor;
	uint8_t textScale;
} displayContext;

void initSPI(void);
void initBacklightPWM(void);
void delayMilliseconds(uint16_t ms);
void initDisplay (void);
void displayOn (void);
void clearDisplay (void);
void setForeColor (displayContext *context, uint8_t r, uint8_t g, uint8_t b);
void setBackColor (displayContext *context, uint8_t r, uint8_t g, uint8_t b);
void setPosition (displayContext *context, uint8_t x, uint8_t y);
void plotPoint (displayContext context);
void drawTo (displayContext *context, uint8_t x, uint8_t y);
void fillRect (displayContext context, uint8_t w, uint8_t h);
void plotChar (displayContext *context, unsigned char c);
void plotText(displayContext *context, char *p);
void drawHorLine (displayContext context, uint8_t length);
void drawVerLine (displayContext context, uint8_t length);

#endif /* TINYTFT_H_ */
