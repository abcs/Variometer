/*
 * logger.h
 *
 *  Created on: 2014.10.12.
 *      Author: csabi
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdint.h>
#include "ch.h"

#define LOG_BUFFER_SIZE 5

typedef struct log_rec_s {
	uint8_t year;
	uint8_t month;	//The MSB is the century bit; 0 - 1900, 1 - 2000
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint16_t altitude;
} log_rec_t;

typedef struct log_buffer_s {
	log_rec_t * record[LOG_BUFFER_SIZE];
	int8_t can_write;
	int8_t start_to_read;
	int8_t active;
	Mutex mtx;
} log_buffer_t;

void logger_init(void);
int logger_logThis(log_rec_t * rec_to_log);
int logger_writeToEE(void);
uint16_t logger_readFromEE(log_rec_t * buffer, uint16_t size_in_rec);
int logger_deleteLog(void);

#endif /* LOGGER_H_ */
