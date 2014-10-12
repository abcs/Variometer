/*
 * Kalman.c
 *
 *  Created on: 2014.10.04.
 *      Author: csabi
 */

#include "Kalman.h"

volatile static int x_est_last = 0;
volatile static float P_last = 0;

//the noise in the system
volatile static const float Q = 0.15;
volatile static const float R = 0.55;

void init_kalman(int z_measured)
{
	x_est_last = z_measured;
}

int kalman(int z_measured)
{
    float K;
    float P;
    int x_est;

    //do a prediction
    P = P_last + Q;

    //calculate the Kalman gain
    K = P / (P + R);

    //correct
    x_est = (float)x_est_last + K * (float)(z_measured - x_est_last);

    //update our last's
    P_last = (1.0F - K) * P;
    x_est_last = x_est;

    return (x_est);
}
