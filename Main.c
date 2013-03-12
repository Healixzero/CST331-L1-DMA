/*******************************************************************************
* Brent Honzaki
* CST331
* Douglas Lynn
* Lab 01: DMA Lab
*******************************************************************************/

/* Include necessary libraries */
#include <pic32xxxx.h>
#include <plib.h>
#include <sys/types.h>	// dont forget the uint8_t libraries
#include dma.h

#pragma config POSCMOD=XT, FNOSC=PRIPLL, FPLIDIV=DIV_2, FPLLMUL=MUL_20
#pragma config FPLLODIV=DIV_1, FPBDIV=DIV_1, FWDTEN=OFF, CP=OFF, BWP=OFF
#pragma interrupt DMA0_Int ip17 vector 36
#pragma interrupt Button_Int ip17 vector 4


