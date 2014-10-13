/*
 * periph.c
 *
 *  Created on: 2014.10.13.
 *      Author: csabi
 */

#include "ch.h"
#include "hal.h"
#include "periph.h"

/*
 * SPI configuration (1MHz, CPHA=0, CPOL=0).
 */
volatile SPIConfig spicfg = {
  NULL,
  GPIO0,
  GPIO0_LCD_SEL, //GPIO0_SPI0SEL,
  CR0_DSS8BIT | CR0_FRFSPI | CR0_CLOCKRATE(0),
  48
};

/*
 * I2C configuration (400kHz).
 */
volatile I2CConfig i2ccfg = {
    I2C_STANDARD_MODE,             /**< @brief Specifies the I2C mode.        */
	48                             /**< @brief Specifies the clock timing     */
};

volatile PWMConfig pwmcfg = {
	100000,                          /**< @brief frequency                      */
	   100,                          /**< @brief period                         */
	pwm3pcb,                         /**< @brief callback                       */
    {
	    {PWM_OUTPUT_ACTIVE_LOW, pwm3c0cb},
	    {PWM_OUTPUT_ACTIVE_LOW, NULL}
    }
};

/*
 * PWM configuration
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
