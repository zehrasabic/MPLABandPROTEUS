
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
#define ZELENA RC4
#define ZUTA RC5
#define CRVENA RC6

#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

#define s1 RC0
#define s2 RC1
#define s3 RC2
#define s4 RC3

#define _XTAL_FREQ 20000000

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

//Za Lcd displej 

void Lcd_Port(char a) {
    if(a&1)
        D4=1;
    else
        D4=0;
    if(a&2)
        D5=1;
    else
        D5=0;
    if(a&4)
        D6=1;
    else
        D6=0;
    if(a&8)
        D7=1;
    else
        D7=0;
}

void Lcd_Cmd(char a) {
    RS=0;
    Lcd_Port(a);
    EN=1;
    __delay_ms(4);
    EN=0;
}
void Lcd_Clear() {
    Lcd_Cmd(0);
    Lcd_Cmd(1);
}

void Lcd_Set_Cursor(char a, char b) {
    char temp, z, y;
    if(a==1) {
        temp=0x80+b-1;
        z=temp>>4;
        y=temp&0x0F;
        Lcd_Cmd(z);
        Lcd_Cmd(y);
    }
    else if(a==2) {
        temp=0xC0+b-1;
        z=temp>>4;
        y=temp&0x0F;
        Lcd_Cmd(z);
        Lcd_Cmd(y);
    }
}

void Lcd_Init() {
    Lcd_Port(0x00);
    __delay_ms(20);
    Lcd_Cmd(0x03);
    __delay_ms(5);
    Lcd_Cmd(0x03);
    __delay_ms(11);
    Lcd_Cmd(0x03);
    Lcd_Cmd(0x02);
    Lcd_Cmd(0x02);
    Lcd_Cmd(0x08);
    Lcd_Cmd(0x00);
    Lcd_Cmd(0x0C);
    Lcd_Cmd(0x00);
    Lcd_Cmd(0x06);
}

void Lcd_Write_Char(char a) {
    char temp, y;
    temp=a&0x0F;
    y=a&0xF0;
    RS=1;
    Lcd_Port(y>>4);
    EN=1;
    __delay_us(40);
    EN=0;
    Lcd_Port(temp);
    EN=1;
    __delay_us(40);
    EN=0;
}

void Lcd_Write_String(char *a) {
    int i;
    for(i=0;a[i]!='\0';i++) {
        Lcd_Write_Char(a[i]);
    }
}

void main(void) {
    PORTB=0;
    TRISB=0;
    PORTC=0;
    TRISC=0;
    PORTD=0;
    TRISD=0;
    Lcd_Init();
    Lcd_Clear();
    char  DISPLEJ[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D, 0x7D, 0x07,0x7F,0x6F};
    for(int i=0;i<6;i++) {
        for(int j=0;j<6;j++) {
            for(int k=0;k<6;k++) {
                for(int z=0;z<10;z++) {
                    for(int a=0;a<10;a++) {
                        
                        if(i==0 && j==0 && k==5 && z==9) {
                            ZELENA=1;
                            ZUTA=0;
                            CRVENA=0;
                            Lcd_Set_Cursor(1,1);
                            Lcd_Write_String("00:59");
                            Lcd_Set_Cursor(2,1);
                            Lcd_Write_String("Vrijeme je pod kontrolom!");
                            break;
                        }
                        else if(i==0 && j==1 && k==5 && z==9) {
                            ZUTA=1;
                            ZELENA=0;
                            CRVENA=0;
                            Lcd_Clear();
                            Lcd_Set_Cursor(1,1);
                            Lcd_Write_String("01:59");
                            Lcd_Set_Cursor(2,1);
                            Lcd_Write_String("Vrijeme je na granici!");
                            break;
                        }
                        else if(i==0 && j==2 && k==1 && z==5) {
                            CRVENA=1;
                            ZELENA=0;
                            ZUTA=0;
                            Lcd_Clear();
                            Lcd_Set_Cursor(1,1);
                            Lcd_Write_String("02:15");
                            Lcd_Set_Cursor(2,1);
                            Lcd_Write_String("Vrijeme je van dozvoljenih granica!");
                            break;
                        }
                        
                        s2=1;
                        s3=1;
                        s4=1;
                        PORTB=DISPLEJ[z];
                        s1=0;
                        __delay_ms(5);
                        
                        s1=1;
                        s3=1;
                        s4=1;
                        PORTB=DISPLEJ[k];
                        s2=0;
                        __delay_ms(5);
                        
                        s1=1;
                        s2=1;
                        s4=1;
                        PORTB=DISPLEJ[j];
                        s3=0;
                        __delay_ms(5);
                        
                        s1=1;
                        s2=1;
                        s3=1;
                        PORTB=DISPLEJ[i];
                        s4=0;
                        __delay_ms(5);
                        
                    }
                }
            }
        }
    }
}