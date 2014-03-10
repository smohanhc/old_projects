#include <AT89X52.H>
#include <stdio.h>
#include <math.h>

sbit	adc_add0	=	P2^0;
sbit	adc_add1	=	P2^1;
sbit	adc_add2	=	P2^2;

sbit	adc_ale		=	P2^3;
sbit	adc_start	=	P2^4;
sbit	adc_eoc		=	P2^5;
sbit	adc_oe		=	P2^6;

sbit	speed1		=	P1^0;
sbit	speed2		=	P1^1;
sbit	speed3		=	P1^2;
sbit	speed4		=	P1^3;
sbit	speed5		=	P1^4;

sbit	buzer		=	P1^5;
sbit	lighting	=	P1^6;
sbit	light		=	P1^7;

sbit	mode		=	P3^4;
sbit	power		=	P3^5;
sbit	speed		=	P3^6;

#define	on		1
#define	off		0
#define	low		0
#define	normal	1
#define	_auto	1
#define	manual	0
#define	pressed	0
#define	databus	P0
#define	serial	SBUF
#define	limit	0xfff0
#define	marked	0xff
#define	reset	0x00
#define	start	1
#define	stop	0
#define	_timer	TR0
//-----------------------------------------------------------

unsigned char idata adc_data=0,ctrl_data=0,rxd=0,txd=0;
unsigned char idata flag1=0,flag2=0,time_ctrl=0;
unsigned int idata visitors=1,visitors_old=1;
float idata temperature=0;
//-----------------------------------------------------------

delay(unsigned int);
extern void lcdprintf(const char*);
extern void lcdprintd(const char,unsigned char);
extern void preparelcd();
//-----------------------------------------------------------

void beep()
	{
		buzer = on; delay(100); buzer = off;
	}
//-----------------------------------------------------------

unsigned char adc()
	{
		unsigned char _data=0;
		_data=databus = 0xff;
		adc_ale=1; adc_start=1; adc_ale=0; adc_start=0;
		for(adc_eoc=1;adc_eoc!=0;) {}
		for(adc_eoc=1;adc_eoc!=1;) {}
		adc_oe=1; _data=databus;adc_oe=0; return _data;
	}
//-----------------------------------------------------------

void direct_ctrl(unsigned char value)
	{
		float temp=0;
		
		temp = value/5;
					
		if(temp<28)
			{
				P1 = P1&0xe0|0x80;
				
			}
		
		if(temp>=28&&temp<29)
			{
				P1 = ((P1&0xe0)|0x81);
			}
	
		if(temp>=29&&temp<30)
			{
				P1 = ((P1&0xe0)|0x82);
			}

		if(temp>=30&&temp<31)
			{
				P1 = ((P1&0xe0)|0x84);
			}

		if(temp>=31&&temp<32)
			{
				P1 = ((P1&0xe0)|0x88);
			}

		if(temp>=32)
			{
				P1 = ((P1&0xe0)|0x90);
			}
	}
//-----------------------------------------------------------

void timer()
	{
		time_ctrl = 35;
		TH0 = 0; TL0 = 0; _timer = start;
	}
//-----------------------------------------------------------

void timer_end_int(void) interrupt 1
	{
		time_ctrl--;
		if(time_ctrl==0)
			{
				flag1=flag2 = reset;
				_timer = stop;
			}
	}			
//-----------------------------------------------------------
/* ERROR : Now suppose the visitor count is zero and if we again tries to reduce the visitior count
		that is when sensor 1 is activated, it sets f1. After that sensor 2 is activated.
		There f2 is set and checks the condition- 'visitors!=0'. Since control doesn't 
		enter the if block, the flags are kept set for some time ( time for which the timer runs).
		So if within that time, sensor1 is activated, it is eqivqlent to an increase in
		the visitor count.*/ 
		 
void sens1_int(void) interrupt 0 
	{		
		flag1 = marked;
		if(flag1==flag2)
			{
				visitors++;
				flag1=flag2 = reset;
				_timer = stop;
		/*		lcdprintf("VISITORS:         ");
				sprintf(buf1,"%d\n",(const)visitors);

				for(n=0;n<=2&&buf1[n]!='\n';n++)
					{
						lcdprintd(buf1[n],add);
						add++;
					}			*/
			}

		else	timer();
	}
//-----------------------------------------------------------

void sens2_int(void) interrupt 2 
	{			
		flag2 = marked;
		if(flag1==flag2&&visitors!=0)
			{
				visitors--;
				flag1=flag2 = reset;
				_timer = stop;
		/*		lcdprintf("VISITORS:         ");
				sprintf(buf1,"%d\n",(const)visitors);

				for(n=0;n<=2&&buf1[n]!='\n';n++)
					{
						lcdprintd(buf1[n],add);
						add++;
					}			*/
			}

		else	timer();
	}
//-----------------------------------------------------------

void display(float value)
	{
		unsigned char add=0xcc,idata buf[5]=0,n=0;
		
		sprintf(buf,"%f\n",value);

		for(n=0;add<=0xcf;n++)
			{
				lcdprintd(buf[n],add);
				add++;
			}

	}
//-----------------------------------------------------------

show()
	{
		unsigned char buf[4]=0,add=0,n=0;

		sprintf(buf,"%d\n",(const)visitors);

/* ERROR :	once the visitor count reaches 10, then when we decrement only the 1st 	digit is
		cleared. ie the display is showing 90 instead of 9. the second digit doesn't get
		cleared. Like wise, ifthe visitoor count reaches 100 and when decremented, it
		shows 990. The problem persists for manual mode also. */

		for(add=0x8d;n<=2&&buf[n]!='\n';n++)
			{
				lcdprintd(buf[n],add);
				add++;
			}
	}
//-----------------------------------------------------------



void main(void)
	{
		unsigned int count=0;

		P0 = 0xff;		/* data bus */
		P1 = 0x80;		/* relay,buzer,LDR */
		P2 = 0x20;		/* ADC address&ctrl,LCD */
		P3 = 0xff;		/* serial,switch,LCD */

		beep();

		SCON  = 0x50;		        /* SCON: mode 1, 8-bit UART, enable rcvr      */
    	TMOD |= 0x21;               /* TMOD: timer 1, mode 2, 8-bit reload        */
    	TH1   = -3;                 /* TH1:  reload value for 1200 baud @ 11.0592MHz   */
    	TR1   = 1;                  /* TR1:  timer 1 run                          */
		IT0=IT1 = 1;
		IE	  = 0x87;		

		preparelcd();
		lcdprintf("iROOM CONTROLLER\n");
		delay(1000);
		lcdprintf("\nVer. 0.1,   2005");
		delay(2000);
		delay(1);

		P0=0x8; P2_7=0; P3_7=0; P3_7=1;delay(500);
		P0=0xc; P2_7=0; P3_7=0; P3_7=1;delay(500);
		P0=0x8; P2_7=0; P3_7=0; P3_7=1;delay(500);
		P0=0xc; P2_7=0; P3_7=0; P3_7=1;delay(500);

		lcdprintf("PROJECT BY:     \n");delay(1000);
		lcdprintf("\nNIKHIL MAHESH CK");delay(1000);
		lcdprintf("\nRAHUL SURENDRAN ");delay(1000);
		lcdprintf("\nSUMOD MOHAN K   ");delay(2000);

		lcdprintf("VISITORS:    1  \n");
		lcdprintf("\nTEMPERATURE:    ");

		for(;;)
			{
				for(;power==on&&mode==_auto&&visitors!=0;)
					{	
					
	/* ERROR : here since the condition - 'visitors!=0' is given, when the visitor 
		count becomes zero, it is not displayed.*/

						show();
						adc_data=txd = adc();
						temperature = (float)adc_data/5;
						display(temperature);
						
						TI = 0; serial = txd;
						for(TI=0;TI!=1;) {}
						TI = 0;

						count = 0;
						for(RI=0;RI!=1&&count<limit;count++) {}
						RI = 0; rxd = serial;
						if(count<=limit&&rxd==txd)
							{
								count = 0;
								for(RI=0;RI!=1&&count<limit;count++) {}
								RI = 0; rxd = serial;
								if(count<=limit)
									{
										P1 = ((P1&0xe0)|rxd+0x80);
									}
								else	direct_ctrl(adc_data);								
							}

						else	direct_ctrl(adc_data);

						if(light==low)
							{
								lighting = on;
							}
						else	lighting = off;

				//		delay(100);
					}
					

				for(count=0,P1=(P1&0xc0);count<=5&&power==on&&mode==manual&&visitors!=0;)
					{	
						beep();
						lcdprintf("\n   MANUAL MODE  ");
						for(;mode==manual&&count<=5;)
							{
								if(visitors!=visitors_old)
									{
								    	visitors_old=visitors;
										show();
									}

								if(speed==pressed)
									{
										beep();
										ctrl_data = pow(2,count);
										if(P1^5==0)
											{
												P1 = ((P1&0xe0)|ctrl_data);
											}
										else	P1 = P1&0xe0;
												for(;speed==pressed;){}
												count++;
									}

								if(light==low)
									{
										beep();
										lighting=~lighting;	
										for(;light==low;){}
									}						
							}
					}
						P1 = 0x80;
						lcdprintf("\nTEMPERATURE:    ");

				for(;power==off;)
					{						
						P1 = 0x80;
						lcdprintf("\n    STANDBY      ");
						for(;power==off&&visitors==visitors_old;){}

						if(visitors!=visitors_old)
							{
								visitors_old=visitors;
								show();
							}
					}
				lcdprintf("\nTEMPERATURE:    ");

	/* ERROR : Since the above statement is unconditional, it is executed for every cycle.
		   So, when the system works in manual mode, when the speed is increased one by one
		   from 1,2,3,4,5 and then when speed becomes zero, the above statement gets displayed 
		   on the lcd for a fraction of a second. */
			}				
		

	}



