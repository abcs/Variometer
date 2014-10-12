/*
 * logger.c
 *
 *  Created on: 2014.10.12.
 *      Author: csabi
 */

#include "logger.h"
#include "eeprom.h"

static log_buffer_t log_buffer;
static uint32_t first_free;

void logger_init()
{
	log_buffer.start_to_read = 0;
	log_buffer.can_write = 0;
	chMtxInit( &(log_buffer.mtx) ); /* Mutex initialization before use */
	first_free = ee_get_first_free_pointer();
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

		rc = ee_write_log_rec(log_rec_p);
		if ( RDY_OK == rc )
		{
			log_buffer.start_to_read = (log_buffer.start_to_read + 1) % LOG_BUFFER_SIZE;
			log_buffer.active--;
			chHeapFree(log_rec_p);
		}
	}

	return (int)log_buffer.active;
}
