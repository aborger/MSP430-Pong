#include <msp430.h> 
#include <stdio.h>
#include <string.h>
#include "UCS.h"
#include "tinyTFT.h"

#define UNPRESSED   0
#define PRESSED     1
#define NUM_TASKS   5

#define RX_TASK     0
#define RX_HEX      0x00
#define PG_TASK     1
#define PG_HEX      0x10
#define CC_TASK     2
#define CC_HEX      0x20
#define TX_TASK     3
#define TX_HEX      0X30
#define UD_TASK     4
#define UD_HEX      0x40
#define SC_HEX      0X50

#define BLACK       0x0000
#define BLUE        0xF800
#define RED         0x001F
#define GREEN       0x07E0
#define CYAN        0xFFE0
#define MAGENTA     0xF81F
#define YELLOW      0x07FF
#define WHITE       0xFFFF

// Global Variables
displayContext currentContext;
uint8_t rx;

/*---------------------------------------------------------------------------------------------------- *\
 *                                          Function Prototype                                          *
 *-----------------------------------------------------------------------------------------------------*/
void write(uint8_t transmition);
uint8_t S1State(void);
uint8_t S2State(void);
void write(uint8_t transmition);
void display(uint8_t score);
void itoa(int n, char s[]);

/*---------------------------------------------------------------------------------------------------- *\
 *                                             State Machines                                          *
 *-----------------------------------------------------------------------------------------------------*/

typedef struct{
    int state;
    unsigned long period;
    unsigned long elapsedTime;
    uint8_t i;
    uint8_t rx;
    uint8_t tx;
}Task;

typedef struct{
    Task vals;
    Task (*TickFctn)(Task);
} TaskClass;

TaskClass tasks[NUM_TASKS];

// RX task, does not use extra variables
enum RX_States {RX_SMStart, RX_Ready, RX_Game, RX_Score};
// PG task, uses rx, and tx
enum PG_States {PG_SMStart, PG_Waiting, PG_Ready, PG_Play};
// CC task, uses i, and tx
enum CC_States {CC_SMStart, CC_White, CC_Blue, CC_Red, CC_Green, CC_Cyan, CC_Magenta, CC_Yellow};
// TX task, does not use extra variables
enum TX_States {TX_SMStart, TX_Game, TX_Color};
// UD task, uses rx
enum UD_States {UD_SMStart, UD_Display};



Task RX_TickFctn(Task task) {
    switch ( task.state ) {
        case RX_SMStart:
            task.state = RX_Ready;
            break;
        case RX_Ready:
            if ((rx & 0xF0) == PG_HEX) {
                task.state = RX_Game;
            } else if ((rx & 0xF0) == SC_HEX) {
                task.state = RX_Score;
            }
            break;
        case RX_Game:
            if ((rx & 0xF0) == SC_HEX) {
                task.state = RX_Score;
            } else {
                task.state = RX_Ready;
            }
            break;
        case RX_Score:
            if ((rx & 0xF0) == PG_HEX) {
                task.state = RX_Game;
            } else {
                task.state = RX_Ready;
            }
            break;
        default:
            task.state = RX_Ready;
            break;
    }

    // States
    switch ( task.state ) {
        case RX_SMStart:
            break;
        case RX_Ready:

            break;
        case RX_Game:
            tasks[PG_TASK].vals.rx = rx;
            break;
        case RX_Score:
            tasks[UD_TASK].vals.rx = rx;
            break;
    }
    return task;
}


Task PG_TickFctn(Task task) {
    // Transitions
    switch ( task.state ) {
        case PG_SMStart:
            task.state = PG_Waiting;
            break;
        case PG_Waiting:
            if (S1State() == PRESSED) {
                task.state = PG_Ready;
            }
            break;
        case PG_Ready:
            if (task.rx == PG_HEX + PG_Ready) {
                task.state = PG_Ready;
            } else if (task.rx == PG_HEX + PG_Play) {
                task.state = PG_Play;
            }
            break;
        case PG_Play:
            if (task.rx == PG_HEX + PG_Waiting) {
                task.state = PG_Waiting;
            } else if (task.rx == PG_HEX + PG_Play) {
                task.state = PG_Play;
            }
            break;
    }
    switch ( task.state ) { // States
        case PG_SMStart:
            break;
        case PG_Waiting:
            task.tx = PG_HEX + PG_Waiting;
            break;
        case PG_Ready:
            task.tx = PG_HEX + PG_Ready;
            break;
        case PG_Play:
            ADC12CTL0 |= ADC12SC;
            if(ADC12IFG & BIT0) {
                task.tx = (ADC12MEM0 >> 1);
                ADC12CTL0 &= ~ADC12SC;
            }
            break;
    }
    return task;
}

Task CC_TickFctn(Task task) {
    // Transitions
    if (task.state == CC_SMStart) {
        task.state = CC_White;
    }

    if (S2State() == PRESSED) {
        // Button Pressed longer than 2 ticks
        if (task.i == 1) {
            // Next state
            if (task.state < 7) {
                task.state += 1;
            } else {
                task.state = CC_White;
            }
            task.i +=1;
        }
        else {
            // Iterate i
            task.i += 1;
        }
    } else {
        task.i = 0;
    }

    // States
    switch ( task.state ) {
        case CC_SMStart:
            break;
        case CC_White:
            currentContext.foreColor = WHITE;
            break;
        case CC_Blue:
            currentContext.foreColor = BLUE;
            break;
        case CC_Red:
            currentContext.foreColor = RED;
            break;
        case CC_Green:
            currentContext.foreColor = GREEN;
            break;
        case CC_Cyan:
            currentContext.foreColor = CYAN;
            break;
        case CC_Magenta:
            currentContext.foreColor = MAGENTA;
            break;
        case CC_Yellow:
            currentContext.foreColor = YELLOW;
            break;
        default:
            break;
    }
    // Transmit current state
    task.tx = CC_HEX | task.state;
    return task;
}


Task TX_TickFctn(Task task) {
    // Transitions
        switch ( task.state ) {
            case TX_SMStart:
                task.state = TX_Game;
                break;
            case TX_Game:
                task.state = TX_Color;
                break;
            case TX_Color:
                task.state = TX_Game;
                break;
        }
        // States
        switch( task.state ) {
            case TX_SMStart:
                break;
            case TX_Game:
                write(tasks[PG_TASK].vals.tx);
                break;
            case TX_Color:
                write(tasks[CC_TASK].vals.tx);
                // Change lcd color
                break;
        }
    return task;
}

Task UD_TickFctn(Task task) {
    char score[1];
    // Transitions
    switch (task.state) {
        case UD_SMStart:
            task.state = UD_Display;
            break;
        case UD_Display:
            task.state = UD_Display;
            break;
    }

    // States
    switch (task.state) {
        case UD_SMStart:
            break;
        case UD_Display:
            currentContext.x0 = 60;
            currentContext.y0 = 60;
            clearDisplay();
            itoa(task.rx, score);
            plotChar(&currentContext, score[1]);
            break;
    }
    return task;
}


/*---------------------------------------------------------------------------------------------------- *\
 *                                               Helper Functions                                     *
 *-----------------------------------------------------------------------------------------------------*/

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
    UCA0TXBUF = transmition;
}



uint8_t S1State(void) {
    if (!(P4IN & BIT0)) {
        return PRESSED;
    } else {
        return UNPRESSED;
    }
}

uint8_t S2State(void) {
    if (!(P3IN & BIT7)) {
        return PRESSED;
    } else {
        return UNPRESSED;
    }
}

void reverse(char s[])
 {
     int i, j;
     char c;

     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }

void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}

/*---------------------------------------------------------------------------------------------------- *\
 *                                            Initialize Functions                                     *
 *-----------------------------------------------------------------------------------------------------*/

void initTimerB0(void) {
    TB0CTL |= TBSSEL_1 + ID_3 + MC__UP + TBCLR; // Select ACLK source (32768 Hz), divide by 8, UP mode, clear timer logic
    TB0CCTL0 |= CCIE; // enable CCR0 interrupt

    TB0EX0 |= TBIDEX__4; // divide ACLK by 4
    TB0CCR0 = 5; // Period is 1 ms, so CCR0 is 5 to achieve interrupt every 5 ms.
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

void initPorts() {
    P1SEL = 0x00;       // Set all of P1 to be GPIO
    P3SEL = 0x00;       // Set all of P1 to be GPIO
    P4SEL = 0x00;       // Set all of P4 to be GPIO

    P1DIR |= BIT0;      // Set P1.0 (LED1) to output
    P4DIR |= BIT7;      // Set P4.7 (LED2) to output

    P3SEL |= BIT3 + BIT4;   // Set Rx and Tx pins to output
    P6SEL = BIT1;           // Set IMU.y pin to output

    P3REN |= BIT7;          // Enable resistor for P3.7 (S2_BOOST)
    P4REN |= BIT0;          // Enable resistor for P4.0 (S1_BOOST)

    P3OUT |= BIT7;          // Set P3.7 (S2_BOOST) to have pullup resistor
    P4OUT |= BIT0;          // Set P4.0 (S1_BOOST) to have pullup resistor



    P1DIR |= BIT0;          // Set P4.0 (LED1_LP) to output
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
/*---------------------------------------------------------------------------------------------------- *\
 *                                                   Main                                                *
 *-----------------------------------------------------------------------------------------------------*/
int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	initPorts();

	initClocks(SYS_CLOCK_FREQ_16000_KHZ);

	initUART();

	initADC();

	initTimerB0();

	initSPI();
	initDisplay();
	initBacklightPWM();
	displayOn();
	clearDisplay();

	currentContext.x0 = 60;
    currentContext.y0 = 60;
    currentContext.foreColor = WHITE;
    currentContext.backColor = BLACK;
    currentContext.textScale=3;


	int i;
	for (i = 0; i < NUM_TASKS; i++) {
	    tasks[i].vals.state = 0;
	    tasks[i].vals.elapsedTime = 0;
	}
	tasks[RX_TASK].vals.period = 2; // 2 ms
	tasks[RX_TASK].TickFctn = RX_TickFctn;

	tasks[PG_TASK].vals.period = 1; // 1 ms
	tasks[PG_TASK].TickFctn = PG_TickFctn;
	tasks[PG_TASK].vals.rx = 0;
	tasks[PG_TASK].vals.tx = 0;


	tasks[CC_TASK].vals.period = 5; // 5 ms
	tasks[CC_TASK].TickFctn = CC_TickFctn;
	tasks[CC_TASK].vals.i = 0;
	tasks[CC_TASK].vals.rx = 0;
	tasks[CC_TASK].vals.tx = 0;

	tasks[TX_TASK].vals.period = 1; // 1 ms;
	tasks[TX_TASK].TickFctn = TX_TickFctn;

	tasks[UD_TASK].vals.period = 200;
	tasks[UD_TASK].TickFctn = UD_TickFctn;

	__enable_interrupt(); // set the global interrupt enable (GIE) bit


	while(1) {}
}

 /*---------------------------------------------------------------------------------------------------- *\
  *                                                   Interrupts                                          *
  *-----------------------------------------------------------------------------------------------------*/

#pragma vector = USCI_A0_VECTOR
__interrupt void TransmitInterrupt(void) {
    switch(__even_in_range(UCA0IV,4)) {
        case 0:             // Vector 0 - no interrupt
            break;
        case 2:             // Vector 2 - RXIFG
            setRxLED();
            rx = UCA0RXBUF;
            break;
        case 4:             // Vector 4 - TXIFG
            setTxLED();
            break;
        default: break;
    }
}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void TIMERB0_ISR(void) {
    unsigned int i = 0;
    for (i = 0; i < NUM_TASKS; i++) {
        if (tasks[i].vals.elapsedTime >= tasks[i].vals.period) {
            tasks[i].vals = tasks[i].TickFctn(tasks[i].vals);
            tasks[i].vals.elapsedTime = 0;
        }
        tasks[i].vals.elapsedTime += 1;
    }
    clearRxLED();
    clearTxLED();
}

