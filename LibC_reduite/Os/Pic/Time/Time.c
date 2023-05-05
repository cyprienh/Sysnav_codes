///////////////////////////////////////////////////////////////////////////////////////////////////
///  @file		Time.c
///  
///  @brief		Gestion du temps
///  
///  @date		15/04/2014
///  @author	N/A
///  @copyright	SYSNAV
///////////////////////////////////////////////////////////////////////////////////////////////////



#include "Time.h"
#include <Os/Os.h>

// Variable that holds micro seconds since start up - DO NOT READ DIRECTLY
Time global_time = 0;


///////////////////////////////////////////////////////////////////////////////////////////////////
// TIME interrupt function called every 10ms
///////////////////////////////////////////////////////////////////////////////////////////////////
void __attribute__((__interrupt__)) _T1Interrupt(void) 
{    
    global_time += 10000;
    WriteTimer1(0); // Reset timer
    IFS0bits.T1IF = 0; // Clear timer interrupt flag
}//_T1Interrupt


///////////////////////////////////////////////////////////////////////////////////////////////////
// TIME returns time since start up
///////////////////////////////////////////////////////////////////////////////////////////////////
Time getTime()
{
	return global_time + ReadTimer1();
};//getTime
