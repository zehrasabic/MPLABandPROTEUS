
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
#define button RC0
void main() {
    char DISPLEJ[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D, 0x7D, 0x07,0x7F,0x6F};
    char broj=0;
    PORTB=0;
    TRISB=0;
    TRISC0=1;
   
    while(1) {
        if(button) {
            __delay_ms(5);
            if(button) {
                PORTB=DISPLEJ[broj];
                __delay_ms(250);
                broj++;
                if(broj>9){
                    broj=0;
                }
            }
        }
        else{
            PORTB=DISPLEJ[broj];
        }
    }
    
}
