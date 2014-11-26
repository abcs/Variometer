/*
 * RTC_r2051.h
 *
 *  Created on: 2014.05.03.
 *      Author: csabi
 */

#ifndef _RTC_R2051_H_
#define _RTC_R2051_H_

#define RTC_ADDRESS	0x32	/*!< Az RTC I2C címe */

/*!
 * BCD -> Bináris konvertáló makró.
 */
#define BCD_TO_BIN(toConvert)									\
            ((toConvert) - ( ((toConvert) >> 4) * 6 ))
//	((((toConvert) >> 4) * 10) + ((toConvert) & 0b00001111))

/*!
 * Bináris -> BCD konvertáló makró.
 */
#define BIN_TO_BCD(toConvert)									\
            ((toConvert) + ( ((toConvert) / 10) * 6 ))
//	((((toConvert) / 10) << 4) | ((toConvert) % 10))

/*!
 * Az idő tárolására használt struktúra.
 */
typedef struct RTC_time_s {
	uint8_t timeCounterAddress; //Fh start
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
} RTC_time_t;

/*!
 * A dátum tárolására használt struktúra.
 */
typedef struct RTC_date_s {
	uint8_t readFrom_Fh[3];
	uint8_t calendarCounterAddress;
	uint8_t dayOfWeek;
	uint8_t day;
	uint8_t month;	/*!< Az MSB az évszázadot jelöli; 0 - 1900, 1 - 2000 */
	uint8_t year;
} RTC_date_t;

void RTC_init(void);
void RTC_setTime(RTC_time_t timeToBeSet);
void RTC_setDate(RTC_date_t dateToBeSet);
RTC_time_t RTC_getTime(void);
RTC_date_t RTC_getDate(void);


#endif /* _RTC_R2051_H_ */
