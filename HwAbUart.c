/*
 * HwAbUart.c
 *
 *  Created on: Dec 11, 2014
 *      Author: Quark
 */

#include "HwAbUart.h"

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"


typedef struct
{
	char *buf;
	uint16_t size;
	uint16_t head;
	uint16_t tail;
} SingleBuffer_t;

//static
struct
{
	SingleBuffer_t tx;
	SingleBuffer_t rx;
} buffer;



static void ForceFirstByteTransmission( void );
static bool PushToBuffer( SingleBuffer_t *b, char c );
static char GetFromBuffer( SingleBuffer_t *b, bool *isValid );





//*****************************************************************************
//
// Initialize abstraction code.  Must be called before any use of other code
// in this module.
//
//*****************************************************************************
void HwAbUart_Init(void)
{
	static char txBuf[HwAbUart_TX_BUFFER_SIZE];
	static char rxBuf[HwAbUart_RX_BUFFER_SIZE];

	buffer.tx.buf = txBuf;
	buffer.tx.size = HwAbUart_TX_BUFFER_SIZE;
	buffer.tx.head = 0;
	buffer.tx.tail = 0;

	buffer.rx.buf = rxBuf;
	buffer.rx.size = HwAbUart_RX_BUFFER_SIZE;
	buffer.rx.head = 0;
	buffer.rx.tail = 0;
}

//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
void HwAbUart_UARTIntHandler(void)
{
    uint32_t ui32Status;
    bool valid;

    //
    // Get the interrrupt status.
    //
    ui32Status = ROM_UARTIntStatus(UART0_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    ROM_UARTIntClear(UART0_BASE, ui32Status);



    // Check for any bytes to receive
    valid = true;
    while(ROM_UARTCharsAvail(UART0_BASE) && valid)
    {
    	valid = PushToBuffer( &(buffer.rx), (char) UARTCharGetNonBlocking(UART0_BASE) );
    }

    // Check for any bytes to transmit
     while( ROM_UARTSpaceAvail(UART0_BASE) )
    {
		char c = GetFromBuffer( &(buffer.tx), &valid );
    	if( valid )
    	{
    		ROM_UARTCharPutNonBlocking(UART0_BASE, c);
    	}
    	else
    	{
    		break; // buffer was empty
    	}
    }
}


//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
void HwAbUart_Send(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    while(ui32Count--)
    {
    	PushToBuffer( &(buffer.tx), *pui8Buffer++ );
    }

    ForceFirstByteTransmission();
}



void HwAbUart_SendString(const char *pcBuffer)
{
	int i = 0;

	while( ( *pcBuffer != 0 ) && (i < HwAbUart_MAX_ALLOWED_STR_LEN) )
	{
		PushToBuffer( &(buffer.tx), *pcBuffer++ );
		i++;
	}

    ForceFirstByteTransmission();
}


static void ForceFirstByteTransmission( void )
{
	if( ROM_UARTBusy(UART0_BASE) == false )
	{	// force first byte transmission
		bool valid;
		char c = GetFromBuffer( &(buffer.tx), &valid );
		if( valid )
		{
			ROM_UARTCharPutNonBlocking(UART0_BASE, c);
		}
	}
}







static bool PushToBuffer( SingleBuffer_t *b, char c )
{
	bool overflow = false;
	uint16_t nextTail = b->tail + 1;
	if( nextTail >= b->size )
	{
		nextTail = 0;
	}


	if( nextTail == b->head )
	{	// Buffer is full.  Overwrite last character.
		nextTail = b->tail;
		overflow = true;
	}

	b->buf[ b->tail ] = c;
	b->tail = nextTail;

	return overflow;
}

static char GetFromBuffer( SingleBuffer_t *b, bool *isValid )
{
	char retVal = 0;
	bool valid = false;

	if( b->head != b->tail )
	{
		uint16_t nextHead;

		valid = true;
		retVal = b->buf[b->head];
		nextHead = (b->head + 1);
		if( nextHead >= b->size )
		{
			nextHead = 0;
		}
		b->head = nextHead;
	}

	if( isValid != 0 )
	{
		*isValid = valid;
	}
	return retVal;
}





