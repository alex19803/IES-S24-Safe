/*
 * buzzer.c
 *
 * Author: Alexander Swearingen
 * Created on: 04/03/2024
 *
 */

#include <msp430.h>

void buzzerOn();

int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;      //Stop watchdog timer

    P4DIR &= ~BIT1;                //setting P4.1 as input
    P4REN |= BIT1;                 //set pullup/pulldown resistor
    P4OUT |= BIT1;

    P6OUT &= ~BIT3;                //Clear P6.3 output pin for power-on state
    P6DIR |= BIT3;                 //Set P6.3 to output direction
    P6SEL0 &= ~BIT3;
    P6SEL1 &= ~BIT3;

    PM5CTL0 &= ~LOCKLPM5;                   //Disable the GPIO power-on default high-impedance mode
                                            //to activate previously configured port setting

    while(1)
    {
        if((P4IN & BIT1)== 0){
            buzzerOn();
        }
    }
    return 0;
}

void buzzerOn(){

    P6OUT ^= BIT3;
    __delay_cycles(10000);
    P6OUT &= ~BIT3;
    __delay_cycles(10000);

}
