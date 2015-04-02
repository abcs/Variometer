/*
 * periph.c
 *
 *  Created on: 2014.10.13.
 *      Author: csabi
 */

#include "ch.h"
#include "hal.h"
#include "periph.h"

/*!
 * SPI konfiguráció (1MHz, CPHA=0, CPOL=0).
 */
SPIConfig spicfg = {
  NULL,
  GPIO0,
  GPIO0_LCD_SEL, //GPIO0_SPI0SEL,
  CR0_DSS8BIT | CR0_FRFSPI | CR0_CLOCKRATE(0),
  48
};

/*!
 * I2C konfiguráció (400kHz).
 */
I2CConfig i2ccfg = {
    I2C_FAST_MODE_PLUS,            /*!< @brief Az I2C mód beállítása. */
	48                             /*!< @brief Órajel beállítása.     */
};

/*!
 * PWM konfiguráció.
 */
PWMConfig pwmcfg = {
	100000,                          /*!< @brief Frekvencia. */
	   100,                          /**< @brief Periódus.   */
	pwm3pcb,                         /**< @brief Callback.   */
    {
	    {PWM_OUTPUT_ACTIVE_LOW, pwm3c0cb},
	    {PWM_OUTPUT_ACTIVE_LOW, NULL}
    }
};

/*!
 * Callback függvény.
 */
void pwm3pcb(PWMDriver *pwmp)
{
  (void)pwmp;
  palClearPad(GPIO1, GPIO1_SPEAKER);
}

void pwm3c0cb(PWMDriver *pwmp)
{
  (void)pwmp;
  palSetPad(GPIO1, GPIO1_SPEAKER);
}
