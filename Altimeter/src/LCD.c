#include "ch.h"
#include "hal.h"
#include "LCD.h"
#include "LCD_fonts.h"
#include "globals.h"

volatile static bool isUSBWritten = false;

static inline void LCD_reset(bool_t active)
{
	if( active )
	{
		palClearPad(GPIO1, GPIO1_LCD_RESET);
	}
	else
	{
		palSetPad(GPIO1, GPIO1_LCD_RESET);
	}
}


static inline void LCD_CD(unsigned char CD)
{
	if( CD )
	{
		palSetPad(GPIO0, GPIO0_LCD_CD);
	}
	else
	{
		palClearPad(GPIO0, GPIO0_LCD_CD);
	}
}


static inline void reorderBytes(const unsigned char * from, unsigned char * to, int n)
{
	int k;

	for ( k = 0; k < n; k++ )
	{
		*(to + (n-k-1)) = *(from + k);
	}
}


static void LCD_writeLine(unsigned char row,
		           unsigned char col,
		           char * text,
		           unsigned char fontSize)
{
	int textLen = 0;
	unsigned char fonts[6] = {0};
	unsigned char fonts2[10] = {0};
	int i = 0;

	if ( (!text) || (col > 15) || (row > 8) || ((fontSize != 1) && (fontSize != 2)) )
		return;

    if ( (col *= (fontSize * 6)) > 90 )
    	return;

//    for ( textLen = 0; (*(text + textLen) != 0) && (textLen <= (16 / fontSize)); textLen++ );

    col = 96 - col;
    if (1 == fontSize)
    {
        for ( textLen = 0; (*(text + textLen) != 0) && (textLen <= 16); textLen++ );
        col -= (6 * textLen);
    }
    else
    {
        for ( textLen = 0; (*(text + textLen) != 0) && (textLen <= 9); textLen++ );
    	col -= (10 * textLen) + 6;
    }

    row |= 0x40;
    col |= 0x80;

    LCD_CD(CMD);
    spiSend(&SPID1, 1, &row);
    spiSend(&SPID1, 1, &col);

    LCD_CD(DATA);
    while ( (fontSize == 1) && (textLen > 0) )
    {
    	reorderBytes( &fonts1line[ (unsigned char)text[textLen - 1] ][0], fonts, 6);
        spiSend( &SPID1, 6, fonts );
        textLen--;
    }

    while ( (fontSize == 2) && (textLen > 0) )
    {
    	reorderBytes( &fonts2line[ (unsigned char)(text[textLen - 1] - 32) ][0][0], fonts2, 10);
        spiSend( &SPID1, 10, fonts2 );

        row ++;

        LCD_CD(CMD);
        spiSend(&SPID1, 1, &row);
        spiSend(&SPID1, 1, &col);
        LCD_CD(DATA);

        reorderBytes( &fonts2line[ (unsigned char)(text[textLen - 1] - 32) ][1][0], fonts2, 10);

        /* underline */
        for (i = 0; i < 10; i++)
        {
        	fonts2[i] |= 0x80;
        }

        spiSend( &SPID1, 10, fonts2 );

        row --;
        col += 10;
        LCD_CD(CMD);
        spiSend(&SPID1, 1, &row);
        spiSend(&SPID1, 1, &col);
        LCD_CD(DATA);

        textLen--;
    }
    LCD_CD(CMD);
}


// clear LCD
void LCD_clear()
{
	unsigned int i;
	unsigned char clrData[] = { 0x40U, 0x80U };

	LCD_CD(CMD);
	spiSend(&SPID1, 2, clrData);

	LCD_CD(DATA);
	clrData[0] = 0x00U;
	//clrData[1] = 0x00U;
	for( i = 0; i < 918; i++ )
		spiSend(&SPID1, 1, clrData);

	LCD_CD(CMD);
}


// init LCD
void LCD_init() //PCF8813
{
	unsigned char toSend = 0U;
	unsigned char initCommand[] = {0x39,	// PowerON, ExtCommandSet - 0x21
                                   0x08,	// Internal HV-gen x3 - 0x09
			                       0xE0,	// Set Vop
			                       0x16,	// Bias n=2    //15 - 0x16
			                       0x06,	// Temperature coeff 2
			                       0x38,	// StandartCommandSet - 0x20
			                       0x0C};	// normal mode, display non-inverted

	LCD_reset(TRUE);
	chThdSleepMilliseconds(10);
	LCD_reset(FALSE);

	LCD_CD(CMD);
	spiSend(&SPID1, 7, initCommand);

	toSend = 0x16;
	spiSend(&SPID1, 1, &toSend);
	toSend = 0x01;
	spiSend(&SPID1, 1, &toSend);

	LCD_clear();

	toSend = 0x0D;
	spiSend(&SPID1, 1, &toSend);

	chThdSleepMilliseconds(250);

	toSend = 0x0C;
	spiSend(&SPID1, 1, &toSend);
}


void LCD_writeTemp(HP03_meas_t measuredByHP03)
{
	char * prefix = ")";
	char * suffix = "$";
	char toLCD[10] = {0};

    itoa(measuredByHP03.temper, toLCD, 10, 1, 9, ' ', prefix, suffix);
    LCD_writeLine(1U, 0U, toLCD, 2U);
}


void LCD_writePress(HP03_meas_t measuredByHP03)
{
	char * prefix = "\'";
	char * suffix = "%";
	char toLCD[10] = {0};

	itoa(measuredByHP03.press, toLCD, 10, 1, 9, ' ', prefix, suffix);
    LCD_writeLine(3U, 0U, toLCD, 2U);
/*
	*toLCD = ')';
	*(toLCD+1) = '\0';
	Lcd_WriteLine(3U, 0U, toLCD, 2U);
    itoa(temp_press.temper, toLCD, 10, 1, 6, ' ');
    Lcd_WriteLine(3U, 1U, toLCD, 2U);
	*toLCD = '$';
	*(toLCD+1) = '\0';
	Lcd_WriteLine(3U, 7U, toLCD, 2U);
*/
    return;
}


void LCD_writeAlt(int altToWrite)
{
	char * prefix = "#";
	char * suffix = "!";
	char toLCD[10] = {0};

	itoa(altToWrite, toLCD, 10, 1, 9, ' ', prefix, suffix);
    LCD_writeLine(5U, 0U, toLCD, 2U);

    return;
}


void LCD_writeSpeed(int speedToWrite)
{
	return;
}


void LCD_writeDate(RTC_date_t dateToWrite)
{
	return;
}


void LCD_writeTime(RTC_time_t timeToWrite)
{
    char toLCD[4] = {0};

//    itoa(timeToWrite.second, toLCD, 10, 0, 2, '0', 0, 0);
//    Lcd_WriteLine(0U, 14U, toLCD, 1U);
/*
    *toLCD = ':';
    *(toLCD+1) = '\0';
    Lcd_WriteLine(0U, 13U, toLCD, 1U);
*/
    itoa(timeToWrite.minute, toLCD, 10, 0, 2, '0', 0, 0);
    LCD_writeLine(0U, 14U, toLCD, 1U);
/*
    *toLCD = ':';
    *(toLCD+1) = '\0';
    Lcd_WriteLine(0U, 10U, toLCD, 1U);
*/
    itoa(timeToWrite.hour, toLCD, 10, 0, 3, ' ', 0, ":");
    LCD_writeLine(0U, 11U, toLCD, 1U);
}

void LCD_writeUSB()
{
	if(isUSBWritten)
		return;

	char toLCD[4] = "USB";
	LCD_writeLine(0U, 0U, toLCD, 1U);
	isUSBWritten = true;
}

void LCD_writeUSB_delete()
{
	if(!isUSBWritten)
		return;

	char toLCD[4] = "   ";
	LCD_writeLine(0U, 0U, toLCD, 1U);
	isUSBWritten = false;
}
