/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/*
 * LPC11U14 drivers configuration.
 * The following settings override the default settings present in
 * the various device driver implementation headers.
 * Note that the settings for each driver only have effect if the driver
 * is enabled in halconf.h.
 *
 * IRQ priorities:
 * 3...0        Lowest...highest.
 */

/*
 * HAL driver system settings.
 */
#define LPC_PLLCLK_SOURCE                   SYSPLLCLKSEL_SYSOSC
#define LPC_SYSPLL_MUL                      4
#define LPC_SYSPLL_DIV                      4
#define LPC_MAINCLK_SOURCE                  SYSMAINCLKSEL_PLLOUT
#define LPC_SYSABHCLK_DIV                   1

/*
 * GPT driver system settings.
 */
#define LPC_GPT_USE_CT16B0                  TRUE
#define LPC_GPT_USE_CT16B1                  TRUE
#define LPC_GPT_USE_CT32B0                  TRUE
#define LPC_GPT_USE_CT32B1                  TRUE
#define LPC_GPT_CT16B0_IRQ_PRIORITY         2
#define LPC_GPT_CT16B1_IRQ_PRIORITY         2
#define LPC_GPT_CT32B0_IRQ_PRIORITY         2
#define LPC_GPT_CT32B1_IRQ_PRIORITY         2

/*
 * I2C driver system settings.
 */
#define PLATFORM_I2C_USE_I2C1               TRUE
#define LPC11xx_I2C_IRQ_PRIORITY			3

/*
 * PWM driver system settings.
 */
#define PLATFORM_PWM_USE_PWM1				TRUE
#define LPC11xx_PWM_USE_CT16B0				FALSE
#define LPC11xx_PWM_USE_CT16B1				FALSE
#define LPC11xx_PWM_USE_CT32B0				TRUE
#define LPC11xx_PWM_USE_CT32B1				FALSE
#define LPC11xx_PWM_USE_CT16B0_CH0			FALSE
#define LPC11xx_PWM_USE_CT16B0_CH1			FALSE
#define LPC11xx_PWM_USE_CT16B1_CH0			FALSE
#define LPC11xx_PWM_USE_CT16B1_CH1			FALSE
#define LPC11xx_PWM_USE_CT32B0_CH0			TRUE
#define LPC11xx_PWM_USE_CT32B0_CH1			FALSE
#define LPC11xx_PWM_USE_CT32B1_CH0			FALSE
#define LPC11xx_PWM_USE_CT32B1_CH1			FALSE
#define LPC11xx_PWM_CT16B0_IRQ_PRIORITY		3
#define LPC11xx_PWM_CT16B1_IRQ_PRIORITY		3
#define LPC11xx_PWM_CT32B0_IRQ_PRIORITY		3
#define LPC11xx_PWM_CT32B1_IRQ_PRIORITY		3

/*
 * SERIAL driver system settings.
 */
#define LPC_SERIAL_USE_UART0                TRUE
#define LPC_SERIAL_FIFO_PRELOAD             16
#define LPC_SERIAL_UART0CLKDIV              1
#define LPC_SERIAL_UART0_IRQ_PRIORITY       3

/*
 * SPI driver system settings.
 */
#define LPC_SPI_USE_SSP0                    TRUE
#define LPC_SPI_USE_SSP1                    FALSE
#define LPC_SPI_SSP0CLKDIV                  1
#define LPC_SPI_SSP1CLKDIV                  1
#define LPC_SPI_SSP0_IRQ_PRIORITY           1
#define LPC_SPI_SSP1_IRQ_PRIORITY           1
#define LPC_SPI_SSP_ERROR_HOOK(spip)        chSysHalt()

/*
 * USB driver system settings.
 */
#define LPC_USB_IRQ_PRIORITY				1

