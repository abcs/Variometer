/*
 * mymath.c
 *
 *  Created on: 2014.10.04.
 *      Author: csabi
 */

//#include <math.h>
#include "mymath.h"

#define MANTISSA_BITS 23
#define NULL_POINT   127
/*
static float log2(float i)
{
	return(log(i) / log(2));
}
*/

/*! A mantissza átlépésére szolgál,
 *  hogy a karakterisztikához jussunk.
 */
static float shift23 = (1 << MANTISSA_BITS);
//static float OOshift23=1.0/(1<<23);

static float floorf(float x)
{
	if (x >= 0.0f)
		return (float)((int)x);
	return (float)((int)x - 1);
}

/*
static float myLog2(float i)
{
	float LogBodge=0.346607f;
	float x;
	float y;
	x=*(int *)&i;
	x*= OOshift23; //1/pow(2,23);
	x=x-127;

	y=x-floorf(x);
	y=(y-y*y)*LogBodge;
	return x+y;
}
*/

/*! Kettes alapú logaritmust megvalósító függvény */
static float myLog2(float val)
{
   int * const  exp_ptr = (int *)(&val);
   int          x = *exp_ptr;
   const int    log_2 = ((x >> MANTISSA_BITS) & 255) - NULL_POINT - 1;
   x &= ~(255 << MANTISSA_BITS);
   x += NULL_POINT << MANTISSA_BITS;
   *exp_ptr = x;

   return (val + log_2);
}


static float myPow2(float i)
{
	float PowBodge = 0.31f;	//0.33971f
	float x;
	float y = i - floorf(i);
	y = (y - y * y) * PowBodge;

	x = i + NULL_POINT - y;
	x *= shift23; //pow(2,23);
	*(int*)&x = (int)x;
	return x;
}

float myPow(float a, float b)
{
	return myPow2(b * myLog2(a));
}
