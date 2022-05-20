/*******************************************************
Project : Calculator in Interupt mode
Version : 1.0.0
Date    : 05/ 19/ 2022
Author  : Mohammad Reza Haseli 
 

Chip type               : ATmega32
Program type            : Application
AVR Core Clock frequency: 8.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*******************************************************/

// Declare Function
#include <mega32.h>
#include <delay.h>
#include <alcd.h>
#include <stdio.h>

// Declare PortB
#define c1 PINB.4
#define c2 PINB.5
#define c3 PINB.6
#define c4 PINB.7
#define keypad_port PORTB

// Declare your global variables here
char Key = 'C' , State=0, Operand;
char Buf1[5], Buf0[5],Buf[16];
int Num0=0, Num1=0, Result;

unsigned char scan[5]={0XFE,0XFD,0XFB,0XF7};

char arrkey[16]={
'7','8','9','/',
'4','5','6','*',
'1','2','3','-',
'C','0','=','+'};

// Declare calculator function
void Calculator();

// Declare keypad function
char keypad();

// Declare interrupt 0 for keypad that will work in rising edge mdoe
interrupt [EXT_INT0] void pad(void)
{ 
    Key = keypad();    
}


void main(void)
{
    //active intrupt 0 in rising mode
    GICR = (1 << INT0);
    GIFR = (1 << INTF0);
    MCUCR = (1 << ISC00) | (1 << ISC01); 
    #asm ("sei")
        
    DDRB = 0x0F;
    keypad_port=0XF0;
    
    DDRD.0 = 1;
    
    lcd_init(16);
    lcd_puts("start...");
    delay_ms(1000);
    lcd_clear();
    
    while(1)
    {
      Calculator();  
   }
}


char keypad(){
    unsigned char r,c,k;    
    keypad_port=0XFF;
    while(1)
    {
        for (r=0; r<4; r++)
        {
            c=255;
            keypad_port=scan[r];
            if(c1==0) c=0; if(c2==0) c=1; if(c3==0) c=2; if(c4==0) c=3;
            if (c!=255)
            {
                k=arrkey[(r*4)+c];
                keypad_port=0XF0;
                return k;
            }
        }
    }
} 
   

void Calculator()
{
    switch (State)
        {
            case 0:
                if (Key == 'C') 
                {
                    Num0=0; Num1=0; Operand=''; lcd_clear(); lcd_puts("0");Key = ' ';
                }
                else if ((Key == '+') | (Key == '-') | (Key == '/') | (Key == '*')) 
                {
                    Operand = Key; lcd_clear(); lcd_puts(Buf1);
                    lcd_putchar(' '); lcd_putchar(Key); lcd_putchar(' '); State =1;Key = ' ';
                }
                else if((Key == '1') | (Key == '2') | (Key == '3') | (Key == '4')
                | (Key == '5') | (Key == '6') | (Key == '7') | (Key == '8')
                | (Key == '9') | (Key == '0'))
                {
                    Num1 = Num1 * 10 + Key - 48; lcd_clear();
                    sprintf(Buf1,"%d",Num1); lcd_puts(Buf1);Key = ' ';
                }
                break;
            case 1:
                if (Key == 'C') 
                {
                    Num0=0; Num1=0; Operand=''; lcd_clear(); lcd_putchar('0'); State = 0;Key = ' ';
                }
                else if ((Key == '1') | (Key == '2') | (Key == '3') | (Key == '4')
                | (Key == '5') | (Key == '6') | (Key == '7') | (Key == '8')
                | (Key == '9') | (Key == '0')) 
                {
                    Num0 = Num0 * 10 + Key - 48; lcd_gotoxy(0,0); lcd_puts(Buf1);
                    lcd_putchar(' '); lcd_putchar(Operand); lcd_putchar(' ');
                    sprintf(Buf0,"%d",Num0); lcd_puts(Buf0);Key = ' ';
                }
                else if (Key == '=')
                {
                    lcd_putchar(' '); lcd_putchar(Key); lcd_gotoxy(0,1);
                    if (Operand == '+')
                    {
                        Result = Num1 + Num0; sprintf(Buf,"%d",Result); lcd_puts(Buf);
                    }
                    else if (Operand == '-') 
                    {
                        Result = Num1 - Num0; sprintf(Buf,"%d",Result); lcd_puts(Buf);
                    }
                    else if (Operand == '/') 
                    {
                        if (Num0 == 0) {Result = 0; lcd_puts("Nan");}
                        else 
                        {
                                Result = Num1 / Num0; sprintf(Buf,"%d",Result); lcd_puts(Buf);
                        }
                    }
                    else if (Operand == '*')
                    {
                        Result = Num1 * Num0 ; sprintf(Buf,"%d",Result); lcd_puts(Buf);
                    }
                    State = 0; Num0 = 0; Num1 = Result; sprintf(Buf1,"%d",Num1);
                }
            break;        
        }
}

