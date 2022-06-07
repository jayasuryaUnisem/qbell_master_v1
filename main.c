/*+---------------------------------------------------------------------+*/
/*?				Bestcomm document description					   		?*/
/*+---------------------------------------------------------------------?*/
/*?  Name    :  QBell Master Device Main Code   						?*/
/*?  Author  :  Jayasurya Ramasamy  									?*/
/*?  Date    :  2022/05/30	  										   	?*/
/*?  Version :  V1.1  											  	   	?*/
/*+---------------------------------------------------------------------?*/
/*?				    Function description		   			  	   		?*/
/*+---------------------------------------------------------------------?*/
/*?  1.																	?*/
/*?  2. 															 	?*/
/*?  3.																	?*/
/*?  4. 					   											?*/
/*?  5.					   												?*/
/*?  6.																   	?*/
/*?  7.																   	?*/
/*+---------------------------------------------------------------------|*/

/*----------------------------------------------------------------------*/
/*							Header								    	*/
/*----------------------------------------------------------------------*/


#include "main.h"

extern int_flags intF;

char* a;

u16 ADC_Value;


extern struct define_flag Mflag;
extern _BC3602_device_ BC3602_T;

/*------------------------------------------------------------------*/
/*							 Function								*/
/*------------------------------------------------------------------*/
//*********************************************//
// name		:	convert_integer_to_string
// input	:	char* const str -> pointer will stores that string 
//				value, unsigned char number -> number convert to string
// output	:	none
// function	:	for converting interger (unsigned short) to string(char*)
//*********************************************//
void convert_integer_to_string(char* const str, unsigned short number)
{
   
	u16 n, len = 0, i, rem;
	n = number;
	if(0 == n)
		len = 1;
	while (0 != n)
	{
		len++;
		n /= 10;	
	}
	 
	for ( i = 0; i < len; i++ )
	{
		rem = number % 10;
		number = number / 10;
		str[len - (i + 1)] = rem + '0';
	}
	str[len] = '\0';
}



/*------------------------------------------------------------------*/
/*							 Function								*/
/*------------------------------------------------------------------*/
//*********************************************//
// name		:	delay
// input	:	unsigned short time -> in milliseconds
// output	:	none
// function : 	Blocking delay
//*********************************************//

void delay(unsigned short time)
{
    unsigned short a;
	unsigned char b,c;
	
    for(a=0;a<time;a++)
    {
    	for(b=0;b<5;b++)
    	{
    		for(c=0;c<102;c++) asm("nop");
    	}
    }
}



/*------------------------------------------------------------------*/
/*							 Function								*/
/*------------------------------------------------------------------*/
//*********************************************//
// name		:	sysInit
// input	:	none
// output	:	none
// function :	which includes watchdog time clear, uart init, adc init,
//				i2c init, ktd rgb led driver init and other gpio inits
//*********************************************//
void sysInit()
{
    WDTC();
    uart_prot_init();
    delay(100);
    adc_init();
    IIC_Init();
    ktd_en();
    ktd_clear();
    delay(100);
   	int_init();
    delay(1000); 
}

void main()
{
    Sys_init();  //Init function for the RF mode

   	sysInit();   //Init function for the adc, rgb, gpio and watchdog timer control

	RF_Init();																				// RF Initialization
	RF_Parameter_loading();
//	BC3602_WriteRegister(IO2_REGS,0xcd);													// debug use
	BC3602_T.mode = RF_RX;																	// set RF mode flag as RX mode
	
	BC3602_T.mode = RF_RX;	

	delay(100);
	while(1)
	{
		GCC_CLRWDT();		//Watchdog timer clear
		//ktd_clockwise_ring(120, 23, 45, 10);

		/*
			- 	Checking the rgb blink flag tiggred or not
				blink timing will be set by PB0 and PB1 input.	
		*/
		if(intF.rgbBlink_flag)
		{
			intF.rgbBlink_flag = 0;
			ktd_clear();
			unsigned char i;
			sw0_status_check();
			delay(10);
			for(i = 0; i<intF.sw0_status_flag; i++)
			{
				ktd_rgb_ctrl(164, 50, 168);
				delay(500);
				ktd_clear();
				delay(500);
			}
			uart_printf("Finshed\n");
		}	
		

		/*
			-	Checking the data flag, if packet is received or not
				if packet received send the ack to vibrator and enable the
				RGB blink flag.
		*/
		if(1 == intF.int0_isr_flag)
        {
        	uart_printf("Sending ACK\n");			
           	intF.int0_isr_flag = 0;

          	if(BC3602_T.mode != RF_TX)														// while not being tx mode
			{
				uart_printf("Insied the !RX_TX\n");
				Mflag.Key_Stas = 0x01;
				KEY_Process(Mflag.Key_Stas);												// while not being tx mode
			}
			
			uart_printf("Inside the Mflag key\n");
			Mflag.Key_Stas = 0;						
           	intF.int0_isr_flag = 0;
			intF.rgbBlink_flag = 1;
			delay(100);
        }
		
		/*
			-	Processing the RF Tx and Rx mode 
		*/
        if(BC3602_T.mode == RF_TX)															// while RF mode flag = TX
			SimpleFIFO_TX_Process(&BC3602_T);												// RF Simple FIFO TX porcess
			
		if(BC3602_T.mode == RF_RX)															// while RF mode flag = RX
			SimpleFIFO_RX_Process(&BC3602_T);												// RF Simple FIFO RX porcess	
		
		RF_Finished_Process(&BC3602_T);	

	}
}


/*------------------------------------------------------------------*/
/*							 Function								*/
/*------------------------------------------------------------------*/
//*********************************************//
// name		:	sw0_status_check
// input	:	none
// output	:	none
// function :	For checking the SW1 status for RGB blink interval
//				 ------------------------------------- 
//				|  PB0 |  PB1  |  Interval in Seconds |
//				 -------------------------------------
//				|	0  |   0   |		10			  |
//				|	0  |   1   |		10			  |
//				|	1  |   0   |        20			  |
//				|   1  |   1   |		15            |
//				 -------------------------------------
//*********************************************//
void sw0_status_check(void)
{
	if(1 == SW2_PB0 && 1 == SW2_PB1)
	{
		uart_printf("in 15 mode\n");
		intF.sw0_status_flag = SEC_15;
	}
	else if(1 == SW2_PB0 && 0 == SW2_PB1)
	{
		uart_printf("in 20 mode\n");
		intF.sw0_status_flag =  SEC_20;
	}
	else if(0== SW2_PB0 && 1 == SW2_PB1)
	{
		uart_printf("in 10 mode\n");
		intF.sw0_status_flag = SEC_10;
	}
	else
	{
		uart_printf("in 10 mode\n");
		intF.sw0_status_flag = SEC_10;
	}
	delay(10);
}