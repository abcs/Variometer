/*
 * Kalman.c
 *
 *  Created on: 2014.10.04.
 *      Author: csabi
 */

#include "Kalman.h"

volatile static int x_est_last = 0;
volatile static float P_last = 0;

/*!
* A mérés zaja.
*/
volatile static const float Q = 0.15;
volatile static const float R = 0.55;

/*!
* A Kálmán szűrő inicializálása.
* @param[in]	z_measured	A mért érték.
*/
void init_kalman(int z_measured)
{
	x_est_last = z_measured;
}

/*!
* Elvégzi a szűrést (jóslás, erősítés, korrekció).
* @param[in]	z_measured	A mért érték.
* @return		Számított (jósolt) érték.
*/
int kalman(int z_measured)
{
    float K;
    float P;
    int x_est;

    P = P_last + Q;		/*!< Jóslás */

    K = P / (P + R);	/*!< Kálmán erősítés számítása */

    x_est = (float)x_est_last + K * (float)(z_measured - x_est_last); /*!< Korrekció */

    /*! Utolsó érték frissítése */
    P_last = (1.0F - K) * P;
    x_est_last = x_est;

    return (x_est);
}
