#include "App_Commu.h"

void App_Commu_Start(void)
{
    Inf_Mobus_Init();
}

void App_Commu_RecvModbusData(void)
{
    Inf_Mobus_AutoRecvAndReply();
}
