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
    
    OutPortByte( RTC_INDEX_REG, RTC_STATUS_A );    //check status - read access
    TimeOut = 1000;
    while ( InPortByte( RTC_VALUE_REG ) & 0x80 )
        if ( TimeOut < 0 )
            return;
        else
            TimeOut--;
    
    OutPortByte( RTC_INDEX_REG, RTC_DAY );         //get day value
    lpSystemTime->wDay = InPortByte( RTC_VALUE_REG );
    OutPortByte( RTC_INDEX_REG, RTC_MONTH );       //get month value
    lpSystemTime->wMonth = InPortByte( RTC_VALUE_REG );
    OutPortByte( RTC_INDEX_REG, RTC_YEAR );        //get year
    lpSystemTime->wYear = InPortByte( RTC_VALUE_REG );
    
    OutPortByte( RTC_INDEX_REG, RTC_DAY_OF_WEEK ); //get day of week - **** problem
    lpSystemTime->wDayOfWeek = InPortByte( RTC_VALUE_REG );
    
    OutPortByte( RTC_INDEX_REG, RTC_SECOND );
    lpSystemTime->wSecond = InPortByte( RTC_VALUE_REG );
    OutPortByte( RTC_INDEX_REG, RTC_MINUTE );
    lpSystemTime->wMinute = InPortByte( RTC_VALUE_REG );
    OutPortByte( RTC_INDEX_REG, RTC_HOUR );
    lpSystemTime->wHour = InPortByte( RTC_VALUE_REG );
    
    OutPortByte( RTC_INDEX_REG, RTC_STATUS_B );
    OutPortByte( RTC_VALUE_REG, 2 );
    
    return;
}
/* Returns 1 on success and 0 on failue */
BYTE SetLocalTime(SYSTEMTIME * lpSystemTime)
{
    /* Checking whether we can read the time now or not according to some documentation the MSB in Status A will   remain 1 (invalid time) only a millisecond*/
    int TimeOut;
    
    OutPortByte( RTC_INDEX_REG, RTC_STATUS_A );    //checking status -read access
    TimeOut = 1000;
    while ( InPortByte(RTC_VALUE_REG) & 0x80 )
        if ( TimeOut< 0 )
            return 0;
        else
            TimeOut--;
    
    OutPortByte( RTC_INDEX_REG, RTC_DAY );
    OutPortByte( RTC_VALUE_REG, lpSystemTime->wDay );
    OutPortByte( RTC_INDEX_REG, RTC_MONTH );
    OutPortByte( RTC_VALUE_REG, lpSystemTime->wMonth );
    OutPortByte( RTC_INDEX_REG, RTC_YEAR );
    OutPortByte( RTC_VALUE_REG, lpSystemTime->wYear );

    OutPortByte( RTC_INDEX_REG, RTC_SECOND );
    OutPortByte( RTC_VALUE_REG, lpSystemTime->wSecond );
    OutPortByte( RTC_INDEX_REG, RTC_MINUTE );
    OutPortByte( RTC_VALUE_REG, lpSystemTime->wMinute );
    OutPortByte( RTC_INDEX_REG, RTC_HOUR );
    OutPortByte( RTC_VALUE_REG, lpSystemTime->wHour );

    return 1;
}

