/*
    HFLE7E - Copyright (C) 2014-2019
*/

#include "ch.h"
#include "hal.h"
#include "LCD.h"
#include "HP03.h"
#include "RTC_r2051.h"
#include "VirtualSerial.h"
#include "globals.h"
#include "Kalman.h"
#include "logger.h"

static RTC_time_t actualTime;
static RTC_date_t actualDate;
//static MemoryHeap *loggerHeap;


/*
 * SPI configuration (1MHz, CPHA=0, CPOL=0).
 */
static SPIConfig spicfg = {
  NULL,
  GPIO0,
  GPIO0_LCD_SEL, //GPIO0_SPI0SEL,
  CR0_DSS8BIT | CR0_FRFSPI | CR0_CLOCKRATE(0),
  48
};


/*
 * I2C configuration (400kHz).
 */
static I2CConfig i2ccfg = {
    I2C_STANDARD_MODE,             /**< @brief Specifies the I2C mode.        */
	48                             /**< @brief Specifies the clock timing     */
};

volatile static float calculatedSeaLevelPressure = 0;

/* PWM */
static void pwm3pcb(PWMDriver *pwmp) {

  (void)pwmp;
  palClearPad(GPIO1, GPIO1_SPEAKER);
}

static void pwm3c0cb(PWMDriver *pwmp) {

  (void)pwmp;
  palSetPad(GPIO1, GPIO1_SPEAKER);
}

/*
static const PWMChannelConfig pwmchn = {
	{PWM_OUTPUT_ACTIVE_LOW, pwm3c0cb},
	{PWM_OUTPUT_ACTIVE_LOW, NULL}
};
*/

static PWMConfig pwmcfg = {
	100000,                          /**< @brief frequency                      */
	   100,                           /**< @brief period                         */
	pwm3pcb,                         /**< @brief callback                       */
    {
	    {PWM_OUTPUT_ACTIVE_LOW, pwm3c0cb},
	    {PWM_OUTPUT_ACTIVE_LOW, NULL}
    }
};


/* End PWM */

/*
 * Red LED blinker thread, times are in milliseconds.
 */
static WORKING_AREA(waThread1, 48);
static msg_t Thread1(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker1");

  while (TRUE) {
    palClearPad(GPIO0, GPIO0_LED2);
    chThdSleepMilliseconds(500);
    palSetPad(GPIO0, GPIO0_LED2);
    chThdSleepMilliseconds(500);
  }
  return 0;
}

/*
 * Pressure and temperature reader thread, times are in milliseconds.
 */
static WORKING_AREA(waThread2, 208);
static msg_t Thread2(void *arg)
{
	log_rec_t * to_log;

    HP03_meas_t temp_press; // = { -3333, 0};
    int measuredAltitude = 0;

    (void)arg;
    chRegSetThreadName("temp_press");
    uint8_t readTemp = 0;

//    chHeapInit(loggerHeap, to_log, sizeof(to_log) * LOG_BUFFER_SIZE);

    while (TRUE) {
    	if ( 0 == readTemp )
    	{
    		(void)HP03_getTemperature(&temp_press);
    	    LCD_writeTemp(temp_press);
    		readTemp = 23;
    	}

    	(void)HP03_getPressure(&temp_press, true);
    	readTemp--;

 	    measuredAltitude = HP03_pressureToAltitude(calculatedSeaLevelPressure, temp_press);

 	    if ( (readTemp % 4) == 0 )
 	    {
			to_log = (log_rec_t *)chHeapAlloc(NULL, sizeof(log_rec_t)); //loggerHeap

			if (to_log)
			{
				to_log->altitude = measuredAltitude;
				to_log->year = actualDate.year;
				to_log->month = actualDate.month;
				to_log->day = actualDate.day;
				to_log->hour = actualTime.hour;
				to_log->minute = actualTime.minute;
				to_log->second = actualTime.second;

				(void)logger_logThis(to_log);
			}
 	    }

	    LCD_writePress(temp_press);
	    LCD_writeAlt(measuredAltitude);

	    chThdSleepMilliseconds(50);
    }
    return 0;
}


/*
 * Date and time handler thread, times are in milliseconds.
 */
static WORKING_AREA(waThread3, 152);
static msg_t Thread3(void *arg)
{
	(void)arg;
    chRegSetThreadName("date_time");

    while (TRUE) {
    	actualTime = RTC_getTime();

    	if( 2 > actualTime.second && 0 == actualTime.minute && 0 == actualTime.hour )
    	{
    		actualDate = RTC_getDate();
    	}

    	LCD_writeTime(actualTime);

        chThdSleepMilliseconds(100);
    }
    return 0;
}

static WORKING_AREA(waThread4, 184);
static msg_t Thread4(void *arg)
{
	(void)arg;
    chRegSetThreadName("USB_CDC");

	for (;; ) {
//		vTaskSuspendAll();

		if(USB_DeviceState[0] == DEVICE_STATE_Configured)
			LCD_writeUSB();
		else
			LCD_writeUSB_delete();

		VS_echoCharacter();
		USB_USBTask(VirtualSerial_CDC_Interface.Config.PortNumber, USB_MODE_Device); //VirtualSerial_CDC_Interface.Config.PortNumber, USB_MODE_Device
//	    xTaskResumeAll();

	    chThdSleepMilliseconds(30);
	}
	return 0;
}

static WORKING_AREA(waThread5, 64);
static msg_t Thread5(void *arg)
{
	(void)arg;
    chRegSetThreadName("LOGG_WR_EE");

	for (;; ) {
		(void)logger_writeToEE();
	    chThdSleepMilliseconds(800);
	}
	return 0;
}


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

  logger_init();
  LCD_init();
  RTC_init();

  palSetPad(GPIO1, GPIO1_BACKLIGHT);

  HP03_reset();
  HP03_readCoeffs();

  VS_setupHardware();

  actualDate = RTC_getDate();


  pwmStart(&PWMD3, &pwmcfg);
//  chThdSleepMilliseconds(2000);

  pwmEnableChannel(&PWMD3, 0, 50);
//  pwmEnableChannel(&PWMD3, 1, 500);

  chThdSleepMilliseconds(50);

  pwmDisableChannel(&PWMD3, 0);
//  pwmDisableChannel(&PWMD3, 1);

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
