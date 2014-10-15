/*
 * periph.h
 *
 *  Created on: 2014.10.13.
 *      Author: csabi
 */

#ifndef PERIPH_H_
#define PERIPH_H_

/*
 * SPI configuration (1MHz, CPHA=0, CPOL=0).
 */
extern SPIConfig spicfg;

/*
 * I2C configuration (400kHz).
 */
extern I2CConfig i2ccfg;

/*
 * PWM configuration
 */
void pwm3pcb(PWMDriver *pwmp);
void pwm3c0cb(PWMDriver *pwmp);

extern PWMConfig pwmcfg;

#endif /* PERIPH_H_ */
