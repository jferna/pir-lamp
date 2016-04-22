#include <msp430.h>


void setTimer(void) {
    /*
     * TA0CCTL0, Capture/Compare Control Register 0
     *
     * CM_0 -- No Capture
     * CCIS_0 -- CCIxA
     * ~SCS -- Asynchronous Capture
     * ~SCCI -- Latched capture signal (read)
     * ~CAP -- Compare mode
     * OUTMOD_0 -- PWM output mode: 0 - OUT bit value
     *
     * Note: ~<BIT> indicates that <BIT> has value zero
     */
    TA0CCTL0 = CM_0 | CCIS_0 | OUTMOD_0 | CCIE;

    /* TA0CCR0, Timer_A Capture/Compare Register 0 */
    TA0CCR0 = 5;

    /*
     * TA0CTL, Timer_A3 Control Register
     *
     * TASSEL_1 -- ACLK
     * ID_0 -- Divider - /1
     * MC_1 -- Up Mode
     */
    TA0CTL = TASSEL_1 | ID_0 | MC_1;
}


/*
 * ======== Grace related includes ========
 */
#include <ti/mcu/msp430/Grace.h>


#define PAUSA __delay_cycles(100000);


/*
 *  ======== main ========
 */
int main(void)
{
    Grace_init();                   /* Run Grace-generated initialization */

    unsigned char ledRow = 0;
    unsigned char i;
    unsigned char lowBatt = 0;

    // Introducción
    /*
    // Animación de dentro a fuera.
    P2OUT = 0;
    P2OUT = (1 << 2) | (1 << 3); PAUSA;
    P2OUT = (1 << 1) | (1 << 4); PAUSA;
    P2OUT = (1 << 0) | (1 << 5); PAUSA;
	*/


    // Comparador
    CACTL1 = CARSEL + CAREF0 + CAON;          // 0.25 Vcc = -comp, on
    CACTL2 = P2CA4;                           // P1.1/CA1 = +comp

    if ((CAOUT & CACTL2)) {
    	// hay luz
    	P1OUT |= BIT0;                        // if CAOUT set, set P1.0
    	// Lo detiene todo incluido el timer
    	LPM4;
    	/***************************************/
    }
    ///////////////////////////////////////////

    // adc
    ADC10CTL0 = SREF_1 + ADC10SHT_2 + REFON + ADC10ON + ADC10IE;
    __delay_cycles(10000);
    ADC10CTL1 = INCH_2;                       // input A2
    ADC10AE0 |= 0x04;                         // PA.2 ADC option select



    ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
    __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exi

    if (ADC10MEM < 600) {
    	//P2OUT |= BIT2 |BIT1 | BIT0;
    	//LPM4;
    	lowBatt = 1;
    }

    ////////////////////////////



    // intro
    P2OUT = 0;
    if (lowBatt) {
        for (i=0; i< 3; i++) {
        	P2OUT |= (1<< i); PAUSA;
        }
    } else {
        for (i=0; i< 6; i++) {
        	P2OUT |= (1<< i); PAUSA;
        }

    }


    setTimer();

    // Bucle infintio
    while (1) {
    	LPM3;
        //P1OUT ^= BIT0;              /* Toggle LED on P1.0 */
    	P2OUT = 1 << (ledRow++);
    	if (lowBatt) {
    		if (ledRow == 3) ledRow = 0;
    	} else {
    		if (ledRow == 6) ledRow = 0;
    	}


    }
}


// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
  __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}


#pragma vector=TIMER0_A0_VECTOR
__interrupt void timer0_ISR(void)
{
	__bic_SR_register_on_exit(LPM4_bits);
}
