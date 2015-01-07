/*
 * __CANExample.c
 *
 *  Created on: Jan 05, 2015
 *      Author: Quark
 */
#include <stdint.h>
#include <stdbool.h>

#include "__CANExample.h"
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/can.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
//#include "driverlib/uart.h"


#define CAN_BASE CAN0_BASE

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
    uint32_t canIntStsCause;

	CANExampleISR_Hits++;

    //
    // Read the CAN interrupt status to find the cause of the interrupt
    //
	canIntStsCause = CANIntStatus(CAN_BASE, CAN_INT_STS_CAUSE);

	if( canIntStsCause == CAN_INT_INTID_STATUS )
	{
		uint32_t canStsControl = CANStatusGet(CAN_BASE, CAN_STS_CONTROL); // also clears status interrupt
	}
	else if( canIntStsCause >= 1 && canIntStsCause <= 32)
	{	// canIntStsCause holds the number of the highest priority message object that has an interrupt pending.

		CANIntClear(CAN0_BASE, 1);


		// CANIntClear()
		// CANMessageGet()

		uint32_t canIntStsObject = CANIntStatus(CAN_BASE, CAN_INT_STS_OBJECT); // bit mask indicating which message objects have pending interrupts
	}


//    CAN_STS_CONTROL - the main controller status
//    CAN_STS_TXREQUEST - bit mask of objects pending transmission
//    CAN_STS_NEWDAT - bit mask of objects with new data
//    CAN_STS_MSGVAL - bit mask of objects with valid configuration



//
//    if( ui32Status & CAN_STS_BOFF ) // Bus-Off Status
//    {
//
//    }
//    if( ui32Status & CAN_STS_EWARN ) // Warning Status
//    {
//
//    }
//    if( ui32Status & CAN_STS_EPASS ) // Error Passive
//    {
//
//    }
//    if( ui32Status & CAN_STS_RXOK ) // Received a Message Successfully
//    {
//
//    }
//    if( ui32Status & CAN_STS_TXOK ) // Transmitted a Message Successfully
//    {
//
//    }
//
//    switch( ui32Status & CAN_STS_LEC_M ) // Last Error Code
//    {
//		case CAN_STS_LEC_STUFF:	// Stuff Error
//			break;
//		case CAN_STS_LEC_FORM:	// Format Error
//			break;
//		case CAN_STS_LEC_ACK:	// ACK Error
//			break;
//		case CAN_STS_LEC_BIT1: 	// Bit 1 Error
//			break;
//		case CAN_STS_LEC_BIT0:	// Bit 0 Error
//			break;
//		case CAN_STS_LEC_CRC:	// CRC Error
//			break;
//		case CAN_STS_LEC_NOEVENT:	// No Event
//			break;
//
//		case CAN_STS_LEC_NONE:
//    	default:
//    		break;
//    }


}

//****************************************************************************************
void Setup( uint32_t ui32SysClock )
{
	tCANBitClkParms CANBitClk;

	//
	// Reset the state of all the message objects and the state of the CAN
	// module to a known state.
	//
	CANInit(CAN_BASE);

	// Configure the controller for 1 Mbit operation.
	//
	CANBitRateSet(CAN_BASE, ui32SysClock, 1000000);

	//
	// Take the CAN0 device out of INIT state.
	//
	CANEnable(CAN_BASE);
}


//****************************************************************************************
void Test( void )
{
//	tCANMsgObject sMsgObjectRx;
//	uint8_t pui8BufferIn[8];

	tCANMsgObject sMsgObjectTx;
	uint8_t pui8BufferOut[8];

	//
	// Configure and start transmit of message object.
	//
//	sMsgObjectTx.ulMsgID = 0x400;
//	sMsgObjectTx.ulFlags = 0;
//	sMsgObjectTx.ulMsgLen = 8;
//	sMsgObjectTx.pucMsgData = pui8BufferOut;
//	CANMessageSet(CAN_BASE, 2, &sMsgObjectTx, MSG_OBJ_TYPE_TX);

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
