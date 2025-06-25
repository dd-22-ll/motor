#ifndef __APP_MOTORCONTROL_H
#define __APP_MOTORCONTROL_H
#include "Common_Debug.h"
#include "Inf_Key.h"
#include "Inf_Motor.h"
#include "Com_PID.h"
#include "Com_Define.h"
#include "Inf_M24C02.h"
#include "port.h"
typedef enum
{
    SPEED_CONTROL = 0,
    ID_CONTROL,
    MODE_CONTROL,
} Key_func_t;
extern Key_func_t keyFunc;

void App_MotorControl_Start(void);

void App_MotorControl_KeyScan(void);

void    App_MotorControl_MotorRun(void);
int16_t App_MotorControl_GetRealRPM(float dt);
void    App_MotorControl_PIDCacl(int16_t realRPM, float dt);
#endif
