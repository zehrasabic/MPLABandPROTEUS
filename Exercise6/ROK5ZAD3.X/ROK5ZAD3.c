
// PIC16F877A Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = ON         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define _XTAL_FREQ 20000000
#define Baud_rate 9600

#define LED1 RC0
#define LED2 RC1
#define LED3 RC2
#define LED4 RC3
#define LED5 RC4
#define LED6 RC5
#define LED7 RD0
#define LED8 RD1
#define LED9 RE0
#define LED10 RE1

#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

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
Lcd_Clear() {
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


//Za UART komunikaciju

void UART_Init(void) {
    TRISC6=0;
    TRISC7=1;
    
    SPBRG=((_XTAL_FREQ/16)/Baud_rate)-1;
    BRGH=1;
    
    SYNC=0;
    SPEN=1;
    
    TXEN=1;
    CREN=1;
    
    TX9=0;
    RX9=0;
}

void UART_Send_Char(char a) {
    while(!TXIF);
    TXREG=a;
}

char UART_Get_Char() {
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


void ADC_Initialize()
{
  ADCON0 = 0b01000001; //ADC ON and Fosc/16 is selected
  ADCON1 = 0b11000000; // Internal reference voltage is selected
}

unsigned int ADC_Read(unsigned char channel)
{
  ADCON0 &= 0X1000101; //Clearing the Channel Selection Bits
  ADCON0 |= channel<<3; //Setting the required Bits
  __delay_ms(2); //Acquisition time to charge hold capacitor
  GO_nDONE = 1; //Initializes A/D Conversion
  
  while(GO_nDONE); //Wait for A/D Conversion to complete
  
  return ((ADRESH<<8)+ADRESL); //Returns Result
}



void main(void) {
   
    
    PORTB=0;
    TRISB=0;
    TRISC0=0;
    TRISC1=0;
    TRISC2=0;
    TRISC3=0;
    TRISC4=0;
    TRISC5=0;
    TRISE0=0;
    TRISE1=0;
    TRISD=0;
     Lcd_Init();
    Lcd_Clear();
    UART_Init();
    char DISPLEJ[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D, 0x7D, 0x07,0x7F,0x6F};
    char broj=3;
    float adc;
    float volt, temp;
    int c1, c2, c3, c4, temp1;
    ADC_Initialize();
                LED1=0;
                LED2=0;
                LED3=0;
                LED4=0;
                LED5=0;
                LED6=0;
                LED7=0;
                LED8=0;
                LED9=0;
                LED10=0;
    int opcija;
    do {
        printf("Dobrodosli! Odaberite opciju 1, 2 ili 3\r\r");
        printf("\r1. Upali svaku drugu diodu (od ukupno 10 dioda)\r\r");
        printf("\r2. Pokreni brojac na 7 segmentnom displeju");
        printf("\rkoji broji od 3 do 8 pet puta\r\r");
        printf("\r3. Trenutna temperatura\r\r");
        printf("\rUnesite zeljenu opciju: ");
        opcija=UART_Get_Char();
        UART_Send_Char(opcija);
        printf("\r\r");
        switch(opcija) {
            case '1':
                printf("\r\rIzabrali ste opciju 1.");
               Lcd_Set_Cursor(1,1);
                Lcd_Write_String("Svaka druga");
                Lcd_Set_Cursor(2,1);
                Lcd_Write_String("dioda upaljena!");
                
                
                LED1=0;
                LED2=1;
                LED3=0;
                LED4=1;
                LED5=0;
                LED6=1;
                LED7=0;
                LED8=1;
                LED9=0;
                LED10=1;
                __delay_ms(2000);
                Lcd_Clear();
        
        
                break;
            case '2':
                printf("\r\rIzabrali ste opciju 2.");
                Lcd_Set_Cursor(1,1);
                Lcd_Write_String("Brojanje 5 puta");
                Lcd_Set_Cursor(2,1);
                Lcd_Write_String("od 0 do 8:");
                
                
                for(int i=0;i<31;i++) {
                    PORTB=DISPLEJ[broj];
                    __delay_ms(250);
                    
                    broj++;
                    
                    if(broj>8) broj=3;
                }
                Lcd_Clear();
                break;
            case '3':
                printf("\r\rIzabrali ste opciju 3.");  
                adc = (ADC_Read(4)); 
                volt = adc*4.88281; 
                temp=volt/10.0;  
                temp1 = temp*100; 

                c1 = (temp1/1000)%10;
                c2 = (temp1/100)%10;
                c3 = (temp1/10)%10;
                c4 = (temp1/1)%10;

                Lcd_Clear();
                Lcd_Set_Cursor(1,3);
                Lcd_Write_String("Temperatura");
                Lcd_Set_Cursor(2,5);
                Lcd_Write_Char(c1+'0');
                Lcd_Write_Char(c2+'0');
                Lcd_Write_String(".");
                Lcd_Write_Char(c3+'0');
                Lcd_Write_Char(c4+'0');
                Lcd_Write_Char(0xDF);
                Lcd_Write_String("C");
                __delay_ms(3000);
                Lcd_Clear();
                break;
        }
    }while(opcija!=0);
}
