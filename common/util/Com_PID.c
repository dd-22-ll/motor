#include "Com_PID.h"

void Com_PID_Calc(PID_t *pid)
{
    /* 1. 计算偏差: 测量值-期望值 */
    float err = pid->measure - pid->expect;

    /* 2. 计算比例项 */
    float kv = pid->Kp * err;

    /* 3. 计算积分项 */
    if(pid->result < 500 && pid->result > -500)
    {
        pid->integral += pid->Ki * err * pid->dt;
    }
    

    /* 4. 计算微分项 */
    float kd = pid->Kd * (err - pid->lastErr) / pid->dt;

    /* 5. 把这次偏差保存到上次的偏差成员中 */
    pid->lastErr = err;

    /* 6. 计算输出值 */

    pid->result = kv + pid->integral + kd;
}

void Com_PID_Cascade(PID_t *outer, PID_t *inner)
{
    Com_PID_Calc(outer);
    inner->expect = outer->result; /* 外环的输出作为内环的输入 */
    Com_PID_Calc(inner);
}
