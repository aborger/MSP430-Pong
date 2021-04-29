#include <msp430.h> 
#include "UCS.h"

#define UNPRESSED 0
#define PRESSED 1


enum PG_States {PG_SMStart, PG_Waiting, PG_Ready, PG_Play} PG_State;


uint8_t S1State(void) {
    if (!(P4IN & BIT0)) {
        return PRESSED;
    } else {
        return UNPRESSED;
    }
}

int PG_TickFctn(void) {
    switch ( PG_State ) { // Transitions
        case PG_SMStart:
            PG_State = PG_Waiting;
            break;
        case PG_Waiting:
            if (S1State() == PRESSED) {
                PG_State = PG_Ready;
            }
            break;
        case PG_Ready:
            if (UCA0RXBUF == 1) {
                PG_State = PG_Ready;
            } else if (UCA0RXBUF == 2) {
                PG_State = PG_Play;
            }
            break;
        case PG_Play:
            if (UCA0RXBUF == 1) {
                PG_State = PG_Waiting;
            } else if (UCA0RXBUF == 2) {
                PG_State = PG_Play;
            }
            break;
    }
    switch ( PG_State ) { // States
        case PG_SMStart:
            break;
        case PG_Waiting:
            write(1);
            break;
        case PG_Ready:
            write(2);
            break;
        case PG_Play:
            ADC12CTL0 |= ADC12SC;
            while(!(ADC12IFG & BIT0)) {}
            write(ADC12MEM0 >> 1);
            ADC12CTL0 &= ~ADC12SC;
            break;
    }
}

void initPorts() {
    P1SEL = 0x00;       // set all of P1 to be GPIO
    P4SEL = 0x00;       // set all of P4 to be GPIO

    P1DIR |= BIT0;      // Set P1.0 (LED1) to output
    P4DIR |= BIT7;      // Set P4.7 (LED2) to output

    P3SEL |= BIT3 + BIT4;   // Set Rx and Tx pins to output
    P6SEL = BIT1;           // Set IMU.y pin to output

    P4REN |= BIT0;          // Enable resistor for P4.0 (S1_LP)
    P4OUT |= BIT0;          // Set P4.0 (S1_LP) to have pullup resistor

    P1DIR |= BIT0;          // Set P4.0 (LED1_LP) to output
}

void setTxLED() {
    P1OUT |= BIT0;
}

void setRxLED() {
    P4OUT |= BIT7;
}

void clearTxLED() {
    P1OUT &= ~BIT0;
}

void clearRxLED() {
    P4OUT &= ~BIT7;
}

void write(uint8_t transmition) {
   //UCA0TXBUF = 0x01;

    UCA0TXBUF = transmition;

}

void initTimerB0(void) {
    TB0CTL |= TBSSEL_1 + ID_3 + MC__UP + TBCLR; // Select ACLK source (32768 Hz), divide by 8, UP mode, clear timer logic
    TB0CCTL0 |= CCIE; // enable CCR0 interrupt

    TB0EX0 |= TBIDEX__4; // divide ACLK by 4
    TB0CCR0 = 25; // Period is 1 ms, so CCR0 is 25 to achieve interrupt every 25 ms.
}

void initUART() {
    UCA0CTL1 |= UCSWRST;    // put state machine in reset
    UCA0CTL1 |= UCSSEL_2;   // SMCLK
    UCA0BRW = 1666;
    UCA0MCTL = UCBRS_6 + UCBRF_0;

    UCA0CTL1 &= ~UCSWRST;  // Initialize USCI state machine

    UCA0IE |= UCTXIE;       // Enables Tx interrupt
    UCA0IE |= UCRXIE;       // Enables Rx interrupt


}

void initADC(void) {
    /* Initialize ADC12_A */
    /*
    ADC12CTL0 |= ADC12ON+ADC12MSC+ADC12SHT0_2;          // Turn on ADC12, enable multiple conversions, set sampling time to 16 ADC12CLK cycles
    ADC12CTL1 |= ADC12SHP+ADC12CONSEQ_1+ADC12SSEL_3;    // Use sampling timer, single sequence mode, SMCLK (25 MHz)
    ADC12CTL2 &= ~(ADC12RES0 + ADC12RES1);              // set ADC to 8-bit resolution
    ADC12MCTL0 |= ADC12INCH_5;                          // ref+=AVcc = 3.3VDC, ref-=AVss = 0V, channel = A5
    ADC12MCTL1 |= ADC12INCH_3+ADC12EOS;                 // ref+=AVcc = 3.3VDC, ref-=AVss = 0V, channel = A3, this channel is the end of the conversion sequence
    ADC12CTL0 |= ADC12ENC;                              // Enable conversions

    */

    ADC12CTL0 |= ADC12ON+ADC12SHT0_2;          // Turn on ADC12, enable multiple conversions, set sampling time to 16 ADC12CLK cycles
    ADC12CTL1 |= ADC12SHP+ADC12CONSEQ_2+ADC12SSEL_3;    // Use sampling timer, single sequence mode, SMCLK (25 MHz)
    ADC12CTL2 &= ~(ADC12RES0 + ADC12RES1);              // set ADC to 8-bit resolution
    ADC12MCTL0 |= ADC12INCH_1;                          // ref+=AVcc = 3.3VDC, ref-=AVss = 0V, channel = A5
    ADC12CTL0 |= ADC12ENC;

}
/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	initPorts();

	initClocks(SYS_CLOCK_FREQ_16000_KHZ);

	initUART();

	initADC();


	__enable_interrupt(); // set the global interrupt enable (GIE) bit



	while(1) {
	    PG_TickFctn();
        clearRxLED();
        clearTxLED();
        __delay_cycles(10000);
	};
}

#pragma vector = USCI_A0_VECTOR
__interrupt void TransmitInterrupt(void) {
    switch(__even_in_range(UCA0IV,4)) {
        case 0:             // Vector 0 - no interrupt
            break;
        case 2:             // Vector 2 - RXIFG
            setRxLED();
            break;
        case 4:             // Vector 4 - TXIFG
            setTxLED();
            break;
        default: break;
    }
}

