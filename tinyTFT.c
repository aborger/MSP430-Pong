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

#include "tinyTFT.h"

// Character set for text - stored in program memory
const uint8_t charMap[96][6] = {
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00 },
		{ 0x00, 0x07, 0x00, 0x07, 0x00, 0x00 },
		{ 0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00 },
		{ 0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00 },
		{ 0x23, 0x13, 0x08, 0x64, 0x62, 0x00 },
		{ 0x36, 0x49, 0x56, 0x20, 0x50, 0x00 },
		{ 0x00, 0x08, 0x07, 0x03, 0x00, 0x00 },
		{ 0x00, 0x1C, 0x22, 0x41, 0x00, 0x00 },
		{ 0x00, 0x41, 0x22, 0x1C, 0x00, 0x00 },
		{ 0x2A, 0x1C, 0x7F, 0x1C, 0x2A, 0x00 },
		{ 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00 },
		{ 0x00, 0x80, 0x70, 0x30, 0x00, 0x00 },
		{ 0x08, 0x08, 0x08, 0x08, 0x08, 0x00 },
		{ 0x00, 0x00, 0x60, 0x60, 0x00, 0x00 },
		{ 0x20, 0x10, 0x08, 0x04, 0x02, 0x00 },
		{ 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00 },
		{ 0x00, 0x42, 0x7F, 0x40, 0x00, 0x00 },
		{ 0x72, 0x49, 0x49, 0x49, 0x46, 0x00 },
		{ 0x21, 0x41, 0x49, 0x4D, 0x33, 0x00 },
		{ 0x18, 0x14, 0x12, 0x7F, 0x10, 0x00 },
		{ 0x27, 0x45, 0x45, 0x45, 0x39, 0x00 },
		{ 0x3C, 0x4A, 0x49, 0x49, 0x31, 0x00 },
		{ 0x41, 0x21, 0x11, 0x09, 0x07, 0x00 },
		{ 0x36, 0x49, 0x49, 0x49, 0x36, 0x00 },
		{ 0x46, 0x49, 0x49, 0x29, 0x1E, 0x00 },
		{ 0x00, 0x00, 0x14, 0x00, 0x00, 0x00 },
		{ 0x00, 0x40, 0x34, 0x00, 0x00, 0x00 },
		{ 0x00, 0x08, 0x14, 0x22, 0x41, 0x00 },
		{ 0x14, 0x14, 0x14, 0x14, 0x14, 0x00 },
		{ 0x00, 0x41, 0x22, 0x14, 0x08, 0x00 },
		{ 0x02, 0x01, 0x59, 0x09, 0x06, 0x00 },
		{ 0x3E, 0x41, 0x5D, 0x59, 0x4E, 0x00 },
		{ 0x7C, 0x12, 0x11, 0x12, 0x7C, 0x00 },
		{ 0x7F, 0x49, 0x49, 0x49, 0x36, 0x00 },
		{ 0x3E, 0x41, 0x41, 0x41, 0x22, 0x00 },
		{ 0x7F, 0x41, 0x41, 0x41, 0x3E, 0x00 },
		{ 0x7F, 0x49, 0x49, 0x49, 0x41, 0x00 },
		{ 0x7F, 0x09, 0x09, 0x09, 0x01, 0x00 },
		{ 0x3E, 0x41, 0x41, 0x51, 0x73, 0x00 },
		{ 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00 },
		{ 0x00, 0x41, 0x7F, 0x41, 0x00, 0x00 },
		{ 0x20, 0x40, 0x41, 0x3F, 0x01, 0x00 },
		{ 0x7F, 0x08, 0x14, 0x22, 0x41, 0x00 },
		{ 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00 },
		{ 0x7F, 0x02, 0x1C, 0x02, 0x7F, 0x00 },
		{ 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00 },
		{ 0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00 },
		{ 0x7F, 0x09, 0x09, 0x09, 0x06, 0x00 },
		{ 0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00 },
		{ 0x7F, 0x09, 0x19, 0x29, 0x46, 0x00 },
		{ 0x26, 0x49, 0x49, 0x49, 0x32, 0x00 },
		{ 0x03, 0x01, 0x7F, 0x01, 0x03, 0x00 },
		{ 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00 },
		{ 0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00 },
		{ 0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00 },
		{ 0x63, 0x14, 0x08, 0x14, 0x63, 0x00 },
		{ 0x03, 0x04, 0x78, 0x04, 0x03, 0x00 },
		{ 0x61, 0x59, 0x49, 0x4D, 0x43, 0x00 },
		{ 0x00, 0x7F, 0x41, 0x41, 0x41, 0x00 },
		{ 0x02, 0x04, 0x08, 0x10, 0x20, 0x00 },
		{ 0x00, 0x41, 0x41, 0x41, 0x7F, 0x00 },
		{ 0x04, 0x02, 0x01, 0x02, 0x04, 0x00 },
		{ 0x40, 0x40, 0x40, 0x40, 0x40, 0x00 },
		{ 0x00, 0x03, 0x07, 0x08, 0x00, 0x00 },
		{ 0x20, 0x54, 0x54, 0x78, 0x40, 0x00 },
		{ 0x7F, 0x28, 0x44, 0x44, 0x38, 0x00 },
		{ 0x38, 0x44, 0x44, 0x44, 0x28, 0x00 },
		{ 0x38, 0x44, 0x44, 0x28, 0x7F, 0x00 },
		{ 0x38, 0x54, 0x54, 0x54, 0x18, 0x00 },
		{ 0x00, 0x08, 0x7E, 0x09, 0x02, 0x00 },
		{ 0x18, 0xA4, 0xA4, 0x9C, 0x78, 0x00 },
		{ 0x7F, 0x08, 0x04, 0x04, 0x78, 0x00 },
		{ 0x00, 0x44, 0x7D, 0x40, 0x00, 0x00 },
		{ 0x20, 0x40, 0x40, 0x3D, 0x00, 0x00 },
		{ 0x7F, 0x10, 0x28, 0x44, 0x00, 0x00 },
		{ 0x00, 0x41, 0x7F, 0x40, 0x00, 0x00 },
		{ 0x7C, 0x04, 0x78, 0x04, 0x78, 0x00 },
		{ 0x7C, 0x08, 0x04, 0x04, 0x78, 0x00 },
		{ 0x38, 0x44, 0x44, 0x44, 0x38, 0x00 },
		{ 0xFC, 0x18, 0x24, 0x24, 0x18, 0x00 },
		{ 0x18, 0x24, 0x24, 0x18, 0xFC, 0x00 },
		{ 0x7C, 0x08, 0x04, 0x04, 0x08, 0x00 },
		{ 0x48, 0x54, 0x54, 0x54, 0x24, 0x00 },
		{ 0x04, 0x04, 0x3F, 0x44, 0x24, 0x00 },
		{ 0x3C, 0x40, 0x40, 0x20, 0x7C, 0x00 },
		{ 0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00 },
		{ 0x3C, 0x40, 0x30, 0x40, 0x3C, 0x00 },
		{ 0x44, 0x28, 0x10, 0x28, 0x44, 0x00 },
		{ 0x4C, 0x90, 0x90, 0x90, 0x7C, 0x00 },
		{ 0x44, 0x64, 0x54, 0x4C, 0x44, 0x00 },
		{ 0x00, 0x08, 0x36, 0x41, 0x00, 0x00 },
		{ 0x00, 0x00, 0x77, 0x00, 0x00, 0x00 },
		{ 0x00, 0x41, 0x36, 0x08, 0x00, 0x00 },
		{ 0x00, 0x06, 0x09, 0x06, 0x00, 0x00 },  // degree symbol = '~'
		{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00 }
};

void initSPI(void) {

	LCD_RS_SEL_REG &= ~LCD_RS; // set P8.2 to GPIO mode
	LCD_RS_OUT_REG |= LCD_RS; // LCD_RS for data
	LCD_RS_DIR_REG |= LCD_RS; // set P8.2 as output

	LCD_CS_SEL_REG &= ~LCD_CS; // set P2.6 to GPIO mode
	LCD_CS_OUT_REG |= LCD_CS; // set LCD_CS
	LCD_CS_DIR_REG |= LCD_CS; // set P2.6 as output

	LCD_RST_SEL_REG &= ~LCD_RST; // set P7.4 to GPIO mode
	LCD_RST_OUT_REG  |= LCD_RST; // set LCD_RST
	LCD_RST_DIR_REG  |= LCD_RST; // set P7.4 as output

	LCD_SCL_SDA_SEL_REG |= LCD_SCL + LCD_SDA; // set P3.0 and 3.2 to peripheral mode

	UCB0CTL1 |= UCSWRST;                      // **Put state machine in reset**

	// MSB first, master mode, synchronous mode
	UCB0CTL0 |= UCCKPL + UCMSB + UCMST + UCSYNC;
	UCB0CTL1 |= UCSSEL_2;                     // SMCLK
	UCB0BR0 |= 0x00;                          // BRCLK = SMCLK
	UCB0BR1 = 0x00;

	UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

}

void initBacklightPWM(void) {
	LCD_BL_SEL_REG &= ~LCD_BL; // set P2.4 to GPIO mode
	LCD_BL_OUT_REG |= LCD_BL; // turn on LCD_BL for full brightness
	LCD_BL_DIR_REG |= LCD_BL; // set P2.4 as output
}

void delayMilliseconds(uint16_t ms) {
	uint16_t ii;
	for(ii = ms; ii; --ii) {
		__delay_cycles(ONE_MS_DELAY);
	}
}

void initDisplay (void) {

	LCD_RST_OUT_REG &= ~LCD_RST;	// hardware reset
	delayMilliseconds(5);
	LCD_RST_OUT_REG |= LCD_RST;
	delayMilliseconds(120);

	// CS low
	LCD_CS_OUT_REG &= ~LCD_CS;

	WRITECOMM(SWRESET);
	delayMilliseconds(150);

	WRITECOMM(SLPOUT);
	delayMilliseconds(500);

	WRITECOMM(COLMOD);
	WRITEDATA(COLOR_MODE_16_BIT);

	WRITECOMM(INVOFF);

	WRITECOMM(MADCTL);
	WRITEDATA(ROTATE<<5);

	// CS High
	LCD_CS_OUT_REG |= LCD_CS;
}

void displayOn (void) {
	// CS low
	LCD_CS_OUT_REG &= ~LCD_CS;

	WRITECOMM(DISPON); 						 // display on
	delayMilliseconds(100);

	// CS High
	LCD_CS_OUT_REG |= LCD_CS;

}

void clearDisplay (void) {
	// CS low
	LCD_CS_OUT_REG &= ~LCD_CS;

	uint16_t start = Y_OFF;
	uint16_t end = Y_OFF + Y_SIZE - 1;

	WRITECOMM(CASET);
	WRITEDATA(start >> 8);
	WRITEDATA(start & 0xFF);
	WRITEDATA(end >> 8);
	WRITEDATA(end & 0xFF);

	start = X_OFF;
	end = X_OFF + X_SIZE - 1;

	WRITECOMM(RASET);
	WRITEDATA(start >> 8);
	WRITEDATA(start & 0xFF);
	WRITEDATA(end >> 8);
	WRITEDATA(end & 0xFF);

	WRITECOMM(COLMOD);
	WRITEDATA(COLOR_MODE_12_BIT);

	WRITECOMM(RAMWR);

	uint16_t ii;
	for (ii = TOTAL_PIXEL_CNT; ii; --ii) {
		WRITEDATA(0x00);
		WRITEDATA(0x00);
	}

	WRITECOMM(COLMOD);
	WRITEDATA(COLOR_MODE_16_BIT);

	// CS High
	LCD_CS_OUT_REG |= LCD_CS;
}

void setForeColor (displayContext *context, uint8_t r, uint8_t g, uint8_t b) {
	context->foreColor = ((b & 0xf8) << 8) | ((g & 0xfc) << 3) | (r >> 3);
}

void setBackColor (displayContext *context, uint8_t r, uint8_t g, uint8_t b) {
	context->backColor = ((b & 0xf8) << 8) | ((g & 0xfc) << 3) | (r >> 3);
}

// Move current plot position to x,y
void setPosition (displayContext *context, uint8_t x, uint8_t y) {
	context->x0 = x;
	context->y0 = y;
}

// Plot point at x,y
void plotPoint (displayContext context) {
	// CS low
	LCD_CS_OUT_REG &= ~LCD_CS;

	uint16_t start = Y_OFF + context.y0;
	uint16_t end = Y_OFF + context.y0;

	WRITECOMM(CASET);
	WRITEDATA(start >> 8);
	WRITEDATA(start & 0xFF);
	WRITEDATA(end >> 8);
	WRITEDATA(end & 0xFF);

	start = X_OFF + context.x0;
	end = X_OFF + context.x0;

	WRITECOMM(RASET);
	WRITEDATA(start >> 8);
	WRITEDATA(start & 0xFF);
	WRITEDATA(end >> 8);
	WRITEDATA(end & 0xFF);

	WRITECOMM(RAMWR);

	WRITEDATA(context.foreColor >> 8);
	WRITEDATA(context.foreColor & 0xFF);

	// CS high
	LCD_CS_OUT_REG |= LCD_CS;
}

void drawHorLine (displayContext context, uint8_t length) {

	// CS low
	LCD_CS_OUT_REG &= ~LCD_CS;

	uint16_t start = Y_OFF + context.y0;
	uint16_t end = Y_OFF + context.y0;

	WRITECOMM(CASET);
	WRITEDATA(start >> 8);
	WRITEDATA(start & 0xFF);
	WRITEDATA(end >> 8);
	WRITEDATA(end & 0xFF);

	start = X_OFF + context.x0;
	end = X_OFF + context.x0 + length - 1;

	uint8_t numPoints = end - start + 1;

	WRITECOMM(RASET);
	WRITEDATA(start >> 8);
	WRITEDATA(start & 0xFF);
	WRITEDATA(end >> 8);
	WRITEDATA(end & 0xFF);

	WRITECOMM(RAMWR);

	uint8_t ii;
	for (ii = numPoints; ii; --ii) {
		WRITEDATA(context.foreColor >> 8);
		WRITEDATA(context.foreColor & 0xFF);
	}

	// CS high
	LCD_CS_OUT_REG |= LCD_CS;
}

void drawVerLine (displayContext context, uint8_t length) {

	// CS low
	LCD_CS_OUT_REG &= ~LCD_CS;

	uint16_t start = Y_OFF + context.y0 - 1;
	uint16_t end = Y_OFF + context.y0 + length - 1;

	uint8_t numPoints = end - start + 1;

	WRITECOMM(CASET);
	WRITEDATA(start >> 8);
	WRITEDATA(start & 0xFF);
	WRITEDATA(end >> 8);
	WRITEDATA(end & 0xFF);

	start = X_OFF + context.x0;
	end = X_OFF + context.x0;

	WRITECOMM(RASET);
	WRITEDATA(start >> 8);
	WRITEDATA(start & 0xFF);
	WRITEDATA(end >> 8);
	WRITEDATA(end & 0xFF);

	WRITECOMM(RAMWR);

	uint16_t ii;
	for (ii = numPoints; ii; --ii) {
		WRITEDATA(context.foreColor >> 8);
		WRITEDATA(context.foreColor & 0xFF);
	}

	// CS high
	LCD_CS_OUT_REG |= LCD_CS;
}

// Draw a line to x,y
void drawTo (displayContext *context, uint8_t x, uint8_t y) {
	int16_t sx, sy, e2, err;
	int16_t dx = abs(x - context->x0);
	int16_t dy = abs(y - context->y0);

	if (context->x0 < x) {
		sx = 1;
	}
	else {
		sx = -1;
	}

	if (context->y0 < y) {
		sy = 1;
	}
	else {
		sy = -1;
	}

	err = dx - dy;

	for (;;) {

		plotPoint(*context);

		if (context->x0==x && context->y0==y){
			return;
		}

		e2 = err<<1;

		if (e2 > -dy) {
			err = err - dy;
			context->x0 = context->x0 + sx;
		}

		if (e2 < dx) {
			err = err + dx;
			context->y0 = context->y0 + sy;
		}
	}
}

void fillRect (displayContext context, uint8_t w, uint8_t h) {
	// CS low
	LCD_CS_OUT_REG &= ~LCD_CS;

	uint16_t start = context.y0 + Y_OFF;
	uint16_t end = context.y0 + Y_OFF + h-1;

	WRITECOMM(CASET);
	WRITEDATA(start >> 8);
	WRITEDATA(start & 0xFF);
	WRITEDATA(end >> 8);
	WRITEDATA(end & 0xFF);

	start = context.x0 + X_OFF;
	end = context.x0 + X_OFF + w-1;

	WRITECOMM(RASET);
	WRITEDATA(start >> 8);
	WRITEDATA(start & 0xFF);
	WRITEDATA(end >> 8);
	WRITEDATA(end & 0xFF);

	WRITECOMM(RAMWR);

	uint16_t p;
	for (p=w*h*2; p; --p) {
		WRITEDATA(context.foreColor >> 8);
		WRITEDATA(context.foreColor & 0xFF);
	}

	// CS high
	LCD_CS_OUT_REG |= LCD_CS;
}

// Plot an ASCII character with bottom left corner at x,y
void plotChar (displayContext *context, unsigned char c) {

	uint16_t color;
	// CS low
	LCD_CS_OUT_REG &= ~LCD_CS;

	uint16_t start = Y_OFF+context->y0;
	uint16_t end = Y_OFF+context->y0+8*context->textScale-1;

	WRITECOMM(CASET);
	WRITEDATA(start >> 8);
	WRITEDATA(start & 0xFF);
	WRITEDATA(end >> 8);
	WRITEDATA(end & 0xFF);

	start = X_OFF+context->x0;
	end = X_OFF+context->x0+6*context->textScale-1;

	WRITECOMM(RASET);
	WRITEDATA(start >> 8);
	WRITEDATA(start & 0xFF);
	WRITEDATA(end >> 8);
	WRITEDATA(end & 0xFF);

	WRITECOMM(RAMWR);

	uint8_t xx;
	uint8_t xr;
	uint8_t yy;
	uint8_t yr;

	for (xx = 0; xx < 6; ++xx) {

		uint8_t bits = charMap[c-32][xx];

		for (xr=context->textScale; xr; --xr) {

			for (yy = 0; yy < 8; ++yy) {

				if (bits >> (yy) & 1) {
					color = context->foreColor;
				}
				else {
					color = context->backColor;
				}

				for (yr=context->textScale; yr; --yr) {
					WRITEDATA(color >> 8);
					WRITEDATA(color & 0xFF);
				}
			}
		}
	}
	// CS high
	LCD_CS_OUT_REG |= LCD_CS;

	context->x0 = context->x0 + 6*context->textScale;
}

// Plot text starting at the current plot position
void plotText(displayContext *context, char *p) {
	uint8_t i = 0;
	while (p[i] != '\0') {
		plotChar(context, p[i]);
		i=i+1;
	}
}
