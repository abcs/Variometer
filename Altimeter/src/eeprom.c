/*
 * eeprom.c
 *
 *  Created on: 2014.10.12.
 *      Author: csabi
 */

#include "ch.h"
#include "hal.h"
#include "eeprom.h"

typedef struct log_rec_ext_s {
	uint16_t ff_addr;
	log_rec_t record_to_write;
} log_rec_ext_t;


static int ee_write_descriptors(ee_descriptor_t * to_write)
{
	return 0;
}

int ee_write_log_rec(log_rec_t * record, uint16_t * address)
{
	uint8_t hibyte = 0;
	msg_t rc = -1;
	log_rec_ext_t to_write;
	ee_descriptor_t desc_to_write;

	to_write.ff_addr = *address;
	desc_to_write.cont_bl_address = 0x0080;	//0x8000 swapped

	/* swap bytes of Data */
    hibyte = ( to_write.ff_addr & 0xff00) >> 8;
    to_write.ff_addr <<= 8;
    to_write.ff_addr |= hibyte;

    to_write.record_to_write = *record;


	i2cAcquireBus(&I2CD1);
	rc = i2cMasterTransmitTimeout( &I2CD1, (i2caddr_t)EE_ADDR_B0,
                                   (uint8_t *)(&to_write), sizeof(log_rec_ext_t),
                                   0, 0, TIME_INFINITE ); //sizeof(to_write)

	if ( rc != RDY_OK )
	{
		i2cReleaseBus(&I2CD1);
		return 0xFFFF;
	}

	/* swap bytes of Data */
    hibyte = ( to_write.ff_addr & 0xff00) >> 8;
    to_write.ff_addr <<= 8;
    to_write.ff_addr |= hibyte;

    if (to_write.ff_addr > 0x8000 - sizeof(log_rec_t))
    {
    	to_write.ff_addr = 0x0000;
    }
    else
    {
    	to_write.ff_addr += sizeof(log_rec_t); //sizeof(log_rec_t);
    }

    desc_to_write.first_free_rec = to_write.ff_addr;

	rc = i2cMasterTransmitTimeout( &I2CD1, (i2caddr_t)EE_ADDR_B1,
                                   (uint8_t *)(&desc_to_write), sizeof(desc_to_write),
                                   0, 0, TIME_INFINITE );

	i2cReleaseBus(&I2CD1);

	if ( RDY_OK == rc )
	    *address = to_write.ff_addr;

	return rc;
};

uint16_t ee_read_log_rec(log_rec_t * record, uint16_t from_addr, uint16_t num_of_rec)
{
	uint8_t hibyte = 0;
//	uint16_t control_block_address = 0x0080;	//0x8000 swapped
	msg_t rc = RDY_OK;

	/* swap bytes of Data */
    hibyte = ( from_addr & 0xff00) >> 8;
    from_addr <<= 8;
    from_addr |= hibyte;

	i2cAcquireBus(&I2CD1);
	rc = i2cMasterTransmitTimeout( &I2CD1, (i2caddr_t)EE_ADDR_B0,
								   (uint8_t *)(&from_addr), sizeof(from_addr),
								   0, 0, TIME_INFINITE );

	if ( rc != RDY_OK )
	{
		i2cReleaseBus(&I2CD1);
		return 0xFFFF;
	}

	rc = i2cMasterReceiveTimeout( &I2CD1, (i2caddr_t)EE_ADDR_B0,
								  (uint8_t *)(record), num_of_rec * sizeof(log_rec_t), TIME_INFINITE );
	i2cReleaseBus(&I2CD1);

	if ( rc != RDY_OK )
		return 0xFFFF;

	/* swap bytes of Data */
    hibyte = ( from_addr & 0xff00) >> 8;
    from_addr <<= 8;
    from_addr |= hibyte;

	return (from_addr + num_of_rec * sizeof(log_rec_t)) / sizeof(log_rec_t); /* % 0x8000; */
}

uint16_t ee_get_first_free_address()
{
	uint16_t ff_addr = 0xFFFF;
	uint16_t control_block_address = 0x0080;	//0x8000 swapped
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

