/*
	Real Time Clock (Motorola MC146818)

	This program is writen by Sam on 4-Feb-2002
			    samueldotj@gmail.com
*/

#include "RTC.h"

/* This function will return the current system date & time in the passed pointer parameters. 

   Note : The year is only 2 digits and the RTC is Real Time Clock  and donot confuse it with the Y2K problem.

   The Day of week problem is still unsolved
*/
void GetLocalTime(SYSTEMTIME * lpSystemTime)
{
    /* Checking whether we can read the time now or not according to some documentation the MSB in Status A will remain 1 (invalid time) only a millisecond*/
    int TimeOut;
    
    outportByte( RTC_INDEX_REG, RTC_STATUS_A );    //check status - read access
    TimeOut = 1000;
    while ( inportByte( RTC_VALUE_REG ) & 0x80 )
        if ( TimeOut < 0 )
            return;
        else
            TimeOut--;
    
    outportByte( RTC_INDEX_REG, RTC_DAY );         //get day value
    lpSystemTime->wDay = inportByte( RTC_VALUE_REG );
    outportByte( RTC_INDEX_REG, RTC_MONTH );       //get month value
    lpSystemTime->wMonth = inportByte( RTC_VALUE_REG );
    outportByte( RTC_INDEX_REG, RTC_YEAR );        //get year
    lpSystemTime->wYear = inportByte( RTC_VALUE_REG );
    
    outportByte( RTC_INDEX_REG, RTC_DAY_OF_WEEK ); //get day of week - **** problem
    lpSystemTime->wDayOfWeek = inportByte( RTC_VALUE_REG );
    
    outportByte( RTC_INDEX_REG, RTC_SECOND );
    lpSystemTime->wSecond = inportByte( RTC_VALUE_REG );
    outportByte( RTC_INDEX_REG, RTC_MINUTE );
    lpSystemTime->wMinute = inportByte( RTC_VALUE_REG );
    outportByte( RTC_INDEX_REG, RTC_HOUR );
    lpSystemTime->wHour = inportByte( RTC_VALUE_REG );
    
    outportByte( RTC_INDEX_REG, RTC_STATUS_B );
    outportByte( RTC_VALUE_REG, 2 );
    
    return;
}
/* Returns 1 on success and 0 on failue */
BYTE SetLocalTime(SYSTEMTIME * lpSystemTime)
{
    /* Checking whether we can read the time now or not according to some documentation the MSB in Status A will   remain 1 (invalid time) only a millisecond*/
    int TimeOut;
    
    outportByte( RTC_INDEX_REG, RTC_STATUS_A );    //checking status -read access
    TimeOut = 1000;
    while ( inportByte(RTC_VALUE_REG) & 0x80 )
        if ( TimeOut< 0 )
            return 0;
        else
            TimeOut--;
    
    outportByte( RTC_INDEX_REG, RTC_DAY );
    outportByte( RTC_VALUE_REG, lpSystemTime->wDay );
    outportByte( RTC_INDEX_REG, RTC_MONTH );
    outportByte( RTC_VALUE_REG, lpSystemTime->wMonth );
    outportByte( RTC_INDEX_REG, RTC_YEAR );
    outportByte( RTC_VALUE_REG, lpSystemTime->wYear );

    outportByte( RTC_INDEX_REG, RTC_SECOND );
    outportByte( RTC_VALUE_REG, lpSystemTime->wSecond );
    outportByte( RTC_INDEX_REG, RTC_MINUTE );
    outportByte( RTC_VALUE_REG, lpSystemTime->wMinute );
    outportByte( RTC_INDEX_REG, RTC_HOUR );
    outportByte( RTC_VALUE_REG, lpSystemTime->wHour );

    return 1;
}

