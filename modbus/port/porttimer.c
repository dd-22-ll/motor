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

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR( void );

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    //为了防止中断一启用直接进入一次中断, 需要先清除中断标记位
    __HAL_TIM_CLEAR_FLAG(&MB_TIMER, TIM_FLAG_UPDATE);
    //启用定时器中断
    __HAL_TIM_ENABLE_IT(&MB_TIMER, TIM_IT_UPDATE);
    return TRUE;
}


inline void
vMBPortTimersEnable(  )
{
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
    __HAL_TIM_SetCounter(&MB_TIMER, 0);
    HAL_TIM_Base_Start_IT(&MB_TIMER);
}

inline void
vMBPortTimersDisable(  )
{
    /* Disable any pending timers. */
    HAL_TIM_Base_Stop_IT(&MB_TIMER);
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
static void prvvTIMERExpiredISR( void )
{
    ( void )pxMBPortCBTimerExpired(  );
}

void TIM3_IRQHandler(void)
{
    if(__HAL_TIM_GET_FLAG(&MB_TIMER, TIM_FLAG_UPDATE) != RESET)
    {
        __HAL_TIM_CLEAR_FLAG(&MB_TIMER, TIM_FLAG_UPDATE);
        prvvTIMERExpiredISR();
    }
    prvvTIMERExpiredISR(); 
}
