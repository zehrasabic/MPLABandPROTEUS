
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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define _XTAL_FREQ 20000000
#define Baud_rate 9600

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

//Za UART komunikaciju
void UART_Init(void) {
    TRISC6=0;
    TRISC7=1;
    
    SPBRG=((_XTAL_FREQ/16)/Baud_rate)-1;
    BRGH=1;
    
    SYNC=0;
    SPEN=1;
    
    CREN=1;
    TXEN=1;
    
    RX9=0;
    TX9=0;
}

void UART_Send_Char(char a) {
    while(!TXIF);
    TXREG=a;
}

char UART_Get_Char(){
    if(OERR) {
        CREN=0;
        CREN=1;
    }
    while(!RCIF);
    return RCREG;
}

void putch(char a){
    UART_Send_Char(a);
}

//Za LCD displej
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
        y=temp & 0x0F;
        Lcd_Cmd(z);
        Lcd_Cmd(y);
    }
    else if(a==2) {
        temp=0xC0+b-1;
        z=temp>>4;
        y=temp & 0x0F;
        Lcd_Cmd(z);
        Lcd_Cmd(y);
    }
}

void Lcd_Init() {
    Lcd_Port(0x00);
    __delay_ms(20);
    Lcd_Cmd(0x03);
    __delay_ms(11);
    Lcd_Cmd(0x03);
    __delay_ms(5);
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
    temp=a & 0x0F;
    y=a & 0xF0;
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
    char DISPLEJ[10]= {0x3F,0x06,0x5B,0x4F,0x66,0x6D, 0x7D, 0x07,0x7F,0x6F};
    char DISPLEJ1[10]={0XBF,0X86,0XDB,0XCF,0XE6,0XED,0XFD,0X87,0XFF,0X1F};
    UART_Init();
    TRISC0=0;
    TRISC1=0;
    TRISC2=0;
    TRISC3=0;
    PORTB=0;
    TRISB=0;
    TRISD=0;
    Lcd_Init();
    Lcd_Clear();
    int opcija;
    do {
        printf("            DIGITALNI SISTEMI - ISPIT           ");
        printf("\r                ZEHRA SABIC 890                 ");
        printf("\r1.Sabiranje");
        printf("\r2.Oduzimanje");
        printf("\r3.Mnozenje");
        printf("\r4.Dijeljenje");
        printf("\rUnesite opciju koju zelite realizirati: ");
        opcija=UART_Get_Char();
        UART_Send_Char(opcija);
        printf("\r\r");
        switch(opcija) {
            case '1':
                printf("\rOdabrali ste sabiranje!");
                Lcd_Set_Cursor(1,1);
                Lcd_Write_String("Sabiranje: ");
                
                printf("\r\rUnesite prvi broj: ");
                int a2=UART_Get_Char();
                UART_Send_Char(a2);
                int a1=a2-48;
                s2=1;
                s3=1;
                s4=1;
                PORTB=DISPLEJ[a1];
                s1=0;
                
                printf("\rUnesite drugi broj: ");
                int a4=UART_Get_Char();
                UART_Send_Char(a4);
                int a3=a4-48;
                s1=1;
                s3=1;
                s4=1;
                PORTB=DISPLEJ[a3];
                s2=0;
                __delay_ms(250);
                
                int s=(a1+a3);
                if(s<10) {
                    s1=1;
                    s2=1;
                    s4=1;
                    PORTB=DISPLEJ[s];
                    s3=0;
                    __delay_ms(250);
                }
                else {
                    int broj1,broj2;
                    
                    broj1=s/10;
                    s1=1;
                    s2=1;
                    s4=1;
                    PORTB=DISPLEJ[broj1];
                    s3=0;
                    __delay_ms(250);
                    
                    broj2=s%10;
                    s1=1;
                    s2=1;
                    s3=1;
                    PORTB=DISPLEJ[broj2];
                    s4=0;
                    __delay_ms(250);
                    
                }
                char buffer1[20];
                sprintf(buffer1,"%d",s);
                printf("\rSabiranje -> ");
                printf(buffer1);
                printf("\r\r");
                
                Lcd_Set_Cursor(2,1);
                Lcd_Write_Char(a2);
                Lcd_Set_Cursor(2,3);
                Lcd_Write_Char('+');
                Lcd_Set_Cursor(2,5);
                Lcd_Write_Char(a4);
                Lcd_Set_Cursor(2,7);
                Lcd_Write_Char('=');
                Lcd_Set_Cursor(2,9);
                Lcd_Write_String(buffer1);
                __delay_ms(500);
                Lcd_Clear();
                
                
                break;
                
            case '2':
                printf("\r\rOdabrali ste oduzimanje!");
                Lcd_Set_Cursor(1,1);
                Lcd_Write_String("Oduzimanje:");
                
                printf("\r\rUnesite prvi broj: ");
                int r2=UART_Get_Char();
                UART_Send_Char(r2);
                int r1=r2-48;
                s2=1;
                s3=1;
                s4=1;
                PORTB=DISPLEJ[r1];
                s1=0;
                
                printf("\rUnesite drugi broj: ");
                int r4=UART_Get_Char();
                UART_Send_Char(r4);
                int r3=r4-48;
                s1=1;
                s3=1;
                s4=1;
                PORTB=DISPLEJ[r3];
                s2=0;
                __delay_ms(250);
                int r=(r1-r3);
                
                if(r<0) {
                    char minus[1]={0x40};
                    
                    int broj1;
                    broj1=r/10;
                    s1=1;
                    s2=1;
                    s4=1;
                    PORTB=minus[broj1];
                    s3=0;
                    __delay_ms(250);
                    
                    int broj2=r*(-1);
                    s1=1;
                    s2=1;
                    s3=1;
                    PORTB=DISPLEJ[broj2];
                    s4=0;
                    __delay_ms(250);
                }
                else {
                    s1=1;
                    s2=1;
                    s4=1;
                    PORTB=DISPLEJ[r];
                    s3=0;
                    __delay_ms(250);
                }
                
                char buffer2[20];
                sprintf(buffer2,"%d",r);
                printf("\rRazlika -> ");
                printf(buffer2);
                printf("\r\r");
                
                Lcd_Set_Cursor(2,1);
                Lcd_Write_Char(r2);
                Lcd_Set_Cursor(2,3);
                Lcd_Write_Char('-');
                Lcd_Set_Cursor(2,5);
                Lcd_Write_Char(r4);
                Lcd_Set_Cursor(2,7);
                Lcd_Write_Char('=');
                Lcd_Set_Cursor(2,9);
                Lcd_Write_String(buffer2);
                __delay_ms(500);
                Lcd_Clear();
                
                
                break;
                
            case '3':
                printf("\r\rOdabrali ste mnozenje!");
                Lcd_Set_Cursor(1,1);
                Lcd_Write_String("Mnozenje: ");
                
                printf("\r\rUnesite prvi broj: ");
                int p2=UART_Get_Char();
                UART_Send_Char(p2);
                int p1=p2-48;
                s2=1;
                s3=1;
                s4=1;
                PORTB=DISPLEJ[p1];
                s1=0;
                
                printf("\rUnesite drugi broj: ");
                int p4=UART_Get_Char();
                UART_Send_Char(p4);
                int p3=p4-48;
                s1=1;
                s3=1;
                s4=1;
                PORTB=DISPLEJ[p3];
                s2=0;
                __delay_ms(250);
                
                int p=(p1*p3);
                
                if(p<10) {
                    s1=1;
                    s2=1;
                    s4=1;
                    PORTB=DISPLEJ[p];
                    s3=0;
                    __delay_ms(250);
                }
                else {
                    int broj1, broj2;
                    
                    broj1=p/10;
                    s1=1;
                    s2=1;
                    s4=1;
                    PORTB=DISPLEJ[broj1];
                    s3=0;
                    __delay_ms(250);
                    
                    
                    
                    broj2=p%10;
                    s1=1;
                    s2=1;
                    s3=1;
                    PORTB=DISPLEJ[broj2];
                    s4=0;
                    __delay_ms(250);
                    
                }
                
                char buffer3[20];
                sprintf(buffer3,"%d",p);
                printf("\rProizvod -> ");
                printf(buffer3);
                printf("\r\r");
                
                Lcd_Set_Cursor(2,1);
                Lcd_Write_Char(p2);
                Lcd_Set_Cursor(2,3);
                Lcd_Write_Char('*');
                Lcd_Set_Cursor(2,5);
                Lcd_Write_Char(p4);
                Lcd_Set_Cursor(2,7);
                Lcd_Write_Char('=');
                Lcd_Set_Cursor(2,9);
                Lcd_Write_String(buffer3);
                __delay_ms(500);
                Lcd_Clear();
                break;
                
            case '4':
                printf("\r\rOdabrali ste dijeljenje!");
                Lcd_Set_Cursor(1,1);
                Lcd_Write_String("Dijeljenje: ");
                
                printf("\r\rUnesite prvi broj: ");
                int k2=UART_Get_Char();
                UART_Send_Char(k2);
                int d1=k2-48;
                float k1=k2-48.0;
                s2=1;
                s3=1;
                s4=1;
                PORTB=DISPLEJ[d1];
                s1=0;
                
                printf("\rUnesite drugi broj: ");
                int k4=UART_Get_Char();
                UART_Send_Char(k4);
                int d2=k4-48;
                float k3=k4-48.0;
                s1=1;
                s3=1;
                s4=1;
                PORTB=DISPLEJ[d2];
                s2=0;
                __delay_ms(250);
                
                float k=(k1/k3);
                int m=k;
                int n=(k*10);
                n=n%10;
                
                    s1=1;
                    s2=1;
                    s4=1;
                    PORTB=DISPLEJ1[m];
                    s3=0;
                    __delay_ms(250);
                
                    s1=1;
                    s2=1;
                    s3=1;
                    PORTB=DISPLEJ[n];
                    s4=0;
                    __delay_ms(250);
               
                char buffer4[20];
                sprintf(buffer4,"%d.%d",m,n);
                printf("\rKolicnik -> ");
                printf(buffer4);
                printf("\r\r");
                
                Lcd_Set_Cursor(2,1);
                Lcd_Write_Char(k2);
                Lcd_Set_Cursor(2,3);
                Lcd_Write_Char('/');
                Lcd_Set_Cursor(2,5);
                Lcd_Write_Char(k4);
                Lcd_Set_Cursor(2,7);
                Lcd_Write_Char('=');
                Lcd_Set_Cursor(2,9);
                Lcd_Write_String(buffer4);
                __delay_ms(500);
                Lcd_Clear();
                break;
        }
        
    }
    while(opcija!=5);
}