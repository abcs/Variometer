#ifndef _LCD_H_
#define _LCD_H_

#include "HP03.h"
#include "itoa.h"
#include "RTC_r2051.h"


#define	CMD		0
#define DATA	1

void LCD_writeTemp(HP03_meas_t measuredByHP03);
void LCD_writePress(HP03_meas_t measuredByHP03);
void LCD_writeAlt(int altToWrite);
void LCD_writeSpeed(int speedToWrite);
void LCD_writeDate(RTC_date_t dateToWrite);
void LCD_writeTime(RTC_time_t timeToWrite);
void LCD_writeUSB(void);
void LCD_writeUSB_delete(void);
void LCD_writeLOG(void);
void LCD_writeLOG_delete(void);
void LCD_init(void);
void LCD_clear(void);
/*
void LCD_writeLine(unsigned char row,
		           unsigned char col,
		           char * text,
		           unsigned char fontSize);
*/

#endif
