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
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#define REG32(x)  (*((volatile uint32_t *) (x) ))

#define CAN_BASE            CAN0_BASE
#define SYSCTL_PERIPH_CAN   SYSCTL_PERIPH_CAN0

int32_t CANExampleISR_Hits = 0;
bool    AllTransmitDone = true;

struct receivedData
{
	tCANMsgObject msgObj;
	uint8_t data[8];
} rxData;


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
uint32_t cause=0;
uint32_t stCtrl = 0;
uint32_t stNewdata = 0;
uint32_t stTxReq = 0;
uint32_t stObj = 0;

void CANExample_ISR( void )
{
    uint32_t canIntStsCause;

	CANExampleISR_Hits++;

    //
    // Read the CAN interrupt status to find the cause of the interrupt
    //
	canIntStsCause = CANIntStatus(CAN_BASE, CAN_INT_STS_CAUSE);
	cause = canIntStsCause;

	if( canIntStsCause == CAN_INT_INTID_STATUS )
	{
		uint32_t canStsControl = CANStatusGet(CAN_BASE, CAN_STS_CONTROL); // also clears status interrupt
		stCtrl = canStsControl;

		if( canStsControl & CAN_STATUS_BUS_OFF ) // controller is in bus-off condition
		{

		}
		if( canStsControl & CAN_STATUS_EWARN ) // an error counter has reached a limit of at least 96
		{

		}
		if( canStsControl & CAN_STATUS_EPASS ) // CAN controller is in the error passive state
		{

		}
		if( canStsControl & CAN_STS_RXOK ) // a message was received successfully
		{
			uint32_t canStsNewdata = CANStatusGet(CAN_BASE, CAN_STS_NEWDAT);
			stNewdata = canStsNewdata;

		}
		if( canStsControl & CAN_STS_TXOK ) // a message was successfully transmitted
		{
			uint32_t canStsTxRequest = CANStatusGet(CAN_BASE, CAN_STS_TXREQUEST);
			stTxReq = canStsTxRequest;

			AllTransmitDone = (canStsTxRequest == 0);
		}
		switch( canStsControl & CAN_STATUS_LEC_MSK ) // Last Error Code
		{
			case CAN_STATUS_LEC_STUFF:	// stuffing error detected
				break;
			case CAN_STATUS_LEC_FORM:	// a format error occurred in the fixed format part of a message
				break;
			case CAN_STATUS_LEC_ACK:	// a transmitted message was not acknowledged
				break;
			case CAN_STATUS_LEC_BIT1: 	// dominant level detected when trying to send in recessive mode
				break;
			case CAN_STATUS_LEC_BIT0:	// recessive level detected when trying to send in dominant mode
				break;
			case CAN_STATUS_LEC_CRC:	// CRC error in received message
				break;
			case CAN_STATUS_LEC_NONE:	// no error
			default:
				break;
		}
	}
	else if( canIntStsCause >= 1 && canIntStsCause <= 32)
	{	// canIntStsCause holds the number of the highest priority message object that has an interrupt pending.
		uint32_t canIntStsObject = CANIntStatus(CAN_BASE, CAN_INT_STS_OBJECT); // bit mask indicating which message objects have pending interrupts
		stObj = canIntStsObject;


		switch(canIntStsCause)
		{
			case 1:
				rxData.data[0] = 0;
				rxData.msgObj.pui8MsgData = rxData.data;
				CANMessageGet(CAN_BASE,	canIntStsCause,	&(rxData.msgObj), true);
				break;
			default:
				CANIntClear(CAN_BASE, canIntStsCause);
				break;
		}
	}
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
extern void SetLED(bool value);

void Test( void )
{
	bool ledState = false;
	tCANMsgObject sMsgObjectRx;

	tCANMsgObject sMsgObjectTx;
	uint8_t pui8BufferOut[8];


	// Configure receiver object
	sMsgObjectRx.ui32MsgID = 0x400;
	sMsgObjectRx.ui32MsgIDMask = 0;
	sMsgObjectRx.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
	sMsgObjectRx.ui32MsgLen = 8;
	sMsgObjectRx.pui8MsgData = 0;
	CANMessageSet(CAN_BASE, 1, &sMsgObjectRx, MSG_OBJ_TYPE_RX);


	AllTransmitDone = true;
	SetLED(ledState);
	pui8BufferOut[0] = 0;

    // Send the CAN message using object number 1 (not the same thing as
    // CAN ID, which is 0x400 in this example).  This function will cause
    // the message to be transmitted right away.
	for(;;)
	{
		if( AllTransmitDone )
		{
			AllTransmitDone = false;

			// Configure and start transmit of message object.
			sMsgObjectTx.ui32MsgID = 0x400;
			sMsgObjectTx.ui32MsgIDMask = 0;
			sMsgObjectTx.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
			sMsgObjectTx.ui32MsgLen = 8;
			sMsgObjectTx.pui8MsgData = pui8BufferOut;
			pui8BufferOut[0]++;
			CANMessageSet(CAN_BASE, 3, &sMsgObjectTx, MSG_OBJ_TYPE_TX);

			ledState = ! ledState;
			SetLED(ledState);
		}
	}

}

