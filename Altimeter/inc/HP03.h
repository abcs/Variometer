#ifndef _HP03_H_
#define _HP03_H_

#define HP03_ADDRESS_ADC  0x77	/*!< Szenzor I2C címe a méréshez. */
#define HP03_ADDRESS_EE   0x50	/*!< Szenzor I2C címe a koefficiensekhez. */

/*!
 * Struktúra a kiolvasott koefficiensek számára.
 */
typedef struct HP03_coeff_s {
	uint16_t    C1;
	uint16_t    C2;
	uint16_t    C3;
	uint16_t    C4;
	uint16_t    C5;
	uint16_t    C6;
	uint16_t    C7;
	uint8_t      A;
	uint8_t      B;
	uint8_t      C;
	uint8_t      D;
} HP03_coeff_t;

/*!
 * Struktúra a számításokhoz számára.
 */
typedef struct HP03_calc_s {
	int        dUT;
	int        OFF;
	int       SENS;
	int          X;
} HP03_calc_t;

/*!
 * Struktúra a mért értékek számára.
 */
typedef struct HP03_meas_s {
	int                       temper;
	int                        press;
	HP03_calc_t constsForCalculating;
} HP03_meas_t;

void HP03_reset(void);
void HP03_readCoeffs(void);
int HP03_getPressure(HP03_meas_t * constsIn_pressureOut, bool withKalman);
int HP03_getTemperature(HP03_meas_t * result);
int HP03_pressureToAltitude(float seaLevel, HP03_meas_t measuredPressTemp);
float HP03_pressureSeaLevelFromAltitude(float altitude, HP03_meas_t measuredPressTemp);

#endif
