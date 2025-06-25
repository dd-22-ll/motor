#include "Inf_Mobus.h"

void Inf_Mobus_Init(void)
{
    //初始化
    eMBInit(MB_RTU, motor.id, 0, 115200, MB_PAR_NONE);

    //使能modbus
    eMBEnable();
}

void Inf_Mobus_AutoRecvAndReply(void)
{
    //周期性的调用
    eMBPoll();
}

