/*
 * eeprom.c
 *
 *  Created on: 2014.10.12.
 *      Author: csabi
 */

#include "ch.h"
#include "hal.h"
#include "eeprom.h"

int ee_write_log_rec(log_rec_t record)
{
	return 0;
};

log_rec_t ee_read_log_rec(uint32_t rec_pointer)
{
	log_rec_t a = {0};
	return a;
};

uint32_t ee_get_first_free_pointer(void)
{
	return 0;
};

