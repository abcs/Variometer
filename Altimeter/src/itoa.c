/*
 * itoa.c
 *
 *  Created on: 2014.10.13.
 *      Author: csabi
 */

# include "itoa.h"

static void __reverse (char* start, char* end);

/*!
* Előjeles egészet karakter tömbbé konvertál.
* @param[in]	value	 		Az átalakítandó egész szám.
* @param[out]	buffer	 		Itt keletkezik az eredmény.
* @param[in]	base	 		Alap (2; 10; 16 lehet).
* @param[in]	decimals		Tizedesek száma.
* @param[in]	expectedLength	Kívánt karakterhosszúság.
* @param[in]	padding_char	Kitöltő karakter.
* @param[in]	prefix			Előtag, mely a karakterlánc elejéhez fűződik.
* @param[in]	suffix			Utótag, mely a karakterlánc végéhez fűződik.
* @return		Mutató az eredmény bufferre.
*/
char* itoa (int value, char* buffer, int base, int decimals,
		    int expectedLength, char padding_char, char * prefix, char * suffix) //, char * prefix, char * suffix
{
	static const char digits [] = "0123456789ABCDEF";
	char* tmpBuffer = buffer;
	int sign = 0;
	int quot, rem;
	int dec = 1;
	int prefLength = 0;

	if ( (base >= 2) && (base <= 16) )					/* check if the base is valid */
	{
		if ( base == 10 && (sign = value) < 0) 		/* check base & sign of value in the tmp copy */
		{
			value = -value;
		}

		while ( suffix && *suffix )
		{
			*buffer ++ = *suffix;
			suffix++;
		}

		do										/* calculate quotient & reminder */
		{
			quot = value / base;
			rem = value % base;
			*buffer ++ = digits[rem];				/* append the reminder to the string */
			if (decimals == dec)
			{
				*buffer ++ = '.';
			}
			dec ++;
		} while ( (value = quot) );

		if( (sign < base) && (base == 10) && (decimals == 1) )
		{
			*buffer ++ = '0';
		}

		if ( sign < 0 )							/* if negative value add a sign */
		{
			*buffer++ = '-';
		}

		while ( (prefix) && (*(prefix + prefLength)) )
		{
			prefLength++;
		}

		while ( (buffer - tmpBuffer + prefLength) < expectedLength ) // + prefLength
		{
			*buffer++ = padding_char;
		}

		while ( prefix && *prefix )
		{
			*buffer++ = *prefix;
			prefix++;
		}

		__reverse(tmpBuffer, buffer - 1);		/* reverse the string */
	}

	*buffer = '\0';
	return tmpBuffer;
}

/*!
* Megfordítja a karakterlánc sorrendjét.
* @param[in]	start	Mutató a sztring elejére.
* @param[in]	end		Mutató a sztring végére.
*/
static void __reverse (char* start, char* end)
{
	char tmp;

	while (end > start)
	{
		tmp = *end;						/* save end of the string int tmp variable */
		*end-- = *start;				/* write to the end of the string its beginning */
		*start++ = tmp;				/* and write to the beginning stuff from tmp */
	}
}
