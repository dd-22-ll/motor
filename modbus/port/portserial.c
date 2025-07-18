/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "stdio.h"
/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
    if( xRxEnable == TRUE )
    {
        __HAL_UART_ENABLE_IT(&MB_UART, UART_IT_RXNE);
    }
    else
    {
        __HAL_UART_DISABLE_IT(&MB_UART, UART_IT_RXNE);
    }

    if( xTxEnable == TRUE )
    {
        __HAL_UART_ENABLE_IT(&MB_UART, UART_IT_TXE);
    }
    else
    {
        __HAL_UART_DISABLE_IT(&MB_UART, UART_IT_TXE);
    }
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    //HAL_UART_Transmit, 提高效率, 直接操作寄存器
    MB_UART.Instance->DR = ucByte;
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    *pucByte = (CHAR)(MB_UART.Instance->DR & 0xff);
    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}

//实现串口2的中断服务函数
void USART2_IRQHandler(void)
{
    //接收非空
    if(__HAL_UART_GET_FLAG(&MB_UART, UART_FLAG_RXNE) != RESET)
    {
        prvvUARTRxISR();
        __HAL_UART_CLEAR_FLAG(&MB_UART, UART_FLAG_RXNE);

    }
    else if(__HAL_UART_GET_FLAG(&MB_UART, UART_FLAG_TXE) != RESET)
    {
        prvvUARTTxReadyISR();
        __HAL_UART_CLEAR_FLAG(&MB_UART, UART_FLAG_TXE);
    }
}
