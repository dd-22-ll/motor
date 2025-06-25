#ifndef __INF_MOTOR_H
#define __INF_MOTOR_H
#include "Common_Debug.h"
#include "tim.h"
#include "gpio.h"
#include "Com_Define.h"

typedef enum
{
    FORWARD  = 1,    // 正转
    BACKWARD = -1,   // 反转
} Dir_t;

typedef enum
{
    LOCAL  = 0,   // 本地控制
    REMOTE = 1,   // 远程控制
} Contorl_Mode_t;

typedef struct
{
    TIM_HandleTypeDef *htim;
    uint32_t           channel;

    Dir_t runDir; /* 转动方向: 1正转 -1反转 */

    int16_t speed; /* 设置占空比: [-500 +500] */

    int16_t expectRPM; /* 期望转速: [-280, 280] */
    int16_t realRPM;   /* 实际转速: [-280, 280] */

    uint8_t id; /* 电机编号  modbus通讯的时的地址*/

    Contorl_Mode_t controlMode; /* 控制模式: 0本地控制 1远程控制 */

} Motor_t;

extern Motor_t motor;

void Inf_Motor_Init(void);

void Inf_Motor_Start(void);

void Inf_Motor_Stop(void);

void Inf_Motor_SetSpeed(int16_t speed);

int16_t Inf_Motor_GetRPM(float dt);

#endif
