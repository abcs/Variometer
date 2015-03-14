#include "ch.h"
#include "hal.h"
#include "HP03.h"
#include "globals.h"
#include <string.h>
//#include <math.h>
#include "mymath.h"
#include "Kalman.h"
//#include "VirtualSerial.h"
//#include "itoa.h"

/*
#define USB_send(ccSendTime)	do																			\
								{																			\
									if(USB_DeviceState[0] == DEVICE_STATE_Configured)						\
										CDC_Device_SendData(&VirtualSerial_CDC_Interface, (ccSendTime), 9);	\
								} while (0)
*/

static HP03_coeff_t HP03_coeff;

static int power(int base, int exp)
{
    int result = 1;
    while ( exp )
    {
    	result *= base;
    	exp--;
    }
    return result;
}

/*!
* Kiszámítja a magasságot (méterben) a hPa-ban megadott légnyomás,
* tengerszintre átszámított légnyomás és a °C-ban megadott hőmérséklet alapján.
* @param[in] seaLevel			Tengerszintre átszámított légnyomás hPa-ban.
* @param[in] measuredPressTemp	Mért légnyomás és hőmérséklet.
* @return						Magasság méterben.
*/
int HP03_pressureToAltitude(float seaLevel, HP03_meas_t measuredPressTemp)
{
  /* Hyposometric formula:                      */
  /*                                            */
  /*     ((P0/P)^(1/5.257) - 1) * (T + 273.15)  */
  /* h = -------------------------------------  */
  /*                   0.0065                   */
  /*                                            */
  /* where: h   = height (in meters)            */
  /*        P0  = sea-level pressure (in hPa)   */
  /*        P   = atmospheric pressure (in hPa) */
  /*        T   = temperature (in °C)           */

  float atmospheric = (float)measuredPressTemp.press / 10.0F;

  float result = ((myPow((seaLevel/atmospheric), 0.190267F) - 1.0F)
	              * ((float)measuredPressTemp.temper/10.0F + 273.15F)) / 0.0065F;
/*
  float result = 44330.77F * (1.0F - myPow(atmospheric / seaLevel, 0.1902));
*/
  return (int)(result * 10.0F);
}

/*!
* Kiszámítja a tengerszintre átszámított légnyomást hPa-ban
* az aktuális méterben megadott magasság, hPa-ban megadott légnyomás
* és a °C-ban megadott hőmérséklet alapján.
* @param[in] altitude			Magasság méterben.
* @param[in] measuredPressTemp	Mért légnyomás és hőmérséklet.
* @return						Tengerszintre átszámított légnyomás hPa-ban.
*/
float HP03_pressureSeaLevelFromAltitude(float altitude, HP03_meas_t measuredPressTemp)
{
  /* Sea-level pressure:                        */
  /*                                            */
  /*                   0.0065*h                 */
  /* P0 = P * (1 - ----------------- ) ^ -5.257 */
  /*               T+0.0065*h+273.15            */
  /*                                            */
  /* where: P0  = sea-level pressure (in hPa)   */
  /*        P   = atmospheric pressure (in hPa) */
  /*        h   = altitude (in meters)          */
  /*        T   = Temperature (in °C)           */

  float alt_0065 = 0.0065F * altitude;
  float atmospheric = (float)measuredPressTemp.press / 10.0F;
  float T = (float)measuredPressTemp.temper/10.0F;

  float seaLevelPress = atmospheric * myPow((1.0F - (alt_0065 /
               (T + alt_0065 + 273.15F))), -5.256F);
/*
  float seaLevelPress = atmospheric / myPow(1.0F - (altitude / 44330.77F), 5.2558F);
*/
  return seaLevelPress;
}


/*!
* Alaphelyzetbe állítja a légnyomás szenzort az XCLR kivezetésének
* alacsony szintre történő álíításával.
*/
void HP03_reset()
{
	palClearPad(GPIO1, GPIO1_XCLR);
//	chThdSleepMilliseconds(50);
//	palSetPad(GPIO1, GPIO1_XCLR);
//	temperature = -3333;
//	pressure = -3333;
}


/*!
* Kiolvassa a szenzorból a pontos hőmérséklet és légnyomás
* kiszámításához szükséges koefficiensek értékeit
* @return	HP03_coeff változó.
*/
void HP03_readCoeffs()
{
	uint8_t HP03_ee_addr = 16;
	uint8_t hibyte;
	msg_t rc = RDY_OK;
	uint16_t * tmpCoeff;
	int i;

	memset( &HP03_coeff, 0, sizeof(HP03_coeff_t) );

	palClearPad(GPIO1, GPIO1_XCLR);
	i2cAcquireBus(&I2CD1);
	rc = i2cMasterTransmitTimeout(&I2CD1, (i2caddr_t)HP03_ADDRESS_EE, &HP03_ee_addr, 1, (uint8_t *)(&HP03_coeff), sizeof(HP03_coeff_t), TIME_INFINITE );
	i2cReleaseBus(&I2CD1);
//	palSetPad(GPIO1, GPIO1_XCLR);

	tmpCoeff = (uint16_t *) &HP03_coeff;

	for(i = 0; i < 7; i++)
	{
	    hibyte = ( tmpCoeff[i] & 0xff00) >> 8;
	    tmpCoeff[i] <<= 8;
	    tmpCoeff[i] |= hibyte;
	}
}


/*!
* Kiolvassa a légnyomás szenzorból a nyomásértéket, majd a koefficiensek
* segítségével kiszámítja a pontos értéket.
* @param[in,out] constsIn_pressureOut Itt keletkezik az eredmény.
* @param[in]	 withKalman			  Használja-e a Kálmán szűrőt.
* @return		 0, ha sikeres.
*/
int HP03_getPressure(HP03_meas_t * constsIn_pressureOut, bool withKalman)
{
	uint16_t D = 0; //Measured and pressure
	msg_t rc = RDY_OK;
	uint8_t hibyte = 0;
	const uint8_t HP03_cmd_1[2] = {0xFF, 0xF0};
	const uint8_t HP03_cmd_2  = (uint8_t)0xFD;
	char actTime[10] = {0};

	constsIn_pressureOut->press = -3333;
/*
    itoa(chTimeNow(), actTime, 10, 0, 9, ' ', 0, 0);
	USB_send(actTime);
*/
	/* Get pressure and calculate dUT */
    palSetPad(GPIO1, GPIO1_XCLR);

	i2cAcquireBus(&I2CD1);
	rc = i2cMasterTransmitTimeout(&I2CD1, (i2caddr_t)HP03_ADDRESS_ADC, HP03_cmd_1, 2, 0, 0, TIME_INFINITE);
//	i2cReleaseBus(&I2CD1);

	chThdSleepMilliseconds(40);

//	i2cAcquireBus(&I2CD1);
	rc = i2cMasterTransmitTimeout(&I2CD1, (i2caddr_t)HP03_ADDRESS_ADC, &HP03_cmd_2, 1, (uint8_t *)(&D), 2, TIME_INFINITE);
	i2cReleaseBus(&I2CD1);

    if ( rc != RDY_OK )
	{
	    return rc;
	}

	/* swap bytes of Data */
    hibyte = ( D & 0xff00) >> 8;
    D <<= 8;
    D |= hibyte;

    palClearPad(GPIO1, GPIO1_XCLR);

	/* Calculating the real pressure */

    constsIn_pressureOut->constsForCalculating.X = ((constsIn_pressureOut->constsForCalculating.SENS *
    												(D - 7168)) >> 14) -
    												constsIn_pressureOut->constsForCalculating.OFF; // / 16384

    constsIn_pressureOut->press  = ((constsIn_pressureOut->constsForCalculating.X * 10) >> 5) +
    		                       (HP03_coeff.C7 + 150); // * 10; / 32

    if (withKalman)
    	constsIn_pressureOut->press  = kalman(constsIn_pressureOut->press);
/*
	itoa(chTimeNow(), actTime, 10, 0, 9, ' ', 0, 0);
	USB_send(actTime);
	USB_send(" - Press\n");
*/
    return RDY_OK;
}


/*!
* Kiolvassa a légnyomás szenzorból a hőmérsékletet, majd a koefficiensek
* segítségével kiszámítja a pontos értéket.
* @param[out]	result	Itt keletkezik az eredmény.
* @return		 0, ha sikeres.
*/
int HP03_getTemperature(HP03_meas_t * result)
{
	uint16_t D = 0; //Measured temperature
	msg_t rc = RDY_OK;
	uint8_t hibyte = 0;
	uint8_t * coeff_AB;
	const uint8_t HP03_cmd_1[2] = {0xFF, 0xE8};
	const uint8_t HP03_cmd_2  = (uint8_t)0xFD;
	char actTime[10] = {0};

	result->temper = -3333;
/*
	itoa(chTimeNow(), actTime, 10, 0, 9, ' ', 0, 0);
	USB_send(actTime);
*/
	/* Get temperature, pressure and calculate dUT */
    palSetPad(GPIO1, GPIO1_XCLR);

	i2cAcquireBus(&I2CD1);
	rc = i2cMasterTransmitTimeout(&I2CD1, (i2caddr_t)HP03_ADDRESS_ADC, HP03_cmd_1, 2, 0, 0, TIME_INFINITE);
//	i2cReleaseBus(&I2CD1);

	chThdSleepMilliseconds(40);

//	i2cAcquireBus(&I2CD1);
	rc = i2cMasterTransmitTimeout(&I2CD1, (i2caddr_t)HP03_ADDRESS_ADC, &HP03_cmd_2, 1, (uint8_t *)(&D), 2, TIME_INFINITE);
	i2cReleaseBus(&I2CD1);

    if ( rc != RDY_OK )
	{
	    return rc;
	}

	/* swap bytes of Data */
    hibyte = ( D & 0xff00) >> 8;
    D <<= 8;
    D |= hibyte;

    palClearPad(GPIO1, GPIO1_XCLR);

	/* Calculating the real temperature and dUT */
    if ( D >= HP03_coeff.C5 )
    {
    	coeff_AB = &HP03_coeff.A;
    }
    else
    {
    	coeff_AB = &HP03_coeff.B;
    }


    result->constsForCalculating.dUT = (D - HP03_coeff.C5) -
			   	   	   	   	   	   	   ( (D - HP03_coeff.C5) * (D - HP03_coeff.C5) >> 14 ) *
			   	   	   	   	   	   	   (( *coeff_AB ) >> HP03_coeff.C); //power(2, HP03_coeff.C) // / 16384

    result->constsForCalculating.OFF = ( HP03_coeff.C2 +
    		      	  	  	  	  	   (((HP03_coeff.C4 - 1024) *
    		      	  	  	  	  		result->constsForCalculating.dUT) >> 14)) << 2; // / 16384 // *4

    result->constsForCalculating.SENS = HP03_coeff.C1 +
    		                            ((HP03_coeff.C3 * result->constsForCalculating.dUT) >> 10);  // / 1024

    result->temper = 250 + ((result->constsForCalculating.dUT * HP03_coeff.C6) >> 16) -
    		         (result->constsForCalculating.dUT / (0x01 << HP03_coeff.D)); //250 -> 235 power(2, HP03_coeff.D)  // / 65536
/*
	itoa(chTimeNow(), actTime, 10, 0, 9, ' ', 0, 0);
	USB_send(actTime);
	USB_send(" - Temp\n");
*/
    return RDY_OK;
}
