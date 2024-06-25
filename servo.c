/*
 * servo.c
 * 
 * Author: Alexander Swearingen
 * Created on: 04/03/2024
 * 
 * Example code for basic, button based, operation of a servo motor.
 */

#include <msp430.h>

void ServoOp(int angle);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;          //stop watchdog timer

    P4DIR &= ~BIT1;                     //set P4.1 button on MSP430 board as input
    P4REN |= BIT1;                      //enable pullup/pulldown resistor
    P4OUT |= BIT1;

    P4IES |= BIT1;                      //setting interrupt to trigger on the falling edge
    P4IE |= BIT1;                       //interrupt enable
    P4IFG &= ~BIT1;

    P6DIR |= BIT4;                      // P6.4 output direction set
    P6SEL0 |= BIT4;                   

    PM5CTL0 &= ~LOCKLPM5;
  
    TB3CCR0 = 20000;                         //initial CCR0PWM Period
    TB3CCTL5 = OUTMOD_7;                     //setup CCR1 reset/set
    TB3CCR5 = 0;                             //set CCR1 PWM duty cycle initially as 0
    TB3CTL = TBSSEL__SMCLK | MC__UP | TBCLR; //set up timer with SMCLK, up mode, and clear timer register
    __bis_SR_register(GIE);             
    __no_operation();                         
}

#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    P4IFG &= ~BIT1;
    ServoOp(0);
    __delay_cycles(250000);
    ServoOp(90);
    __delay_cycles(250000);
    ServoOp(180);
    __delay_cycles(250000);
  
}

void ServoOp(int angle){
switch(angle){
case 0: TB3CCR5 = 550;
break;
case 90: TB3CCR5 = 1500;
break;
case 180: TB3CCR5 = 2500;
break;
}




