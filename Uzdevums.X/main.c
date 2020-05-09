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
#include <stdbool.h>

#define SONG_LENGTH 35

void moveLED(void);
void playSong(void);
void delay_ms(unsigned char milliseconds);

unsigned long _XTAL_FREQ = 4000000;
enum directions{
    LEFT = -1,
    RIGHT = 1
};
int currentDirection = RIGHT;
bool buttonPressed = false;

// PR2 values for different notes with TMR2 pre-scaler of 16
// Unused notes are commented out
typedef enum  {
    //B3  = 254,
    //C4  = 240,
    //Db4 = 226,
    //D4  = 214,
    //Eb4 = 202,
    E4  = 191,
    //F4  = 180,
    Gb4 = 170,
    //G4  = 160,
    //Ab4 = 152,
    //A4  = 143,
    Bb4 = 135,
    //B4  = 128,
    //C5  = 120,
    Db5 = 114,
    //D5  = 107,
    Eb5 = 101,
    E5  = 96,
    //F5  = 90,
    Gb5 = 85,
    //G5  = 81,
    //Ab5 = 76,
    //A5  = 72,
} NOTES;

// Holds properties of a single note
struct note{
    NOTES key;
    unsigned char duration;
};

// Song based on this sheet music: 
// https://upload.wikimedia.org/wikipedia/commons/5/50/Flohwalzer.pdf
struct note song[SONG_LENGTH] = {
    {Eb5, 50}, {E5, 50},
    {Gb4, 100}, {Gb5, 100}, {Gb5, 100}, {Eb5, 50}, {E5, 50},
    {Gb4, 100}, {Gb5, 100}, {Gb5, 100}, {Eb5, 50}, {E5, 50},
    {Gb4, 100}, {Gb5, 100}, {E4, 100}, {Gb5, 100},
    {Bb4, 100}, {E5, 100}, {E5, 100}, {Eb5, 50}, {Db5, 50},
    {Bb4, 100}, {E5, 100}, {E5, 100}, {Eb5, 50}, {Db5, 50},
    {Bb4, 100}, {E5, 100}, {E5, 100}, {Eb5, 50}, {Db5, 50},
    {Db5, 100}, {E5, 100}, {E4, 100}, {E5, 100}
};

/*
 *  Executes the task
 */
void main(void) {
    
    // Sets the oscillator speed at 4MHz for notes calculation
    OSCCON = 0b01101011;

    // Disable analog input/output for PORTB and PORTC
    ANSELB = 0;
    ANSELC = 0;
    
    // Make all PORTC pins outputs
    TRISC = 0;
    
    // Makes sure PORTC is clear
    PORTC = 0;
    
    // Configures the button
    TRISBbits.TRISB6 = 1;   // Make PORTB pin6 an input for the button
    INTCONbits.GIE = 1;     // Enables interrupts
    INTCONbits.IOCIE = 1;   // Enables interrupts on change
    IOCBPbits.IOCBP6 = 1;   // Detects the rising edge on PORTB pin6
    
    
    // Prepares a timer for debouncing the button
    //Timer0 Registers Prescaler= 64 - TMR0 Preset = 99 - Freq = 99.52 Hz - Period = 0.010048 seconds
    OPTION_REGbits.T0CS = 0;  // TMR0 Clock Source Select bit...0 = Internal Clock (CLKO) 1 = Transition on T0CKI pin
    OPTION_REGbits.T0SE = 0;  // TMR0 Source Edge Select bit 0 = low/high 1 = high/low
    OPTION_REGbits.PSA = 0;   // Prescaler Assignment bit...0 = Prescaler is assigned to the Timer0
    OPTION_REGbits.PS2 = 1;   // PS2:PS0: Prescaler Rate Select bits
    OPTION_REGbits.PS1 = 0;
    OPTION_REGbits.PS0 = 1;

    
    // Prepares a timer for moving the LEDs
    //Timer1 Registers Prescaler= 8 - TMR1 Preset = 34211 - Freq = 3.99 Hz - Period = 0.250600 seconds
    T1CONbits.T1CKPS1 = 1;   // Prescaler Rate Select bits
    T1CONbits.T1CKPS0 = 1;   
    T1CONbits.T1OSCEN = 1;   // Timer1 Oscillator Enable Control bit 1 = on
    T1CONbits.TMR1CS = 0;    // Timer1 Clock Source Select bit...0 = Internal clock (FOSC/4)
    T1CONbits.TMR1ON = 1;    // enables timer
    TMR1H = 133;             // preset for timer1 MSB register
    TMR1L = 163;             // preset for timer1 LSB register

    INTCONbits.PEIE = 1;    // Enables interrupts for TMR1
    PIE1bits.TMR1IE = 1;

    
    // Prepares the buzzer
    PWM3CON = 0;                // Clear previous values
    PWM3DCH = 0;
    PWM3DCL = 0;
    
    PIR1bits.TMR2IF = 0;        // Clears TMR2IF
    T2CONbits.T2CKPS0 = 0;      // Sets a pre-scaler of 16
    T2CONbits.T2CKPS1 = 1;
    T2CONbits.TMR2ON = 1;       // Enables Timer 2
    PWM3CONbits.PWM3EN = 1;	    // Enables the PWM3 module. Note: output is still off
    TRISAbits.TRISA2 = 0;       // Enables output on RA2
    
    // Plays the song on a loop
    while (1) {
        
        playSong();
        
    }
    
}

void __interrupt () isr_routine(void) {
    
    // Handles moving LEDs
    if (PIR1bits.TMR1IF) {
        moveLED();
        TMR1H = 133;             // preset for timer1 MSB register
        TMR1L = 163;             // preset for timer1 LSB register
        PIR1bits.TMR1IF = 0;
    }
    
    // Handles checking for debounce on the button
    if (INTCONbits.T0IF) {
        if (buttonPressed) {
            if (PORTBbits.RB6)      // If it is still active, then change direction
                currentDirection = -currentDirection;
            buttonPressed = false;
            INTCONbits.T0IE = 0;    // Disable Timer0 interrupt
        }
        INTCONbits.T0IF = 0;
    }
    
    // Handles checking if the button has been touched
    if (IOCBFbits.IOCBF6)  {
        if (!buttonPressed) {
            buttonPressed = true;
            // Start Timer
            TMR0 = 99;              // preset for timer register
            INTCONbits.T0IE = 1;    // Enable timer
        }
        IOCBFbits.IOCBF6 = 0;
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

/*
 *  Plays the song using the currentDirection
 */
void playSong(void) {
    
    enum directions direction = currentDirection;
    int i;
    
    switch(direction) {
        case RIGHT:
            i = 0;
            break;
        case LEFT:
            i = SONG_LENGTH - 1;
            break;
        default:
            break;
    }
    
    for (; i >= 0 && i < SONG_LENGTH; i += currentDirection) {
    
        // Load the note
        // (PR2 * 2) gets spread between two registers which provides a 50% duty cycle
        PR2 = song[i].key;
        PWM3DCLbits.PWM3DCL0 = 0;
        PWM3DCLbits.PWM3DCL1 = PR2 & 1;
        PWM3DCH = PR2 >> 1;
        
        // Play the note with the appropriate duration
        delay_ms(song[i].duration);
        PWM3CONbits.PWM3OE = 1;
        delay_ms(song[i].duration);
        PWM3CONbits.PWM3OE = 0;
        
    }
}

/*
 * A delay function that provides a variable delay
 */
void delay_ms(unsigned char milliseconds)
{
   while(milliseconds > 0)
   {
      milliseconds--;
       __delay_us(990);
   }
}
