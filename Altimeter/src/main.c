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
BinarySemaphore binSem_T5;
volatile bool_t canT5Run = FALSE;

/*!
 * A szoftver belépési pontja.
 */
int main(void) {

  HP03_meas_t temp_press; // = { -3333, 0};

  /*!
   * Rendszer-inicializálás.
   * - HAL inicializálás (SoC és kártya specifikus)
   * - Kernel inicializálás, a main() függvényből szál lesz és az RTOS elindul.
   */
  halInit();
  chSysInit();
  chBSemInit(&binSem_T5, FALSE);

  /*!
   * Az SPI és az I2C aktiválása.
   */
//  sdStart(&SD1, NULL);                  /* Default: 38400,8,N,1.            */
  spiStart(&SPID1, &spicfg);
  i2cStart(&I2CD1, &i2ccfg);

  /*!
   * A perifériák és modulok inicializálása.
   */
  LCD_init();
  RTC_init();

  HP03_reset();
  HP03_readCoeffs();

  VS_setupHardware();

  logger_init();

  /*!
   * A tengerszintre átszámított légnyomás kiszámíttatása.
   * Az első kiolvasás a szenzorból nem biztos, hogy helyes,
   * ezért két kiolvasás szükséges.
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

  /*!
   * Aktuális dátum kiolvasása az RTC-ből.
   */
  actualDate = RTC_getDate();

  /*!
   * Hangszóró tesztelése (PWM).
   */
  pwmStart(&PWMD3, &pwmcfg);
  pwmEnableChannel(&PWMD3, 0, 50);
  chThdSleepMilliseconds(50);
  pwmDisableChannel(&PWMD3, 0);

  /*!
   * Háttérvilágítás bekapcsolása.
   */
  palSetPad(GPIO1, GPIO1_BACKLIGHT);

  /*!
   * Szálak létrehozása.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), LOWPRIO, Thread1, NULL);
  chThdCreateStatic(waThread2, sizeof(waThread2), HIGHPRIO, Thread2, NULL);
  chThdCreateStatic(waThread3, sizeof(waThread3), LOWPRIO, Thread3, NULL);
  chThdCreateStatic(waThread4, sizeof(waThread4), NORMALPRIO, Thread4, NULL);
  chThdCreateStatic(waThread5, sizeof(waThread5), LOWPRIO, Thread5, NULL);

  while (TRUE) {
    if ( !palReadPad(GPIO1, GPIO1_SW3) ) {
    	palTogglePad(GPIO1, GPIO1_BACKLIGHT);
    	while ( !palReadPad(GPIO1, GPIO1_SW3) )
    		chThdSleepMilliseconds(75);
    }

    if ( !palReadPad(GPIO1, GPIO1_SW2) ) {

    	canT5Run = !canT5Run;

    	while ( !palReadPad(GPIO1, GPIO1_SW2) )
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
