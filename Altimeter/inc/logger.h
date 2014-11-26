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

/*!
 * A naplózandó rekord struktúrája.
 */
typedef struct log_rec_s {
	uint8_t year;		/*!< Év */
	uint8_t month;		/*!< Hó (Az MSB az évszázad jelölő; 0 - 1900, 1 - 2000) */
	uint8_t day;		/*!< Nap */
	uint8_t hour;		/*!< Óra */
	uint8_t minute;		/*!< Perc */
	uint8_t second;		/*!< Másodperc */
	uint16_t altitude;	/*!< Magasság */
} log_rec_t;

/*!
 * A ringbufferben tárolt elemek struktúrája.
 */
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
