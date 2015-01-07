/*
 * __CANExample.c
 *
 *  Created on: Jan 05, 2015
 *      Author: Quark
 */
#include <stdint.h>
#include <stdbool.h>

#include "__CANExample.h"
#include "inc/tm4c129xnczad.h"


int32_t CANExampleISR_Hits = 0;


//****************************************************************************************
static void Setup( uint32_t ui32SysClock );
static void Test( void );

//****************************************************************************************
void CANExample( uint32_t ui32SysClock )
{
	Setup( ui32SysClock );
	Test();
}

//****************************************************************************************
void CANExample_ISR( void )
{
	CANExampleISR_Hits++;

    uint32_t ui32Status;
    //
    // Read the CAN interrupt status to find the cause of the interrupt
    //
//    ui32Status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);
}

//****************************************************************************************
void Setup( uint32_t ui32SysClock )
{

}


//****************************************************************************************
void Test( void )
{

}




#if 0


//========================================
#define ENABLE_DEST_UART
#define ENABLE_DEST_FILE
#define ENABLE_DEST_CONSOLE
//========================================


#ifdef ENABLE_DEST_UART
	#include <TraceDestUart.h>
	#include <HwAbUart.h>
	static int8_t destUartHandle;
#endif
#ifdef ENABLE_DEST_FILE
	#include <TraceDestFile.h>
	static int8_t destFileHandle;
#endif
#ifdef ENABLE_DEST_CONSOLE
	#include <TraceDestConsole.h>
	static int8_t destConsoleHandle;
#endif

//****************************************************************************************
static void SetupLogging( void );
static void TestLogging( void );

//****************************************************************************************
void LogTraceExample( void )
{

    SetupLogging();

    TestLogging();
}

//****************************************************************************************
static void SetupLogging( void )
{
	LogTrace_Init();


#ifdef ENABLE_DEST_UART
	{
		static TraceDestUart_t destUart;
		TraceDestUart_Init( &destUart, &HwAbUart_Send, &HwAbUart_SendString );
		destUartHandle = LogTrace_AddTraceDestination(&destUart.base, LogTraceLevel_VERBOSE);
	}
#endif
#ifdef ENABLE_DEST_FILE
	{
		static TraceDestFile_t destFile;
		TraceDestFile_Init( &destFile, "LogTest.txt");
		destFileHandle = LogTrace_AddTraceDestination(&destFile.base, LogTraceLevel_VERBOSE);
	}
#endif
#ifdef ENABLE_DEST_CONSOLE
	{
		static TraceDestConsole_t destConsole;
		TraceDestConsole_Init( &destConsole );
		destConsoleHandle = LogTrace_AddTraceDestination(&destConsole.base, LogTraceLevel_VERBOSE);
	}
#endif
}

//****************************************************************************************
static void TestLogging( void )
{
	static const char group[] = "TEST_LOG";
	static const char bytes[] = {0xAB, 0xCD, 0xEF};

	LogTrace_Write(LogTraceLevel_INFO,    group, "Test log INFO message" );
	LogTrace_Write(LogTraceLevel_VERBOSE, group, "Test log VERBOSE message" );

	LogTrace_WriteHex(LogTraceLevel_ERROR, group, bytes, sizeof(bytes) );

#ifdef ENABLE_DEST_UART
	LogTrace_SetTraceLevelForDest(destUartHandle, LogTraceLevel_WARNING);
#endif

	LogTrace_Write(LogTraceLevel_INFO,    group, "Blocked?" );
	LogTrace_Write(LogTraceLevel_WARNING, group, "Not Blocked?" );
}


#endif
