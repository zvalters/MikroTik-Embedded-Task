/* 
 * File:   main.c
 * Author: Valters Ivars Zem?tis
 *
 * Created on May 8, 2020, 2:36 PM
 */

// PIC16F1507 Configuration Bit Settings
// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection bits (Internal Oscillator, I/O Function on OSC1)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)


#include <xc.h>

void moveLED(void);

unsigned long _XTAL_FREQ = 400000;

enum directions{
    LEFT = -1,
    RIGHT = 1
};
int currentDirection = LEFT;

/*
 *  Executes the task
 */
void main(void) {

    // Disable analog input/output
    ANSELB = 0;
    ANSELC = 0;
    
    // Make all PORTC pins outputs
    TRISC = 0;
    
    // Makes sure PORTC is clear
    PORTC = 0;
    
    while (1) {
        
        __delay_ms(500);
        moveLED();
        
    }
    
}

/*
 *  Moves an LED by one position depending on the global currentDirection variable
 */
void moveLED(void) {
    enum directions direction = currentDirection;
    
    // Resets the LEDs with the correct position depending on the direction
    if (PORTC == 0) {
        switch(direction) {
            case RIGHT:
                PORTC = 0b10000000;
                break;
            case LEFT:
                PORTC = 0b00000001;
                break;
            default:
                break;
        }
        return;
    }

    
    // Moves the LED in the correct direction
    switch(direction) {
        case RIGHT:
            PORTC >>= 1;
            break;
        case LEFT:
            PORTC <<= 1;
            break;
        default:
            break;
    }
        
}

