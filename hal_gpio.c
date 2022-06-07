#include "hal_gpio.h"

struct int_flags intF;


/*------------------------------------------------------------------*/
/*							 Function								*/
/*------------------------------------------------------------------*/
//*********************************************//
// name		:	gpio_init
// input	:	none
// output	:	none
// function :   initiate the gpio for interrupts
//              and buzzer
//*********************************************//
void gpio_init()
{
#if INT0_EN
    _pbc0 = 1;  _pbpu0 = 1;
    _pbs01 = 0; _pbs00 = 0;
#endif
#if INT1_EN
    _pbc1 = 1;  _pbpu1 = 1;
    _pbs03 = 0; _pbs02 = 0;
#endif

    BUZZER_PIN = 0;
    BUZZER_PIN_CTR = 0;
}


/*------------------------------------------------------------------*/
/*							 Function								*/
/*------------------------------------------------------------------*/
//*********************************************//
// name		:	int_init
// input	:	none
// output	:	none
// function :   gpio init function call and interrup configuration
//              interrup enable
//*********************************************//
void int_init()
{
    gpio_init();
    INT0_EDGE_FALLING_CTR();
    INT1_EDGE_BOTH_CTR();
    INT0_REG_FLAG_NOREQ(); //Interrupt0 flag cleared
    INT1_REG_FLAG_NOREQ();
    INT0_ISR_CTR_EN(); //Enable external interrupt
    INT1_ISR_CTR_EN();

    ISR_CTR_EN();
    //delay(1000);
}


/*------------------------------------------------------------------*/
/*							 Function								*/
/*------------------------------------------------------------------*/
//*********************************************//
// name		:	buzzer_ctrl
// input	:	none
// output	:	none
// function :   Contorling the buzzer
//*********************************************//
void buzzer_ctrl(unsigned char value)
{
    if(value)
        BUZZER_PIN = 1;
    else
        BUZZER_PIN = 0;
}

/*
extern void __attribute((interrupt(0x04))) INT0_proc(void)
{
	//intF.int0_isr_flag = 1;
	_nop();
}

extern void __attribute((interrupt(0x24))) INT1_proc(void)
{
    //intF.int1_isr_flag = 1;
	_nop();
}

*/