#include "App_MotorControl.h"

static PID_t pid = {.Kp = 2, .Ki = 0.02, .Kd = 0};

Key_func_t      keyFunc = SPEED_CONTROL;
static uint8_t  isHasId = 0;

void App_MotorControl_Start(void)
{
    Inf_Motor_Init();

    /* �ϵ�ʱ,�ȶ�ȡe2prom�е�id: �����ȡ��,�͸�motor.id��ֵ
     * ���û��ȡ��,��ʹ��Ĭ��id: 1 (ע����ǰ����δʵ��Ĭ��ֵ)
     * ��ַ0: �洢1 ��ʾid�Ѵ洢, �洢0: ��ʾû�洢��
     * ��ַ1: �洢id��ֵ
     */

    Inf_M24C02_ReadByte(0, &isHasId);
    if(isHasId == 1)
    {
        Inf_M24C02_ReadByte(1, &motor.id);
        debug_printfln("��ȡid: %d", motor.id);
    }
    else
    {
        debug_printfln("û��ȡ��id,ʹ��Ĭ��id: 1");
        motor.id = 1;  // ����Ĭ��IDΪ1
    
        // ��ѡ����Ĭ��IDд��EEPROM���´�����ʱ���ܶ�ȡ��
        Inf_M24C02_WriteByte(0, 1);  // ���ID�Ѵ洢
        Inf_M24C02_WriteByte(1, motor.id);  // д��Ĭ��ID
        isHasId = 1;  // ���±��ر�־
    }
}

void App_MotorControl_KeyScan(void)
{
    uint8_t key = Inf_Key_ReadKey();

    if(key == KEY_4) // ����4�����л����Ʒ�ʽ
    {
        keyFunc = (Key_func_t)((keyFunc + 1) % 3);
        if(keyFunc == SPEED_CONTROL)
        {
            debug_printfln("���Ʒ�ʽ: �����ٶ�");
        }
        else if(keyFunc == ID_CONTROL)
        {
            debug_printfln("���Ʒ�ʽ: ����id");
        }
        else if(keyFunc == MODE_CONTROL)
        {
            debug_printfln("���Ʒ�ʽ: Զ��or����");
        }
    }
    //---------------------
    if(keyFunc == SPEED_CONTROL && motor.controlMode == LOCAL)
    {
        if(key == KEY_1)   // ��ת��ת�л�
        {
            motor.runDir *= -1;
            debug_printfln("��ת��ת�л�: %d", motor.runDir);

            motor.expectRPM = 0; // �л�����������ٶ�����
        }
        else if(key == KEY_2)   // ����
        {
            debug_printfln("����");
            motor.expectRPM -= 20 * motor.runDir;

            if(motor.runDir == 1)
            {
                motor.expectRPM = LIMIT(motor.expectRPM, 0, 280);
            }
            else if(motor.runDir == -1)
            {
                motor.expectRPM = LIMIT(motor.expectRPM, -280, 0);
            }
            debug_printfln("����ת��: %d", motor.expectRPM);
        }
        else if(key == KEY_3)   // ����
        {
            debug_printfln("����");
            motor.expectRPM += 20 * motor.runDir;

            if(motor.runDir == 1)
            {
                motor.expectRPM = LIMIT(motor.expectRPM, 0, 280);
            }
            else if(motor.runDir == -1)
            {
                motor.expectRPM = LIMIT(motor.expectRPM, -280, 0);
            }
            debug_printfln("����ת��: %d", motor.expectRPM);
        }
    }
    else if(keyFunc == ID_CONTROL)
    {
        /* key2: id--  key3:id++ */
        if(key == KEY_2)   // id--
        {
            motor.id--;

            if(motor.id == 0)
            {
                motor.id = 1;
            }
            if(isHasId != 1) // ���֮ǰû��ID, ���Ϊ����ID
            {
                Inf_M24C02_WriteByte(0, 1);
            }
            Inf_M24C02_WriteByte(1, motor.id); // д����ID��EEPROM
        }
        else if(key == KEY_3) // id++
        {
            motor.id++;
            if(motor.id > 247) // Modbus��վ��ַ���247
            {
                motor.id = 247;
            }
            if(isHasId != 1)
            {
                Inf_M24C02_WriteByte(0, 1);
            }
            Inf_M24C02_WriteByte(1, motor.id);
        }
    }
    else if(keyFunc == MODE_CONTROL)
    {
        /* key2: ����  key3: Զ�� */
        if(key == KEY_2)   // ����
        {
            debug_printfln("����");
            motor.controlMode = LOCAL;
        }
        else if(key == KEY_3)   // Զ��
        {
            debug_printfln("Զ��");
            motor.controlMode = REMOTE;
        }
    }
}

int16_t App_MotorControl_GetRealRPM(float dt)
{
    return Inf_Motor_GetRPM(dt);
}

void App_MotorControl_PIDCacl(int16_t realRPM, float dt)
{
    if(motor.controlMode == REMOTE)
    {
        motor.expectRPM = REG_HOLD_BUF[0];
    }
    if(motor.expectRPM == 0)
    {
        pid.integral = 0; /* ���ֹͣʱ, ���pid������ */
        pid.result   = 0;
    }

    pid.expect  = motor.expectRPM;
    pid.measure = realRPM;
    pid.dt      = dt;
    Com_PID_Calc(&pid);

    motor.realRPM = realRPM;   // motor��ʵ��ת��

    motor.speed = (int16_t)(LIMIT(pid.result, -500, 500));

    /* printf("pid.expect: %d, pid.measure: %d, motor.speed: %d, pid.result = %.1f pid.integral = %.1f\n",
           motor.expectRPM,
           motor.realRPM,
           motor.speed,
           pid.result,
           pid.integral); */
}

void App_MotorControl_MotorRun(void)
{
    // motor.speed = (int16_t)(motor.expectRPM / 280.0 * 500);
    if(motor.speed == 0)
    {
        Inf_Motor_Stop();
    }
    else
    {
        Inf_Motor_Start();
    }

    Inf_Motor_SetSpeed(motor.speed);
}
