/*
 * COPYLEFT 2005 SUMOD MOHAN K ( smkmenon@ieee.org )
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 * */



#include <linux/module.h>
#include <linux/kernel.h>

//Remove Symmetric Multiple Processor Capabilities
#undef CONFIG_SMP

#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_fifos.h>


//License Docs
MODULE_LICENSE("GPL");
EXPORT_NO_SYMBOLS;

//PORT Definitions
#define BASE 0x3F8
#define DATA_FMT_REGISTER BASE+3


//RTAI Definitions
#define STACK_SIZE 4096
#define TIMERTICKS 1000000

//FIFO Definitions
#define I_FIFO 0          // Write to UI
#define O_FIFO 1          // Read from UI
#define CNT_FIFO 2	  // Computer Control 
#define FIFO_SIZE 1024
#define LOOP_TIME 867

static RT_TASK main_module,read_serial,calc;
static RTIME tick_period;
char odata=2,cmp=0;

static void init_serial()
{
	// Serial Port Communication Initialisation
	// 9600 baud, 8N1 format
	unsigned int val;
	unsigned char divisor_low,divisor_high;

	val = inb(DATA_FMT_REGISTER);
	val = val|0x80;
	outb(val, DATA_FMT_REGISTER);
	divisor_low = 12;
	divisor_high = 0;

	outb(divisor_low, BASE);
	outb(divisor_high, BASE+1);
	outb(3, DATA_FMT_REGISTER);
}


	
static void m_module()
{	
	unsigned int data=0xaa;
	
	//Do all functions
	//
	while(1)
	{
		while((inb(BASE+0x5)&0x1)!=1)
			rt_sleep(nano2count(LOOP_TIME*TIMERTICKS));	
		data=inb(BASE);
	//	data=~data;
		//printk("<2>""%u: %c\n",data,data);
		if(cmp&1)
		{
		  rt_sleep(nano2count(10*TIMERTICKS));
		  outb(data,BASE);
		  rt_sleep(nano2count(10*TIMERTICKS));
		  outb(odata,BASE);
		}
		if(rtf_put(I_FIFO,&data,1)<0)
			printk("<2>""write error !\n");
		rtf_put(I_FIFO,&data,1);
	}
	
		
}

int handler_output1(unsigned int fifo)
{
	rtf_get(O_FIFO,&odata,1);
	return 0;
}

int handler_output2(unsigned int fifo)
{
	rtf_get(CNT_FIFO,&cmp,1);
	return 0;
}

int init_module(void)
{       RTIME now;
	
	//Serial port deployment
	//iopl(3);
	init_serial();
	
	//Real Time FIFO's
	rtf_create(I_FIFO,FIFO_SIZE);
	rtf_create(O_FIFO,FIFO_SIZE);
	rtf_create(CNT_FIFO,FIFO_SIZE);
	rtf_create_handler(O_FIFO,handler_output1);
	rtf_create_handler(CNT_FIFO,handler_output2); 
	//Real Time Task deployment
	rt_set_periodic_mode();
	rt_task_init(&main_module, m_module, 0, STACK_SIZE, 0, 0, 0);
	tick_period = start_rt_timer(nano2count(TIMERTICKS));
	now = rt_get_time();
	rt_task_make_periodic(&main_module, now+tick_period, tick_period);
	return 0;
}

void cleanup_module(void)
{
	rtf_destroy(I_FIFO);
	rtf_destroy(O_FIFO);
	rtf_destroy(CNT_FIFO);
	stop_rt_timer();
	rt_busy_sleep(10000000);
	rt_task_delete(&main_module);
}	
