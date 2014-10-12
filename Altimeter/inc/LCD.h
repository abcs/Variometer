#ifndef _LCD_H_
#define _LCD_H_

#include "HP03.h"
#include "itoa.h"
#include "RTC_r2051.h"


#define	CMD		0
#define DATA	1

void writeTemp(HP03_meas_t measuredByHP03);
void writePress(HP03_meas_t measuredByHP03);
void writeAlt(int altToWrite);
void writeSpeed(int speedToWrite);
void writeDate(RTC_date_t dateToWrite);
void writeTime(RTC_time_t timeToWrite);
void writeUSB(void);
void writeUSB_delete();
void Lcd_Init(void);
void Lcd_Clear(void);
/*
void Lcd_WriteLine(unsigned char row,
		           unsigned char col,
		           char * text,
		           unsigned char fontSize);
*/

#endif
