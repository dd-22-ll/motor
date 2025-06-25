#ifndef __COM_PID_H
#define __COM_PID_H
#include "Common_Debug.h"
typedef struct
{
    float Kp;   // ����ϵ��
    float Ki;   // ����ϵ��
    float Kd;   // ΢��ϵ��

    float dt;   // ��������

    float integral;   // ������

    float expect;    // ����ֵ
    float measure;   // ����ֵ

    float lastErr;   // ��һ�����

    float result;   // ���ֵ

} PID_t;

void Com_PID_Calc(PID_t *pid);
void Com_PID_Cascade(PID_t *outer, PID_t *inner);

#endif
