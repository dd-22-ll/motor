#include "Com_PID.h"

void Com_PID_Calc(PID_t *pid)
{
    /* 1. ����ƫ��: ����ֵ-����ֵ */
    float err = pid->measure - pid->expect;

    /* 2. ��������� */
    float kv = pid->Kp * err;

    /* 3. ��������� */
    if(pid->result < 500 && pid->result > -500)
    {
        pid->integral += pid->Ki * err * pid->dt;
    }
    

    /* 4. ����΢���� */
    float kd = pid->Kd * (err - pid->lastErr) / pid->dt;

    /* 5. �����ƫ��浽�ϴε�ƫ���Ա�� */
    pid->lastErr = err;

    /* 6. �������ֵ */

    pid->result = kv + pid->integral + kd;
}

void Com_PID_Cascade(PID_t *outer, PID_t *inner)
{
    Com_PID_Calc(outer);
    inner->expect = outer->result; /* �⻷�������Ϊ�ڻ������� */
    Com_PID_Calc(inner);
}
