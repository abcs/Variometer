/*
 * logger.h
 *
 *  Created on: 2014.10.12.
 *      Author: csabi
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdint.h>

typedef struct log_rec_s {
	uint8_t year;
	uint8_t month;	//The MSB is the century bit; 0 - 1900, 1 - 2000
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	int altitude;
} log_rec_t;

#endif /* LOGGER_H_ */
