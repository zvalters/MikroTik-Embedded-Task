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

unsigned long _XTAL_FREQ = 400000;


/*
 * 
 */
void main(void) {

    // Disable analog input/output
    ANSELB = 0;
    ANSELC = 0;
    
    // Make all PORTC pins outputs
    TRISC = 0;
    
    PORTC = 0;
    while (1) {
        
        __delay_ms(500);
        if (PORTC == 0)
            PORTC = 1;
        else
            PORTC <<= 1;
        
    }
    
}

