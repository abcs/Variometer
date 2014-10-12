/*
 * eeprom.h
 *
 *  Created on: 2014.10.12.
 *      Author: csabi
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>
#include "logger.h"

typedef struct ee_descriptor_s {
	uint32_t first_free_rec;
} ee_descriptor_t;

int ee_write_log_rec(log_rec_t * record);
int ee_read_log_rec(log_rec_t * record, uint32_t rec_pointer);
uint32_t ee_get_first_free_pointer(void);

#endif /* EEPROM_H_ */
