
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

 



// DHT11 sensor connection (here data pin is connected to RB0)

#define DHT11_PORT RB0
#define DHT11_TRIS TRISB0

#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

#include <xc.h>
#define _XTAL_FREQ 8000000

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


// variables declaration
char Temperatura[] = "Temp = 00.0 C ";
char Vlaznost[]    = "Vlaz = 00.0 % ";
unsigned char T1, T2, V1, V2, KontrolnaSuma ;
 
// send start signal to the sensor
void Start_Signal(void) {
  DHT11_TRIS = 0;     // configure DHT11_PIN as output
  DHT11_PORT = 0;         // clear DHT11_PIN output (logic 0)
 
  __delay_ms(25);        // wait 25 ms
  DHT11_PORT = 1;         // set DHT11_PIN output (logic 1)
 
  __delay_us(30);        // wait 30 us
  DHT11_TRIS = 1;     // configure DHT11_PIN as input
}
 
// Check sensor response
__bit Check_Response() {
  TMR1H = 0;                 // reset Timer1
  TMR1L = 0;
  TMR1ON = 1;                // enable Timer1 module
  while(!DHT11_PORT && TMR1L < 100);  // wait until DHT11_PIN becomes high (checking of 80µs low time response)
 
  if(TMR1L > 99)                     // if response time > 99µS  ==> Response error
    return 0;                        // return 0 (Device has a problem with response)
 
  else
  {
    TMR1H = 0;               // reset Timer1
    TMR1L = 0;
 
    while(DHT11_PORT && TMR1L < 100); // wait until DHT11_PIN becomes low (checking of 80µs high time response)
 
    if(TMR1L > 99)                   // if response time > 99µS  ==> Response error
      return 0;                      // return 0 (Device has a problem with response)
 
    else
      return 1;                      // return 1 (response OK)
  }
}

// Data read function
__bit Read_Data(unsigned char* dht_data)
{
  *dht_data = 0;
 
  for(char i = 0; i < 8; i++)
  {
    TMR1H = 0;             // reset Timer1
    TMR1L = 0;
 
    while(!DHT11_PORT)      // wait until DHT11_PIN becomes high
      if(TMR1L > 100) {    // if low time > 100  ==>  Time out error (Normally it takes 50µs)
        return 1;
      }
 
    TMR1H = 0;             // reset Timer1
    TMR1L = 0;
 
    while(DHT11_PORT)       // wait until DHT11_PIN becomes low
      if(TMR1L > 100) {    // if high time > 100  ==>  Time out error (Normally it takes 26-28µs for 0 and 70µs for 1)
        return 1;          // return 1 (timeout error)
      }
 
     if(TMR1L > 50)                  // if high time > 50  ==>  Sensor sent 1
       *dht_data |= (1 << (7 - i));  // set bit (7 - i)
  }
 
  return 0;                          // return 0 (data read OK)
}

void main(void) {
    PORTD=0;
    TRISD=0;
    PORTB=0;
    TRISB=0;
    T1CON  = 0x10;        // set Timer1 clock source to internal with 1:2 prescaler (Timer1 clock = 1MHz)
    TMR1H  = 0;           // reset Timer1
    TMR1L  = 0;
    Lcd_Init();
    Lcd_Clear();
    while(1)
  {
    Start_Signal();     // send start signal to the sensor
 
    if(Check_Response())    // check if there is a response from sensor (If OK start reading humidity and temperature data)
    {
      // read (and save) data from the DHT11 sensor and check time out errors
      if(Read_Data(&V1) || Read_Data(&V2) || Read_Data(&T1) || Read_Data(&T2) || Read_Data(&KontrolnaSuma))
      {
        Lcd_Clear();      // clear LCD
        Lcd_Set_Cursor(1, 5);           // go to column 5, row 2
        Lcd_Write_String("Time out!");   // display "Time out!"
      }
 
      else         // if there is no time out error
      {
        if(KontrolnaSuma == ((V1 + V2 + T1 + T2) & 0xFF))
        {                                       // if there is no checksum error
          Temperatura[7]  = T1 / 10  + '0';
          Temperatura[8]  = T1 % 10  + '0';
          Temperatura[10] = T2 / 10  + '0';
          Vlaznost[7]     = V1 / 10 + '0';
          Vlaznost[8]     = V1 % 10 + '0';
          Vlaznost[10]    = V2 / 10 + '0';
          Temperatura[11] = 223;    // put degree symbol (°)
          Lcd_Set_Cursor(1, 1);          
          Lcd_Write_String(Temperatura);
          Lcd_Set_Cursor(2, 1);           
          Lcd_Write_String(Vlaznost);
        }
 
        // if there is a checksum error
        else
        {
          Lcd_Clear();     
          Lcd_Set_Cursor(1, 1);           
          Lcd_Write_String("Kontrolna suma:");
          Lcd_Set_Cursor(2,1);
          Lcd_Write_String("ERROR!!!");
        }
 
      }
    }
 
    // if there is a response (from the sensor) problem
    else
    {
      Lcd_Clear();      
      Lcd_Set_Cursor(1, 1);           
      Lcd_Write_String("Nema odziva");
      Lcd_Set_Cursor(2, 1);           
      Lcd_Write_String("sa senzora!");
    }
 
    TMR1ON = 0;        // disable Timer1 module
    __delay_ms(1000);  // wait 1 second
 
  }
}

