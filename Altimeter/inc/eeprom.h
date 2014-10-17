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
	uint16_t cont_bl_address;
	uint16_t first_free_rec;
} ee_descriptor_t;

static int ee_write_descriptors(ee_descriptor_t * to_write);
int ee_write_log_rec(log_rec_t * record, uint16_t * address);
uint16_t ee_read_log_rec(log_rec_t * record, uint16_t from_addr, uint16_t num_of_rec);
uint16_t ee_get_first_free_address(void);

#endif /* EEPROM_H_ */
