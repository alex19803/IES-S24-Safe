/*
* latch.c
* 
* Author: Alexander Swearingen
* Created on: 04/03/2024
*
* latch/solenoid example code with board button input allowing for engaging and disengaging of the latch
*/

#include <msp430.h> 

//Program Function Declarations
void gpioInit();                
void latchEngage(char ON);
void latchDisengage(char OFF); 

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    gpioInit();			//gpio initialization call
  
    //setup interrupt on P4.1 button input
    P4IES |= BIT1;     
    P4IE |= BIT1;  
    P4IFG &= ~BIT1;
   
    PM5CTL0 &= ~LOCKLPM5;       
                                            
    __bis_SR_register(LPM0_bits | GIE);           
    __no_operation();                             
    return 0;
}

void gpioInit(){
        P4DIR |= BIT4;
        P4SEL0 &= ~BIT4;
        P4SEL1 &= ~BIT4;

        P4DIR &= ~BIT1;                 //set P4.1 as input
        P4REN |= BIT1;                  //activate pullup/pulldown resistor
        P4OUT |= BIT1;
}

#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    P4IFG &= ~BIT1;  // Clear P4.1 IFG
    latchEngage(1);
    __delay_cycles(500000);
    latchDisengage(1);
    __delay_cycles(500000);
}

void latchEngage(char ON){
    if(ON == 1){
        P4OUT |= BIT4;
    }
}

void latchDisengage(char OFF){
    if(OFF == 1){
        P4OUT &= ~BIT4;
    }
}
