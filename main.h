#ifndef MAIN_H
#define MAIN_H

#include "BC66F3652.h"
#include "hal_i2c.h"
#include "hal_ktd.h"
#include "hal_adc.h"
#include "hal_uart.h"
#include "hal_gpio.h"
#include "hal_rf.h"

#define WDTC()                                  (_wdtc=0xa8)
#define NULL()									'\0'

#define SEC_10                                  10
#define SEC_15                                  15
#define SEC_20                                  20

//typedef unsigned char u8;
//typedef unsigned short u16;

extern void delay(unsigned short);
extern void sw0_status_check(void);

typedef struct int_flags
{
    unsigned char int0_isr_flag;
    unsigned char int1_isr_flag;
    unsigned char rgbBlink_flag;
    unsigned char sw0_status_flag;
}int_flags;


//int_flags intF;
//struct define_flag Mflag;


#endif