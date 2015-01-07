/*
 * __CANExample.c
 *
 *  Created on: Jan 05, 2015
 *      Author: Quark
 */
#include <stdint.h>
#include <stdbool.h>

#include "__CANExample.h"
#include "inc/hw_ints.h"
#include "inc/hw_can.h"
#include "inc/hw_memmap.h"
#include "driverlib/can.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
//#include "driverlib/uart.h"

#define REG32(x)  (*((volatile uint32_t *) (x) ))
#define CAN_BASE            CAN0_BASE
#define SYSCTL_PERIPH_CAN   SYSCTL_PERIPH_CAN0

int32_t CANExampleISR_Hits = 0;

//****************************************************************************************
static void Setup( uint32_t ui32SysClock );
static void SetAsLoopback( void );
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
//	tCANBitClkParms CANBitClk;

	// Configure GPIO for CAN
	GPIOPinConfigure(GPIO_PT0_CAN0RX);
	GPIOPinConfigure(GPIO_PT1_CAN0TX);

	GPIOPinTypeCAN(GPIO_PORTT_BASE,	GPIO_PIN_0 | GPIO_PIN_1 );

    // The CAN peripheral must be enabled.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN);

	// Reset the state of all the message objects and the state of the CAN
	// module to a known state.
	CANInit(CAN_BASE);

	// Configure the controller for 1 Mbit operation.
	//
	CANBitRateSet(CAN_BASE, ui32SysClock, 1000000);

	// Enable interrupts on the CAN peripheral
	CANIntEnable(CAN_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

    // Enable the CAN interrupt on the processor (NVIC).
#if CAN_BASE == CAN0_BASE
    IntEnable(INT_CAN0);
#elif CAN_BASE == CAN1_BASE
    IntEnable(INT_CAN1);
#else
	#error Bad CAN_BASE
#endif

	// Take the CAN device out of INIT state.
	CANEnable(CAN_BASE);

	SetAsLoopback();
}

//****************************************************************************************
void SetAsLoopback( void )
{
	REG32( CAN_BASE + CAN_O_CTL ) |= CAN_CTL_TEST;
	REG32( CAN_BASE + CAN_O_TST ) |= CAN_TST_LBACK;
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
	sMsgObjectTx.ui32MsgID = 0x400;
	sMsgObjectTx.ui32MsgIDMask = 0;
	sMsgObjectTx.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
	sMsgObjectTx.ui32MsgLen = 8;
	sMsgObjectTx.pui8MsgData = pui8BufferOut;

    // Send the CAN message using object number 1 (not the same thing as
    // CAN ID, which is 0x400 in this example).  This function will cause
    // the message to be transmitted right away.
	CANMessageSet(CAN_BASE, 1, &sMsgObjectTx, MSG_OBJ_TYPE_TX);
}

