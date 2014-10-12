/*
 * eeprom.h
 *
 *  Created on: 2014.10.12.
 *      Author: csabi
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include "logger.h"

int ee_write_log_rec(log_rec_t record);
log_rec_t ee_read_log_rec(uint32_t rec_pointer);
uint32_t ee_get_first_free_pointer(void);

#endif /* EEPROM_H_ */
