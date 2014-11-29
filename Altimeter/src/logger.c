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

/*!
* A naplózó alrendszer inicializálása.
*/
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

/*!
* A paraméterként kapott rekordot berakja a ringbufferbe,
* ahonnan később ki fog íródni az EEPROM-ba.
* @param[in] rec_to_log	A naplózandó rekord.
* @return	 0 - normál működés, 1 - túlcsordulás, a legrégebbi elem felülíródott.
*/
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

/*!
* A ringbufferből kiír egy rekordot az EEPROM-ba.
* Ha az írás sikeres volt, törli az elemet a bufferből.
* @return A ringbufferben maradt elemek száma.
*/
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

/*!
* Adott számú rekord kikérése az EEPROM-ból.
* @param[out] buffer	  Mutató az eredménybufferre.
* @param[in]  size_in_rec Rekordok száma.
* @return A következő kiolvasható rekord címe.
*/
uint16_t logger_readFromEE(log_rec_t * buffer, uint16_t size_in_rec)
{
	first_rec_to_read = ee_read_log_rec(buffer, first_rec_to_read * sizeof(log_rec_t), size_in_rec);
	return first_rec_to_read;
}

/*!
* Kitörli a naplóállományt az EEPROM-ból.
* (Még nincs implementálva.)
* @return 0.
*/
int logger_deleteLog()
{
}
