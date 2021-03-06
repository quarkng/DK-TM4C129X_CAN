//*****************************************************************************
//
// uart_echo.c - Example for reading data from and writing data to the UART in
//               an interrupt driven fashion.
//
// Copyright (c) 2013 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.0.1.11577 of the DK-TM4C129X Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"
#include "grlib/grlib.h"
#include "drivers/kentec320x240x16_ssd2119.h"
#include "drivers/frame.h"
#include "drivers/pinout.h"

#include "HwAbUart.h"
#include "__CANExample.h"

//*****************************************************************************
void InitGraphicsDisplay(uint32_t ui32SysClock);

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
void UARTIntHandler(void)
{
	HwAbUart_UARTIntHandler();
}

//*****************************************************************************
void CAN0IntHandler(void)
{
	CANExample_ISR();
}

//*****************************************************************************
void CAN1IntHandler(void)
{
}


//*****************************************************************************
int
main(void)
{
    uint32_t ui32SysClock;

    // Run from the PLL at 120 MHz.
    ui32SysClock = SysCtlClockFreqSet(	 (SYSCTL_XTAL_25MHZ |
                                      	  SYSCTL_OSC_MAIN |
                                      	  SYSCTL_USE_PLL |
                                      	  SYSCTL_CFG_VCO_480),
    								120000000);

    // Make sure the main oscillator is enabled.
    // The SYSCTL_MOSC_HIGHFREQ parameter is used when the crystal
    // frequency is 10MHz or higher.
    SysCtlMOSCConfigSet(SYSCTL_MOSC_HIGHFREQ);

    //
    // Initialize the device pinout appropriately for this board.
    //
    PinoutSet();

    InitGraphicsDisplay(ui32SysClock);


    //
    // Enable the (non-GPIO) peripherals used by this example.  PinoutSet()
    // already enabled GPIO Port A.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Enable processor interrupts.
    //
    IntMasterEnable();

    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    ROM_UARTConfigSetExpClk(UART0_BASE, ui32SysClock, 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    //
    // Enable the UART interrupt.
    //
    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT| UART_INT_TX);
    ROM_UARTTxIntModeSet(UART0_BASE, UART_TXINT_MODE_EOT);
    //ROM_UARTIntEnable(UART0_BASE, UART_INT_TX );

    HwAbUart_Init();

    CANExample(ui32SysClock);

    // Loop forever
    while(1)
    {
    }
}

//*****************************************************************************
void InitGraphicsDisplay(uint32_t ui32SysClock)
{
    tContext sContext;

    //
    // Initialize the display driver.
    //
    Kentec320x240x16_SSD2119Init(ui32SysClock);

    //
    // Initialize the graphics context.
    //
    GrContextInit(&sContext, &g_sKentec320x240x16_SSD2119);

    //
    // Draw the application frame.
    //
    FrameDraw(&sContext, "CAN Test");

    //
    // Display UART configuration on the display.
    //
    GrStringDraw(&sContext, "Port:",       -1,  70, 70, 0);
    GrStringDraw(&sContext, "Uart 0",      -1, 150, 70, 0);

    GrStringDraw(&sContext, "Baud:",       -1,  70, 95, 0);
    GrStringDraw(&sContext, "115,200 bps", -1, 150, 95, 0);

    GrStringDraw(&sContext, "Data:",       -1,  70, 120, 0);
    GrStringDraw(&sContext, "8 Bit",       -1, 150, 120, 0);

    GrStringDraw(&sContext, "Parity:",     -1,  70, 145, 0);
    GrStringDraw(&sContext, "None",        -1, 150, 145, 0);

    GrStringDraw(&sContext, "Stop:",       -1,  70, 170, 0);
    GrStringDraw(&sContext, "1 Bit",       -1, 150, 170, 0);
}





void SetLED(bool value)
{   // Drive the user LED at PQ7
	ROM_GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_7, (value)? GPIO_PIN_7 : 0);
}
