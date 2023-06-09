///////////////////////////////////////////////////////////////////////////////////////////////////
///  @file		System.c
///  
///  @brief		initialise tout le système
///  
///  @date		15/04/2014
///  @author	N/A
///  @copyright	SYSNAV
///////////////////////////////////////////////////////////////////////////////////////////////////



#include "system.h"
#include <Os/Pic/Time/Time.h>
#include <Os/Pic/Buffer/Buffer.h>
#include <Os/Pic/Include33e/timer.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
// HARDWARE CONFIG
///////////////////////////////////////////////////////////////////////////////////////////////////
// Config hardware boot (mask are only AND defined)

#if defined(__dsPIC33EP512MU810__) || defined(__dsPIC33EP512MU814__)
_FGS(GWRP_OFF & GSS_OFF & GSSK_OFF);
_FOSCSEL(FNOSC_FRC & IESO_ON);
_FOSC(POSCMD_HS & OSCIOFNC_ON & IOL1WAY_OFF & FCKSM_CSECME);
_FWDT(FWDTEN_OFF & WINDIS_OFF & PLLKEN_ON);
_FPOR(FPWRT_PWR1 & BOREN_OFF);
_FICD(ICS_PGD1 & RSTPRI_PF & JTAGEN_OFF);
_FAS(AWRP_OFF & APL_OFF & APLK_OFF);

#else
#error processeur non support�
#endif



///////////////////////////////////////////////////////////////////////////////////////////////////
// RAM allocation
///////////////////////////////////////////////////////////////////////////////////////////////////
/// structure contenant toutes les variables du syst�me
System sys							__attribute__((__near__));

///////////////////////////////////////////////////////////////////////////////////////////////////
// SYSTEM boot
///////////////////////////////////////////////////////////////////////////////////////////////////
int8 systemPowerUp() {
    // switch frequency => quartz 16Mhz
    PLLFBD = 15 - 2;
    CLKDIVbits.PLLPOST = 0;
    CLKDIVbits.PLLPRE = 0;
    OSCTUN = 0;
    RCONbits.SWDTEN = 0;
    __builtin_write_OSCCONH(0x03); // Initiate Clock Switch to Primary Oscillator with PLL (XTPLL, HSPLL, ECPLL)
    __builtin_write_OSCCONL(0x01); // Start clock switching while (OSCCONbits.COSC != 0b001);
    while (OSCCONbits.COSC != 0x03); // Wait for Clock switch to occur
    while (OSCCONbits.LOCK != 1);
   
    // Watchdog disabled
    RCONbits.SWDTEN = 0;
    //enable multi level interrupt
    INTCON1bits.NSTDIS = 0;
    // disable analog pin
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;
    ANSELE = 0;
    ANSELG = 0;

    ConfigIntTimer1(T1_INT_PRIOR_1 & T1_INT_ON); // Enable interrupt every period
    WriteTimer1(0); // Reset timer on power up
    // Timer1 has period 10000 with prescaler 1:8 so period is 10ms with 1us ticks
    OpenTimer1(T1_ON & T1_GATE_OFF & T1_IDLE_STOP &
               T1_PS_1_8 & T1_SYNC_EXT_OFF &
               T1_SOURCE_INT, 10000); 

    // Seconde timer for emptying the circular buffer
    ConfigIntTimer2(T2_INT_PRIOR_2 & T2_INT_ON); // Enable interrupt every period
    WriteTimer2(0); // Reset timer on power up
    // Timer1 has period 10000 with prescaler 1:8 so period is 10ms with 1us ticks
    OpenTimer2(T2_ON & T2_GATE_OFF & T2_IDLE_STOP &
               T2_PS_1_256 &
               T2_SOURCE_INT, 31250); 

    return RETURN_SUCCESS;
}//powerUpSequence


///////////////////////////////////////////////////////////////////////////////////////////////////
// SYSTEM initialisation
///////////////////////////////////////////////////////////////////////////////////////////////////
int8 systemInit() {
    return RETURN_SUCCESS;
}//systemInit


///////////////////////////////////////////////////////////////////////////////////////////////////
// SYSTEM starting
///////////////////////////////////////////////////////////////////////////////////////////////////
circular_buffer_t buffer;
Time current_time;
Time last_time;
int i = 0;
int8 systemStart() {
    init_buffer(&buffer);
    current_time = 0;
    last_time = 0;
    return RETURN_SUCCESS;
}//systemStart


///////////////////////////////////////////////////////////////////////////////////////////////////
// SYSTEM main loop
///////////////////////////////////////////////////////////////////////////////////////////////////
int8 systemLoop() {
    current_time = getTime();
    if(current_time > last_time + 500000) {
        last_time = current_time;
        push_element(&buffer, i++); // Push an element every .5s
    }
    return RETURN_SUCCESS;
}//systemLoop


///////////////////////////////////////////////////////////////////////////////////////////////////
// SYSTEM shutdown
///////////////////////////////////////////////////////////////////////////////////////////////////
int8 systemShutDown() {
    return RETURN_SUCCESS;
}//systemShutDown


///////////////////////////////////////////////////////////////////////////////////////////////////
// SYSTEM interrupt function called every .8s to test circular buffer
///////////////////////////////////////////////////////////////////////////////////////////////////
void __attribute__((__interrupt__)) _T2Interrupt(void) 
{    
    // Pop an element every second
    int popped = pop_element(&buffer);
    WriteTimer2(0); // Reset timer
    IFS0bits.T2IF = 0; // Clear timer interrupt flag
}//_T2Interrupt

