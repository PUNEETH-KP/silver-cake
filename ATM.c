#include<lpc214x.h>
#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include"lcd.h"

/* THE PROCESSOR USED IS NXP LPC2148 FOUNDED BY PHILIPS */ 
/* PIN NUMBERS AS DECLARED BUT CAN BE CHANGED ACCORDINGLY IN CODE AND IN THE CIRCUIT */

void update(int);
void med(void);
void wrong(void);
void med_accept(int);
void keypad2(void);				 //LINE 373
void update2(int);
void keypad1(void);
void keypad(void);
void update1(int);
void notavailable(void);
void bal(void);	 



//BALANCE PARAMETERS
int amount, balance;
char final_amount[10];
char final_balance[10];

// RFID FUNCTIONS
unsigned char rx(void);
void tx(unsigned char c);
void ser_init(void);
unsigned int x,z;
char pr[11] = "$0001591613"	;
 char tag[12];
 int initial_amount=200;

/* 		4X4 KEYPAD   */

#define R1 1<<16
#define R2 1<<17
#define R3 1<<18
#define R4 1<<19

#define C1 1<<20
#define C2 1<<21
#define C3 1<<22
#define C4 1<<23

/*  LCD  */

#define LCD_BACK_LIGHT_TIMEOUT 1000

#define LCD_DATA_DIR	   IO0DIR
#define LCD_DATA_SET	   IO0SET
#define LCD_DATA_CLR	   IO0CLR

#define LCD_CTRL_DIR	   IO0DIR
#define LCD_CTRL_SET       IO0SET
#define LCD_CTRL_CLR       IO0CLR

#define LCDRS	           (1 << 15)
#define LCDRW	           (1 << 16)
#define LCDEN	           (1 << 17)

#define LCD_D4 (1 << 18)
#define LCD_D5 (1 << 19)
#define LCD_D6 (1 << 20)
#define LCD_D7 (1 << 21)

#define LCD_IO0PIN	IO0PIN

int main(void)
{
while(1)
{
init_lcd();
lcd_clear();
lcd_command_write(0x80);
lcd_putstring( 0,0, "0.CROCIN 1.VASOG" );
lcd_command_write(0xC0);
lcd_putstring( 1,0, "2.POWER 3.ZEEBEE" );

delay_1(4000);
lcd_clear();
lcd_putstring(0,1,"5.CHECK BALANCE");  //line 195
delay_1(4000);

//lcd_clear();
keypad1();
} 
 }			
										 

/* TO ENTER THE QUANTITY   */

void med(void)
{
init_lcd();
lcd_clear();
lcd_command_write(0x80);
lcd_putstring(0,0, "enter the quant" );
delay_1(2000);
lcd_command_write(0xC0);
lcd_putstring(1,0, "8.BACK  9.CANCEL");
delay_1(4000);
keypad();
}

/* WHEN QUANTITY EXCEEDS  */

void wrong(void)
{
init_lcd();
lcd_clear();
lcd_command_write(0x80);
lcd_putstring(0,0, "Quantity exceeded" );
delay_1(1000);
lcd_command_write(0xC0);
lcd_putstring(1,0,"Enter less val" );
lcd_clear();
med();
}


/* TO CALCULATE AMOUNT  */

void med_accept(int quantity)
{

amount = quantity * 2;

balance =  initial_amount - amount;

// convert 1 2 3 to string [buf]
sprintf(final_amount, "%d", amount);
sprintf(final_balance,"%d",balance)	;
init_lcd();
lcd_clear();
lcd_command_write(0x80);
lcd_putstring(0,0,"Total amount = " );
delay_1(1000);
lcd_command_write(0xC0);
lcd_putstring(1,0,final_amount);
delay_1(10000);

ser_init();
init_lcd();
lcd_clear();
lcd_command_write(0x80);
lcd_putstring(0,1,"show the card");
delay_1(1000);
lcd_command_write(0xc0);

for(x=0;x<11;x++)
{
tag[x]=rx();
}
tag[11]	= '\0';
//lcd_clear();
//lcd_putstring(0,1,tag);
delay_1(5000);
if(strcmp(pr,tag)==0)
{
lcd_clear();
lcd_putstring(0,1,"	CARD VALID" );
}

if(strcmp(pr,tag)!=0)
{
lcd_clear();
lcd_putstring(0,1,"	CARD NOT VALID" );
delay_1(2000);
lcd_putstring(1,1,"	REFRESHING " );
main();
}


delay_1(5000);
//lcd_clear();
init_lcd();
lcd_clear();
lcd_command_write(0x80);
lcd_putstring(0,0,"1.Continue ");
delay_1(1000);
lcd_command_write(0xC0);
lcd_putstring(1,0, "2.Cancel 3.BACK");
delay_1(5000);
//lcd_clear();
keypad2();					   //LINE 373
}

 //BALANCE CHECKING
void bal(void)
{
ser_init();
init_lcd();
lcd_clear();
lcd_command_write(0x80);
lcd_putstring(0,1,"show the card");
delay_1(1000);
lcd_command_write(0xc0);

for(x=0;x<11;x++)
{
tag[x]=rx();
}
tag[11]	= '\0';
//lcd_clear();
//lcd_putstring(0,1,tag);
delay_1(5000);
if(strcmp(pr,tag)==0)
{
lcd_clear();
lcd_putstring(0,1,"amount balance=" );
delay_1(1000);
init_lcd();						// conversion done in line 135
lcd_putstring(1,0,final_balance); 
delay_1(3000);
main();
}

}

/* WHEN THE MEDICINE IS NOT IN THE LIST  */

void notavailable()
{
init_lcd();
lcd_command_write(0x80);
lcd_putstring(0,0, "NOT AVAILABLE");
delay_1(10000);
lcd_clear();
med();
}

// RFID FUNCTIONS
void ser_init()
{
 //VPBDIV=0x02; //PCLK = 30MHz
 PINSEL0 |= 0x05;
 U0LCR=0x83;
 U0DLL=97;
 U0DLM=0;
 U0LCR=0x03;
 //U0TER=(1<<7);
}

void tx(unsigned char c)
{
 //U0THR=c;
 while(!(U0LSR &(0x20)));
 U0THR = c;
}

unsigned char rx()
{
 while(!(U0LSR&(0x01)));
 return U0RBR;
}


/* TO CHECK FOR QUANTITY  */

void keypad(void)
{
 IO1DIR |= (R1|R2|R3|R4);
IO1DIR &= ~(C1|C2|C3|C4);
IO1SET |= (R1|R2|R3|R4);
while(1)
{
IO1CLR |= R1;
if(!(IO1PIN & C1))
update(0);
if (!(IO1PIN & C2))
update(1);
if (!(IO1PIN & C3))
update(2);
if(!(IO1PIN & C4))
update(3);
IO1SET |= R1   ;


IO1CLR |= R2;
if(!(IO1PIN & C1))
update(4);
if (!(IO1PIN & C2))
update(5);
if (!(IO1PIN & C3))
update(6);
if(!(IO1PIN & C4))
update(7);
IO1SET |= R2  ;
 

 
IO1CLR |= R3;
if(!(IO1PIN & C1))
update(8);
if (!(IO1PIN & C2))
update(9);
if (!(IO1PIN & C3))
update(10);
if(!(IO1PIN & C4))
update(11);
IO1SET |= R3 ;


IO1CLR |= R4;
if(!(IO1PIN & C1))
update(13);
if (!(IO1PIN & C2))
update(14);
if (!(IO1PIN & C3))
update(15);
if(!(IO1PIN & C4))
update(16);
IO1SET |= R4;
 }
}

/* TO CHOOSE OPTION FOR MEDICINE */

void keypad1(void)
{
IO1DIR |= (R1|R2|R3|R4);
IO1DIR &= ~(C1|C2|C3|C4);
IO1SET |= (R1|R2|R3|R4);
while(1)
{
IO1CLR |= R1;
if(!(IO1PIN & C1))
update1(0);
if (!(IO1PIN & C2))
update1(1);
if (!(IO1PIN & C3))
update1(2);
if(!(IO1PIN & C4))
update1(3);
IO1SET |= R1   ;


IO1CLR |= R2;
if(!(IO1PIN & C1))
update1(4);
if (!(IO1PIN & C2))
update1(5);
if (!(IO1PIN & C3))
update1(6);
if(!(IO1PIN & C4))
update1(7);
IO1SET |= R2  ;
 

 
IO1CLR |= R3;
if(!(IO1PIN & C1))
update1(8);
if (!(IO1PIN & C2))
update1(9);
if (!(IO1PIN & C3))
update1(10);
if(!(IO1PIN & C4))
update1(11);
IO1SET |= R3 ;


IO1CLR |= R4;
if(!(IO1PIN & C1))
update1(13);
if (!(IO1PIN & C2))
update1(14);
if (!(IO1PIN & C3))
update1(15);
if(!(IO1PIN & C4))
update1(16);
IO1SET |= R4;
 }
}

/* TO CONTINUE OR CANCEL TRANSACTION  */

void keypad2(void)
{
IO1DIR |= (R1|R2|R3|R4);
IO1DIR &= ~(C1|C2|C3|C4);
IO1SET |= (R1|R2|R3|R4);
while(1)
{
IO1CLR |= R1;
if(!(IO1PIN & C1))
update2(0);
if (!(IO1PIN & C2))
update2(1);
if (!(IO1PIN & C3))
update2(2);
if(!(IO1PIN & C4))
update2(3);
IO1SET |= R1   ;


IO1CLR |= R2;
if(!(IO1PIN & C1))
update2(4);
if(!(IO1PIN & C2))
update2(5);
if(!(IO1PIN & C3))
update2(6);
if(!(IO1PIN & C4))
update2(7);
IO1SET |= R2  ;
 

IO1CLR |= R3;
if(!(IO1PIN & C1))
update2(8);
if(!(IO1PIN & C2))
update2(9);
if(!(IO1PIN & C3))
update2(10);
if(!(IO1PIN & C4))
update2(11);
IO1SET |= R3 ;


IO1CLR |= R4;
if(!(IO1PIN & C1))
update2(13);
if(!(IO1PIN & C2))
update2(14);
if(!(IO1PIN & C3))
update2(15);
if(!(IO1PIN & C4))
update2(16);
IO1SET |= R4;
 }
}


/* TO CHECK QUANTITY IS WITHIN LIMIT OR EXCEEDS THE LIMIT  */

void update(int val)
{
//int val1= val - '\0';
if(val>4 && val != 8 && val != 9)
wrong();
if(val==8)
main();
if(val==9)
main();
if(val<4)
med_accept(val);
}


/*  TO CHECK WHICH MEDICINE IS CHOSE */

void update1(int opt)
{
if(opt==0)
med();
if(opt==1)
med();
if(opt==2)
power();
if(opt==3)
med();
if(opt==5)
bal();
if(opt==4 && opt>5)
notavailable();
}

/* TO CHECK WHETHER TO CONTINUE OR CANCEL TRANSACTION  */
void update2(int opt1)
{
if(opt1==1)
{
init_lcd();
lcd_clear();
lcd_command_write(0x80);
lcd_putstring(0,0,"MEDICINE DISPENSE");
}
if(opt1==2)
main();
if(opt1==3)
med();
if(opt1>3)
main();
}


int lcd_gotoxy( unsigned int x, unsigned int y)
{
  int retval = 0;
  
  if( (x > 1) && (y > 15) )
  {
    retval = -1;
  } else {
  if( x == 0 )
  {
    lcd_command_write( 0x80 + y );		/* command - position cursor at 0x00 (0x80 + 0x00 ) */
  } else if( x==1 ){
    lcd_command_write( 0xC0 + y );		/* command - position cursor at 0x40 (0x80 + 0x00 ) */
    }
   }
   return retval;
}


void delay_1(int count)
{
  int j=0,i=0;

  for(j=0;j<count;j++)
  {
    /* At 60Mhz, the below loop introduces
    delay of 10 us */
    for(i=0;i<1275;i++);
  }
}

void wait_lcd( void )
{
  LCD_DATA_DIR &= ~LCD_BUSY_FLAG;
  LCD_CTRL_CLR |=  LCDRS;	
  LCD_CTRL_SET |=  LCDRW |LCDEN;
  while(LCD_IO0PIN & LCD_BUSY_FLAG);		/* wait for busy flag to become low */
  
  LCD_CTRL_CLR |= LCDEN | LCDRW;
  LCD_DATA_DIR |= LCD_DATA_MASK;
  
  delay_1(100);  
}

void lcd_command_write( unsigned char command )
{
  unsigned char temp=0;
  unsigned int temp1=0;

  temp=command;
  temp=(temp>>4)&0x0F;
  temp1=(temp<<18);

  LCD_CTRL_CLR = LCDRS;
  LCD_CTRL_SET = LCDEN;
  LCD_DATA_CLR = LCD_DATA_MASK;
  LCD_DATA_SET = temp1;
  delay_1(100);
  LCD_CTRL_CLR = LCDEN;

  temp=command;
  temp&=0x0F;
  temp1=(temp<<18);
  delay_1(100);

  LCD_CTRL_CLR |= LCDRS;
  LCD_CTRL_SET |= LCDEN;
  LCD_DATA_CLR = LCD_DATA_MASK;
  LCD_DATA_SET = temp1;
  delay_1(100);	
  LCD_CTRL_CLR |= LCDEN;
  wait_lcd();
}

void set_lcd_port_output( void )
{
  LCD_CTRL_DIR |= ( LCDEN | LCDRS | LCDRW );
  LCD_CTRL_CLR |= ( LCDEN | LCDRS | LCDRW );	
  LCD_DATA_DIR |= LCD_DATA_MASK;
}

void lcd_clear( void)
{
  lcd_command_write( 0x01 );
}



void lcd_data_write( unsigned char data )
{
  unsigned char temp=0;
  unsigned int temp1=0;

  temp=data;
  temp=(temp>>4)&0x0F;
  temp1=(temp<<18);

  LCD_CTRL_SET |= LCDEN|LCDRS;
  LCD_DATA_CLR = LCD_DATA_MASK;
  LCD_DATA_SET = temp1;
  LCD_CTRL_CLR |= LCDEN;

  temp=data;
  temp&=0x0F;
  temp1=(temp<<18);

  LCD_CTRL_SET |= LCDEN|LCDRS;
  LCD_DATA_CLR = LCD_DATA_MASK;
  LCD_DATA_SET = temp1;
  LCD_CTRL_CLR |= LCDEN;
  wait_lcd();
}

void lcd_putstring( unsigned char line,unsigned char pos, char *string )
{
  unsigned char len = MAX_CHAR_IN_ONE_LINE;

  lcd_gotoxy( line,pos);
  while(*string != NULL && len--)
  {
    lcd_putchar( *string );
    string++;
  }
}

void test_cmd(void)
{
//	lcd_command_write(0x38);   // Interface length 8-bit 2 line
	lcd_command_write(0x08);   //Display off,cursor off
	lcd_command_write(0x0C);   //Display on cursor off
	lcd_command_write(0x10);   //Move cursor left by 1 char
	lcd_command_write(0x14);   //Move cursor right by 1 char
	lcd_command_write(0x01);   //Clear display
}


