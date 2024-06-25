/*
 * rgbled.c
 *
 *  Created on: Feb 14, 2024
 *      Author: Alexander Swearingen
 */

#include <msp430.h>

void setRGB(unsigned int red, unsigned int green, unsigned int blue);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                 //Stop watchdog timer

    P6DIR |= BIT0 | BIT1 | BIT2;             //configure direction of output to LED
    P6SEL0 |= BIT0 | BIT1 | BIT2;

    //Disable the GPIO power-on default high-impedance mode to activate
    //previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    TB3CCR0 = 1000;                             //setting CCR register PWM periods, CCR0 period is 1000, CCR1 to 3 initially zero, changes with call to setRGB function
    TB3CCTL1 = OUTMOD_7;                        //CCR1 reset/set
    TB3CCR1 = 0;                              
    TB3CCTL2 = OUTMOD_7;                        //CCR2 reset/set
    TB3CCR2 = 0;                              
    TB3CCTL3 = OUTMOD_7;                        //CCR3 reset/set
    TB3CCR3 = 0;                              
    TB3CTL = TBSSEL__SMCLK | MC__UP | TBCLR;    //select SMCLK, up mode, clear timer

    __bis_SR_register(GIE);            
    __no_operation();

    //initialize RGB pins
    P6OUT &= ~BIT0;

    P6OUT &= ~BIT1;

    P6OUT &= ~BIT2;



    setRGB(0, 1000, 1000);       //set RGB to red using timer priority/overflow
    __delay_cycles(300000);      //delay
    setRGB(1000, 0, 1000);       //set RGB to green using timer priority/overflow
    __delay_cycles(300000);      //delay
    setRGB(1000, 1000, 0);       //set RGB to blue using timer priority/overflow
    __delay_cycles(300000);      //delay

    return 0;

}

void setRGB(unsigned int red, unsigned int green, unsigned int blue)
{
    //assign PWM duty cycles for RGB component
    TB3CCR1 = red;   
    TB3CCR2 = green; 
    TB3CCR3 = blue;   
}
