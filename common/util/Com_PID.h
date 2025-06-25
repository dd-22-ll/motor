#ifndef __COM_PID_H
#define __COM_PID_H
#include "Common_Debug.h"
typedef struct
{
    float Kp;   // 比例系数
    float Ki;   // 积分系数
    float Kd;   // 微分系数

    float dt;   // 采样周期

    float integral;   // 积分项

    float expect;    // 期望值
    float measure;   // 测量值

    float lastErr;   // 上一次误差

    float result;   // 输出值

} PID_t;

void Com_PID_Calc(PID_t *pid);
void Com_PID_Cascade(PID_t *outer, PID_t *inner);

#endif
