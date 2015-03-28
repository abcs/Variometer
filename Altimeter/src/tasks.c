/*
 * tasks.c
 *
 *  Created on: 2014.10.13.
 *      Author: csabi
 */

#include "ch.h"
#include "hal.h"
#include "LCD.h"
#include "HP03.h"
#include "RTC_r2051.h"
#include "VirtualSerial.h"
#include "logger.h"
#include "periph.h"

extern volatile RTC_time_t actualTime;
extern volatile RTC_date_t actualDate;
extern volatile float calculatedSeaLevelPressure;
extern BinarySemaphore binSem_T5;
extern bool_t canT5Run;
extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;

/*!
 * "HeartBeat" LED villogtató szál.
 * (Az idő msec)
 */
msg_t Thread1(void *arg)
{
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

/*!
 * Nyomás és hőmérséklet kiolvasó szál.
 */
msg_t Thread2(void *arg)
{
	log_rec_t * to_log;

	HP03_meas_t temp_press; // = { -3333, 0};
    int measuredAltitude = 0;
    int prevAlt = 0;

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

 	    if ((prevAlt - measuredAltitude) != 0)
 	    {
 	    	pwmEnableChannel(&PWMD3, 0, 50);
 	    }
 	    else
 	    {
 	    	pwmDisableChannel(&PWMD3, 0);
 	    }

 	    LCD_WriteSpeed(prevAlt, measuredAltitude, 3);
	    LCD_writePress(temp_press);
	    LCD_writeAlt(measuredAltitude);

	    prevAlt = measuredAltitude;

 	    if ( (readTemp % 3) == 0 )
 	    {
			to_log = (log_rec_t *)chHeapAlloc(NULL, sizeof(log_rec_t)); //loggerHeap

			if (to_log)
			{
				to_log->altitude = (uint16_t)measuredAltitude;
				to_log->year = actualDate.year;
				to_log->month = actualDate.month;
				to_log->day = actualDate.day;
				to_log->hour = actualTime.hour;
				to_log->minute = actualTime.minute;
				to_log->second = actualTime.second;

				(void)logger_logThis(to_log);
			}
 	    }

	    chThdSleepMilliseconds(325);
    }
    return 0;
}


/*!
 * Dátum és idő kezelő szál.
 */
msg_t Thread3(void *arg)
{
	(void)arg;
    chRegSetThreadName("date_time");

    while (TRUE) {
    	actualTime = RTC_getTime();

    	if( (2 > actualTime.second) && (0 == actualTime.minute) && (0 == actualTime.hour) )
    	{
    		actualDate = RTC_getDate();
    	}

    	LCD_writeTime(actualTime);

        chThdSleepMilliseconds(100);
    }
    return 0;
}

/*!
 * USB kezelő szál.
 */
msg_t Thread4(void *arg)
{
	(void)arg;
    chRegSetThreadName("USB_CDC");

	for (;; ) {
//		vTaskSuspendAll();

		if(USB_DeviceState[0] == DEVICE_STATE_Configured) {
			LCD_writeUSB();
		} else {
			LCD_writeUSB_delete();
		}

//		VS_echoCharacter();
		VS_USBdataHandling();
		USB_USBTask(VirtualSerial_CDC_Interface.Config.PortNumber, USB_MODE_Device); //VirtualSerial_CDC_Interface.Config.PortNumber, USB_MODE_Device

//	    xTaskResumeAll();

	    chThdSleepMilliseconds(50);
	}
	return 0;
}

/*!
 * EEPROM író szál (ringbuffer -> EEPROM)
 */
msg_t Thread5(void *arg)
{
	(void)arg;
    chRegSetThreadName("LOGG_WR_EE");

    for (;; ) {
    	chBSemWait(&binSem_T5);
    	if (canT5Run) {
    		LCD_writeLOG();
    		(void)logger_writeToEE();
    	} else {
    		LCD_writeLOG_delete();
    	}
		chBSemSignal(&binSem_T5);
        chThdSleepMilliseconds(750);
	}
	return 0;
}
