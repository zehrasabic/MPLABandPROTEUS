
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
#include <conio.h>

#define _XTAL_FREQ 20000000
#define Baud_rate 9600

#define LED1 RB1
#define LED2 RB2

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
  ADCON0 &= 0x11000101; //Clearing the Channel Selection Bits
  ADCON0 |= channel<<3; //Setting the required Bits
  __delay_ms(2); //Acquisition time to charge hold capacitor
  GO_nDONE = 1; //Initializes A/D Conversion
  
  while(GO_nDONE); //Wait for A/D Conversion to complete
  
  return ((ADRESH<<8)+ADRESL); //Returns Result
}



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


void main(void) {
    PORTD=0;
    TRISD=0;
    PORTB=0;
    TRISB=0;
    Lcd_Init();
    Lcd_Clear();
    UART_Init();
    float adc;
    float volt, temp;
    int c1, c2, c3, c4, temp1;
    ADC_Initialize();
    
    int opcija;
    do {
        printf("***Dobrodosli! Ovo je drugi zadatak na ispitu. Zehra Sabic, 890 ***");
        printf("\rOPCIJA1: Ispis trenutne temperature");
        printf("\rOPCIJA2: Ispis trenutnog vremena");
        printf("\rOPCIJA3: Ispis poruke na LCD displeju ");
        printf("\rOPCIJA4: Okretanje servomotora u obje strane 3 puta");
        printf("\r******");
        printf("\rMolimo vas odaberite opciju 1, 2, 3 ili 4: ");
        opcija=UART_Get_Char();
        UART_Send_Char(opcija);
        printf("\r******");
        switch(opcija) {
            case '1':
                printf("\r\rIzabrali ste opciju 1.");  
                adc = (ADC_Read(4)); 
                volt = adc*4.88281; 
                temp=volt/10.0;  
                temp1 = temp*100; 

                c1 = (temp1/1000)%10;
                c2 = (temp1/100)%10;
                c3 = (temp1/10)%10;
                c4 = (temp1/1)%10;
                
                printf("\r\rTemperatura: ");
                printf("%d",c1);
                printf("%d",c2);
                printf(".");
                printf("%d",c3);
                printf("%d",c4);
                printf("\r\r");
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
            case '2':
              
               
                break;
            case '3':
                printf("\rIzabrali ste opciju 3.\r\r");
                Lcd_Set_Cursor(1,1);
                Lcd_Write_String("Ovo je opcija 3");
                Lcd_Set_Cursor(2,1);
                Lcd_Write_String("na virtualnom terminalu!");
                __delay_ms(2000);
                Lcd_Clear();
                
                break;
            case '4':
                Lcd_Set_Cursor(1,1);
                Lcd_Write_String("Rotiranje servomotora u jednu,");
                Lcd_Set_Cursor(2,1);
                Lcd_Write_String("a zatim u drugu stranu 3 puta!");
                for(int i=0;i<3;i++) {
                ServoRotiraj0();
                LED1=1;
                LED2=0;
                __delay_ms(150);
                ServoRotiraj180();
                LED2=1;
                LED1=0;
                __delay_ms(150);
                }
                Lcd_Clear();
                break;   
        }
    }
    while(opcija!=0);
}