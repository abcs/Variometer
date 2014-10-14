/*
 * eeprom.c
 *
 *  Created on: 2014.10.12.
 *      Author: csabi
 */

#include "ch.h"
#include "hal.h"
#include "eeprom.h"

static int ee_write_descriptors()
{
	return 0;
}

int ee_write_log_rec(log_rec_t * record)
{
	int rc = 0;
/*
	i2cAcquireBus(&I2CD1);
	rc = i2cMasterTransmitTimeout( &I2CD1, (i2caddr_t)RTC_ADDRESS,
		                       RTC_control_regs, 2,
		                       0, 0, TIME_INFINITE );
	i2cReleaseBus(&I2CD1);
*/
	return rc;
};

int ee_read_log_rec(log_rec_t * record, uint32_t rec_pointer)
{
	return 0; //TODO: return with -1 if there is no record on the pointer address
};

uint16_t ee_get_first_free_address()
{
	uint16_t ff_addr = 0xFFFF;
	uint16_t control_block_address = 0x8000;
	msg_t rc = RDY_OK;

	i2cAcquireBus(&I2CD1);
	rc = i2cMasterTransmitTimeout( &I2CD1, (i2caddr_t)EE_ADDR_B1,
                                   (uint8_t *)(&control_block_address), sizeof(control_block_address),
                                   0, 0, TIME_INFINITE );

	if ( rc != RDY_OK )
	{
		i2cReleaseBus(&I2CD1);
		return 0xFFFF;
	}

//	chThdSleepMilliseconds(500);

	rc = i2cMasterReceiveTimeout( &I2CD1, (i2caddr_t)EE_ADDR_B1,
			                      (uint8_t *)(&ff_addr), sizeof(ff_addr), TIME_INFINITE );
	i2cReleaseBus(&I2CD1);

	if ( rc != RDY_OK )
		return 0xFFFF;

	return ff_addr;	//*(uint16_t *)
};

