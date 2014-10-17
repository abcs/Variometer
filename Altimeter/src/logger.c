/*
 * logger.c
 *
 *  Created on: 2014.10.12.
 *      Author: csabi
 */

#include "logger.h"
#include "eeprom.h"

static log_buffer_t log_buffer;
static uint16_t first_free;
static uint16_t first_rec_to_read;

void logger_init()
{
	log_buffer.start_to_read = 0;
	log_buffer.can_write = 0;
	chMtxInit( &(log_buffer.mtx) ); /* Mutex initialization before use */
	first_free = ee_get_first_free_address();

	if( 0xFFFF == first_free )
		first_free = 0;

	first_rec_to_read = 0;
}

int logger_logThis(log_rec_t * rec_to_log)
{
	int rc = 0;

	chMtxLock( &(log_buffer.mtx) );

	if (log_buffer.active < LOG_BUFFER_SIZE)
	{
		log_buffer.active++;
		rc = 0;	/* Normal operation, there is not overflow */
	}
	else
	{
		/* Overwriting the oldest. Move start to next-oldest */
		chHeapFree(log_buffer.record[log_buffer.start_to_read]);
		log_buffer.start_to_read = (log_buffer.start_to_read + 1) % LOG_BUFFER_SIZE;
		rc = 1; /* Overflow */
	}

	log_buffer.record[log_buffer.can_write] = rec_to_log;
	log_buffer.can_write = (log_buffer.can_write + 1) % LOG_BUFFER_SIZE;

	chMtxUnlock();

	return rc;
}

int logger_writeToEE()
{
	int rc = 0;
	log_rec_t *log_rec_p;

	if (log_buffer.active)
	{
		log_rec_p = log_buffer.record[log_buffer.start_to_read];

		rc = ee_write_log_rec(log_rec_p, &first_free);
		if ( RDY_OK == rc )
		{
			log_buffer.start_to_read = (log_buffer.start_to_read + 1) % LOG_BUFFER_SIZE;
			log_buffer.active--;
			chHeapFree(log_rec_p);
		}
	}

	return (int)log_buffer.active;
}

int logger_readFromEE(log_rec_t * buffer, uint16_t size_in_rec)
{
	first_rec_to_read = ee_read_log_rec(buffer, first_rec_to_read * 10, size_in_rec);
}

int logger_deleteLog()
{
}
