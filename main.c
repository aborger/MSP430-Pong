#include <msp430.h> 
#include "UCS.h"

void initPorts() {
    P1SEL = 0x00;       // set all of P1 to be GPIO
    P4SEL = 0x00;       // set all of P4 to be GPIO

    P1DIR |= BIT0;      // Set P1.0 (LED1) to output
    P4DIR |= BIT7;      // Set P4.7 (LED2) to output

    P3SEL |= BIT3 + BIT4;   // Set Rx and Tx pins to output

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

void write() {
   UCA0TXBUF = 0x01;
}

void writeEnd() {
    UCA0TXBUF = 'b';
}

void initUART() {
    UCA0CTL1 |= UCSWRST;    // put state machine in reset
    UCA0CTL1 |= UCSSEL_2;   // SMCLK
    UCA0MCTL |= UCBRS_1;    // Modulation
    //UCA0MCTL |= UCBRF_0;    // Modulation

    UCA0BR0 = 104;
    UCA0BR1 = 1;

    UCA0CTL1 &= ~UCSWRST;  // Initialize USCI state machine

    UCA0IE |= UCTXIE;       // Enables Tx interrupt
    UCA0IE |= UCRXIE;       // Enables Rx interrupt


}
/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	initPorts();

	initClocks(SYS_CLOCK_FREQ_25000_KHZ);

	initUART();


	__enable_interrupt(); // set the global interrupt enable (GIE) bit

	int ii = 0;
	while(1) {
	    write();
	    __delay_cycles(10000000);
	    clearRxLED();
	    clearTxLED();
	    __delay_cycles(10000000);
	    ii += 1;
	}
	writeEnd();


	//__bis_SR_register(LPM0_bits + GIE);


	return 0;
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

