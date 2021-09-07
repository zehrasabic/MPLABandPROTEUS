
// PIC16F877A Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#define _XTAL_FREQ 16000000
#define LED1 RB1
#define LED2 RB2
#define button RB3

void ServoRotiraj0() {
    unsigned int i;
    for(i=0;i<50;i++) {
        RB0=1;
        __delay_us(800);
        RB0=0;
        __delay_us(19200);
    }
}

void ServoRotiraj90() {
    unsigned int i;
    for(i=0;i<50;i++) {
        RB0=1;
        __delay_us(1500);
        RB0=0;
        __delay_us(18500);
    }
}

void ServoRotiraj180() {
    unsigned int i;
    for(i=0;i<50;i++) {
        RB0=1;
        __delay_us(2200);
        RB0=0;
        __delay_us(17800);
    }
}

void main(){
    TRISB0=0;
    TRISB1=0;
    TRISB2=0;
    TRISB3=1;
    
    while(1) {
        
        if(!button) {
            __delay_ms(5);
            if(!button) {
                ServoRotiraj180();
                LED1=1;
                LED2=0;
                
            }
        }
        else {
            ServoRotiraj0();
            LED2=1;
            LED1=0;
           
        }
        
    }
}

