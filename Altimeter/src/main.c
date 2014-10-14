/*
    HFLE7E - Copyright (C) 2014-2019
*/

#include "ch.h"
#include "hal.h"
#include "LCD.h"
#include "HP03.h"
#include "RTC_r2051.h"
#include "VirtualSerial.h"
#include "Kalman.h"
#include "logger.h"
#include "periph.h"
#include "tasks.h"

volatile RTC_time_t actualTime;
volatile RTC_date_t actualDate;
volatile float calculatedSeaLevelPressure = 0;

/*
 * Application entry point.
 */
int main(void) {

  HP03_meas_t temp_press; // = { -3333, 0};

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Activates the SD1 and SPI1 drivers.
   */
//  sdStart(&SD1, NULL);                  /* Default: 38400,8,N,1.            */
  spiStart(&SPID1, &spicfg);
  i2cStart(&I2CD1, &i2ccfg);

  /*
   * Initialize the peripherals and modules.
   */
  LCD_init();
  RTC_init();

  HP03_reset();
  HP03_readCoeffs();

  VS_setupHardware();

  logger_init();

  /*
   * Calculate the sea level pressure.
   * The first reading from HP03 is not always correct,
   * therefore two reading is needed.
   */
  (void)HP03_getTemperature(&temp_press);
  chThdSleepMilliseconds(25);
  (void)HP03_getTemperature(&temp_press);
  chThdSleepMilliseconds(25);
  (void)HP03_getPressure(&temp_press, false);
  chThdSleepMilliseconds(25);
  (void)HP03_getPressure(&temp_press, false);

  init_kalman(temp_press.press);
  calculatedSeaLevelPressure = HP03_pressureSeaLevelFromAltitude(25.0F, temp_press);

  /*
   * Set the actual date.
   */
  actualDate = RTC_getDate();

  /*
   * Testing the beeper.
   */
  pwmStart(&PWMD3, &pwmcfg);
  pwmEnableChannel(&PWMD3, 0, 50);
  chThdSleepMilliseconds(50);
  pwmDisableChannel(&PWMD3, 0);

  /*
   * Switch the back light on.
   */
  palSetPad(GPIO1, GPIO1_BACKLIGHT);

  /*
   * Creates the threads.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), LOWPRIO, Thread1, NULL);
  chThdCreateStatic(waThread2, sizeof(waThread2), HIGHPRIO, Thread2, NULL);
  chThdCreateStatic(waThread3, sizeof(waThread3), LOWPRIO, Thread3, NULL);
  chThdCreateStatic(waThread4, sizeof(waThread4), NORMALPRIO, Thread4, NULL);
  chThdCreateStatic(waThread5, sizeof(waThread5), LOWPRIO, Thread5, NULL);

  /*
   * Normal main() thread activity, in this demo it updates the 7-segments
   * display on the LPCXpresso main board using the SPI driver.
   */
  while (TRUE) {
    if ( !palReadPad(GPIO1, GPIO1_SW3) )
    {
    	palTogglePad(GPIO1, GPIO1_BACKLIGHT);
    	while ( !palReadPad(GPIO1, GPIO1_SW3) )
    		chThdSleepMilliseconds(75);
    }
    chThdSleepMilliseconds(125);
    // TestThread(&SD1);
    // spiSelect(&SPID1);
    // spiSend(&SPID1, 1, &digits[i]);                 /* Non polled method.   */
    // spiUnselect(&SPID1);
    // chThdSleepMilliseconds(500);
    // spiSelect(&SPID1);
    // spiPolledExchange(&SPID1, digits[i | 0x10]);    /* Polled method.       */
    // spiUnselect(&SPID1);
    // chThdSleepMilliseconds(500);
    // i = (i + 1) & 15;
  }
}
