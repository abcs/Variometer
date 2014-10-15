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

#define EE_ADDR_B0	0x51
#define EE_ADDR_B1	0x55


typedef struct ee_descriptor_s {
	uint16_t first_free_rec;
} ee_descriptor_t;

static int ee_write_descriptors(void);
int ee_write_log_rec(log_rec_t * record);
int ee_read_log_rec(log_rec_t * record, uint32_t rec_pointer);
uint16_t ee_get_first_free_address(void);

#endif /* EEPROM_H_ */
