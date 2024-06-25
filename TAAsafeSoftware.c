/*
 * TAAsafeSoftware.c
 *
 * - This program is group TAA's code used for final demonstration of our boards operation.
 * - For this program the external keypad and on-board switch are the only ways to get some sort of output from the board.
 * - Upon keypad press the buzzer will turn on.
 * - When switch on board would be pressed the two pin LED would turn off.
 *
 * Author: Alexander Swearingen
 * Completion Date: 05/02/2024
 *
 */

#include <msp430.h>

//defining bit macros for keypad rows/columns
#define ROW1 BIT0   //Row 1 P5.0
#define ROW2 BIT1   //Row 2 P5.1
#define ROW3 BIT2   //Row 3 P5.2
#define ROW4 BIT3   //Row 4 P5.3
#define COL1 BIT0   //Col 1 P2.0
#define COL2 BIT1   //Col 2 P2.1
#define COL3 BIT2   //Col 3 P2.2
#define COL4 BIT4   //Col 4 P2.4

const char rowMask = ROW1 + ROW2 + ROW3 + ROW4;
const char colMask = COL1 + COL2 + COL3 + COL4;

/*
 * OLED Display Connections
 * P1.2 from board to OLED SDA
 * P1.3 from board to OLED SCL
 * SCL from environmental sensor to OLED SCL
 * SDA from environmental sensor to OLED SDA
 */

//Function Declarations used for program
void gpioInit();                                                                //general purpose input/output initialization
char readCols();                                                                //reading keypad columns
char readRows();                                                                //reading keypad rows
char keyValue(char row, char col);                                              //function to read key value when keypad is pressed
void checkValue(int attempt[], int password[]);                                 //function to compare user attempted input with manually set password
void buzzerOn(int setTime);                                                     //function to operate buzzer
void ServoOp(int angle);                                                        //function for changing servo angle
void latchEngage(char ON);                                                      //function for latch engage
void latchDisengage(char OFF);                                                  //function to disengage latch
void setRGB(unsigned int red, unsigned int green, unsigned int blue);           //function to set color output of RGB LED


int keyVal = 0;
char colVal = 0;
char rowVal = 0;

int password[4] = {6, 5, 4, 2};     //program set password for demonstration
int attempt[4];                     //creating user attempt array with maximum of 4 integer values
unsigned int i;                     //int i declaration for use in programs multiple for loops.
unsigned int index = 0;             //array index used to keep track of user attempt array

int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;
    gpioInit();
    PM5CTL0 &= ~LOCKLPM5;
    __bis_SR_register(GIE);
    __no_operation();

    //return 0; //return eliminated to keep timer/interrupt routine running for demonstration

}

void gpioInit(){
    //Initialize Used Pins
    P5REN &= ~rowMask;  // Disable Pulldown Resistor
    P5DIR |= rowMask;   // Set rows outputs
    P5OUT |= rowMask;   // Set all Rows to 1

    P2DIR &= ~colMask;  // Set cols to inputs
    P2REN |= colMask;   // Set cols to use Pulldown resistor
    P2OUT &= ~colMask;

    P2IES &= ~colMask;  // Set interrupt to trigger on falling edge
    P2IE |= colMask;    // Enable interrupt for button pin
    P2IFG &= ~colMask;

    P4DIR |= BIT4;      //solenoid/latch pin direction/output set up
    P4SEL0 &= ~BIT4;
    P4SEL1 &= ~BIT4;
    P4OUT &= ~BIT4;

    P6OUT &= ~BIT3;     //buzzer pin direction/output set up
    P6DIR |= BIT3;
    P6SEL0 &= ~BIT3;
    P6SEL1 &= ~BIT3;


    P6DIR |= BIT0 | BIT1 | BIT2;                //P6.0, 6.1, 6.2 output directions
    P6SEL0 |= BIT0 | BIT1 | BIT2;               //P6.0, 6.1, 6.2 option select to 1


    TB3CCR0 = 20000;                            //setting Duty Cycle Period
    TB3CCTL1 = OUTMOD_7;                        //setting CCR1 reset/set
    TB3CCR1 = 20000;                            //assigning initial value to CCR1 duty cycle
    TB3CCTL2 = OUTMOD_7;                        //setting CCR2 reset/set
    TB3CCR2 = 20000;                            //assigning initial value to CCR2 duty cycle
    TB3CCTL3 = OUTMOD_7;                        //setting CCR3 reset/set
    TB3CCR3 = 20000;                            //assigning initial value to CCR3 duty cycle
    TB3CCTL5 = OUTMOD_7;                        //setting CCR5 reset/set
    TB3CCR5 = 0;                                //assigning initial value to CCR5 duty cycle
    TB3CTL = TBSSEL__SMCLK | MC__UP | TBCLR;    //selecting smclk, up mode, and clear for timer control

    P6OUT &= ~BIT0;     //configuring out
    P6OUT &= ~BIT1;
    P6OUT &= ~BIT2;

    P6DIR |= BIT4;      //setting pin out direction for servo
    P6SEL0 |= BIT4;

}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{

    __delay_cycles(5000);        //delay to reduce effect of keypad debouncing
    rowVal = readRows();
    colVal = readCols();

    keyVal = keyValue(rowVal, colVal);

    attempt[index] = keyVal;
    index++;;

            // red green blue
    setRGB(20000, 20000, 0);       //call to RGB LED function to turn on as BLUE when button pressed, commented out due to RGB LED not working on board.
    buzzerOn(300);
    __delay_cycles(600000);
    setRGB(20000, 20000, 20000);    //call to RGB LED function turn off all colors of LED
    __delay_cycles(50000);

    if(index >= 4){                     //checking keypad input when # of inputs exceeds 4 integers, compare user attempt with set password.
        checkValue(attempt, password);
    }

    for (i = 0; i < 4; i++) {
        if (attempt[i] > 14) {   //checking if a improper keypad value is read due to debouncing
            attempt[i] = 0;      //setting the attempt position with impossible kaypad value to 0
            index--;             //decrease number of attempts by 1.
        }
    }
    P2IFG &= ~colMask;
}

char readCols(){
    P5REN &= ~rowMask;  //disabling pulldown resistoron rows
    P5DIR |= rowMask;   //setting row output direction
    P5OUT |= rowMask;   //setting all rows to 1

    P2DIR &= ~colMask;  // Set cols to inputs
    P2REN |= colMask;   //enabling pulldown resistor on columns
    P2OUT &= ~colMask;

    char cols = P2IN & colMask; //setting column char value as P2IN anded with OR output of colMask;

    switch(cols){
    case COL1:  return 0;       //column 1 connected to P2.0 on board
                break;
    case COL2:  return 1;       //column 2 connected to P2.1 on board
                break;
    case COL3:  return 2;       //column 3 connected to P2.2 on board
                break;
    case COL4:  return 3;       //column 4 connected to P2.3 on board
                break;
    default:    return 0xFF;
                break;
    }
}

char readRows(){

    P2REN &= ~colMask;  //disabling pulldown resistor on columns
    P2DIR |= colMask;   //setting output direction of columns
    P2OUT |= colMask;   //setting all cols to 1

    P5DIR &= ~rowMask;  //setting rows as inputs
    P5REN |= rowMask;   //enabling pulldown resistor on rows
    P5OUT &= ~rowMask;

    char rows = P5IN & rowMask;

    switch(rows){               //case statement to handle output based on what rows have been pressed.
    case ROW1:  return 0;
                break;
    case ROW2:  return 1;
                break;
    case ROW3:  return 2;
                break;
    case ROW4:  return 3;
                break;
    default:    return 0xFF;
                break;
    }
}

char keyValue(char row, char col){
    return (row << 2)+col;       //(row*4)+col
}

void checkValue(int attempt[4], int password[4]) {
    for (i = 0; i < 4; i++) {                   //iterate over both user attempt and set password array
        if (attempt[i] != password[i]) {        //if statement returns high if any attempt input and password set dont match
                    //red green blue
            setRGB(0, 20000, 20000);         //when user input is incorrect turn RGB on RED
            buzzerOn(1000);                     //turn buzzer on to blare, longer than correct input pulse
            __delay_cycles(100000);
            setRGB(20000, 20000, 20000);     //turn RGB off
            __delay_cycles(100000);
            setRGB(0, 20000, 20000);         //after delay, turn RGB back on as RED
            __delay_cycles(400000);
            buzzerOn(1000);                     //turn buzzer back on after delay so it blares twice with incorrect password input
            setRGB(20000, 20000, 20000);     //turn RGB back off

            for (i = 0; i < 4; i++){            //set attempt array values back to zero
                attempt[i] = 0;
                index = 0;                      //set count of attempt inputs back to zero
            }
            return;                             //return used to exit the function, only occurs if user attempt does not match program set password on line 92
        }
    }

    //as the loop can only continue through past line 246 without returning, user input matched program set password

    buzzerOn(500);                     //turn buzzer on with a shorter pulse then incorrect input to differentiate the incorrect/correct password input user feedback from board

    __delay_cycles(120000);

    buzzerOn(500);                     //turning on buzzer again

    setRGB(20000, 0, 20000);        //call to RGB LED function to set it on as GREEN, indicating correct passcode input

    ServoOp(180);                      //turn servo 180 degrees, intended to open up door of safe

    latchEngage(1);                    //engage latch/solenoid to unlock the safe's door

    __delay_cycles(5000000);

    setRGB(20000, 20000, 20000);    //call to turn off RGB LED using timer priority/overflow

    __delay_cycles(100000);

    setRGB(20000, 0, 20000);        //call to turn on RGB LED as GREEN

    latchDisengage(1);                 //disengage latch/solenoid to lock the safe's door

    for (i = 0; i < 4; i++){           //for loop to iterate over attempt array and set its values all to zero.
        attempt[i] = 0;
        index = 0;
    }
}

void buzzerOn(int setTime) {
    for (i = 0; i < setTime; i++) {     //for loop to turn on/off buzzer while i is less then function setTime input
        P6OUT |= BIT3;                      //buzzer pin set high
        __delay_cycles(500);
        P6OUT &= ~BIT3;                     //buzzer pin set low
        __delay_cycles(500);
    }
}

void ServoOp(int angle){
    switch(angle){
    case 0: TB3CCR5 = 550;      //setting servo to 0 degrees
    break;
    case 90: TB3CCR5 = 1500;    //setting servo to 90 degrees
    break;
    case 180: TB3CCR5 = 2500;   //setting servo to 180 degrees
    break;
    }
}

void latchEngage(char ON){
    if(ON == 1){
        P4OUT |= BIT4;              //set P4.4 to HIGH so solenoid gate can conduct, pulling in the latch, allowing for safe door to be opened
    }
}

void latchDisengage(char OFF){
    if(OFF == 1){
        P4OUT &= ~BIT4;
    }

}

void setRGB(unsigned int red, unsigned int green, unsigned int blue){
    //assigning duty cycle values to timer registers of rgb colors
    TB3CCR1 = red;
    TB3CCR2 = green;
    TB3CCR3 = blue;
}
