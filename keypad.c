/* keypad.c
 *
 * Author: Alexander Swearingen
 * Created on: 01/31/24
 * 4x4 Keypad Initial Code Example (1/31/24)
 *
 */


#include <msp430.h> 

//Function declarations
char readRows();                      //returns row index from keypad press
char readCol();                       //returns col index from keypad press
char keyValue(char row, char col);    //returns value of row and col pair
void checkValue(char val);            //no return, determines if value matches set target
void gpioInit();                      //gpio initialization

/*  Pin Allocation
 *  P2.0 = Col 1    P5.0 = Row 1
 *  P2.1 = Col 2    P5.1 = Row 2
 *  P2.2 = Col 3    P5.2 = Row 3
 *  P2.4 = Col 4    P5.3 = Row 4
 *  P6.0 = BLUE P6.1 = GREEN P6.2 = RED
 *
 *  Keypad Indexing
 *  Rows: starting at the 1 key, Row 0 --> Row 3
 *  Cols: starting at the 1 key, Col 0 --> Col 3
 *  Example: B key is Row 1, Col 3
 *           7 key is Row 2, Col 0
 */

//Macro definitions for Rows & Columns
#define ROW1 BIT0
#define ROW2 BIT1
#define ROW3 BIT2
#define ROW4 BIT3
#define COL1 BIT0
#define COL2 BIT1
#define COL3 BIT2
#define COL4 BIT4

//global variables
const char rowMask = ROW1 + ROW2 + ROW3 + ROW4;
const char colMask =  COL1 + COL2 + COL3 + COL4;
const char targetValue = 7;

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	//stop watchdog timer
	
	gpioInit();                 //Initialize LED & Keypad

	PM5CTL0 &= ~LOCKLPM5;

	char row = 0; //no button pressed
	char col = 0;
	char val = 0;

	while(1){
	    row = readRows();
	    col = readCol();
	    val = keyValue(row, col);
	    checkValue(val);
	}
	return 0;
}


void gpioInit(){
//Pin/LED Initialization
//Initialize Green LED on 2355 board
    P6DIR |= BIT6;                  //P6.6 to output direction
    P6SEL0 &= ~BIT6;                //Change SEL0/1 Reg to GPIO
    P6SEL1 &= ~BIT6;

    //Initialize keypad
    P5DIR &= ~rowMask;              //Set Rows as Inputs
    P5REN |= rowMask;               //Set pins with Pull-Down Resistor
    P5OUT |= rowMask;               //Set P5OUT to 1

    P2DIR &= ~colMask;              //Set Columns as Inputs
    P2REN |= colMask;
    P2OUT |= colMask;               //Drive Columns to 1

}

void checkValue(char val){
//check val (keypad value) against target and set GREEN LED based on result
    if(val == targetValue){
        P6OUT |= BIT6;              //turn GREEN LED on
    }else{
        P6OUT &= ~BIT6;             //turn GREEN LED off
    }
}

char keyValue(char row, char col){
    //if needed change value to keyVal
    char value = (row<<2) + col;  //equivalent to (row * 4) + col
    if(value >= 15){
        return 0xFF;
    }else{
        return value;
    }
}

char readRows(){
//Initialize Rows to Inputs, Cols to outputs
        P2DIR |= colMask;           //Set Columns as Outputs
        P2OUT |= colMask;           //Drive Columns to 1

        P5DIR &= ~rowMask;          //Set Rows as Inputs
        P5REN |= rowMask;           //Set pins with Pull-Down Resistor
        P5OUT &= ~rowMask;          //Set P5OUT to 0

        char rowVal = (P5IN & rowMask);
        switch(rowVal){
        case ROW1: return 0;
                   break;
        case ROW2: return 1;
                   break;
        case ROW3: return 2;
                   break;
        case ROW4: return 3;
                   break;
        default: return 0xFF;
                 break;
        }
}

char readCol(){
//Initialize Cols to Inputs, Rows to outputs
            P5DIR |= rowMask;       //Set rows as Outputs
            P5OUT |= rowMask;       //Set row output to 1

            P2DIR &= ~colMask;      //Set Cols as Inputs
            P2REN |= colMask;       //Set pins with Pull-Down Resistor
            P2OUT &= ~colMask;      //Set P2OUT to 0

            char rowVal = (P5IN & rowMask);
            switch(rowVal){
            case COL1: return 0;
                       break;
            case COL2: return 1;
                       break;
            case COL3: return 2;
                       break;
            case COL4: return 3;
                       break;
            default: return 0xFF;
                     break;
            }
}
