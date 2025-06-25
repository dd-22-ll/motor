#include "App_MotorControl.h"

static PID_t pid = {.Kp = 2, .Ki = 0.02, .Kd = 0};

Key_func_t      keyFunc = SPEED_CONTROL;
static uint8_t  isHasId = 0;

void App_MotorControl_Start(void)
{
    Inf_Motor_Init();

    /* 上电时,先读取e2prom中的id: 如果读取到,就给motor.id赋值
     * 如果没读取到,则使用默认id: 1 (注：当前代码未实现默认值)
     * 地址0: 存储1 表示id已存储, 存储0: 表示没存储过
     * 地址1: 存储id的值
     */

    Inf_M24C02_ReadByte(0, &isHasId);
    if(isHasId == 1)
    {
        Inf_M24C02_ReadByte(1, &motor.id);
        debug_printfln("读取id: %d", motor.id);
    }
    else
    {
        debug_printfln("没读取到id,使用默认id: 1");
        motor.id = 1;  // 设置默认ID为1
    
        // 可选：将默认ID写入EEPROM，下次启动时就能读取到
        Inf_M24C02_WriteByte(0, 1);  // 标记ID已存储
        Inf_M24C02_WriteByte(1, motor.id);  // 写入默认ID
        isHasId = 1;  // 更新本地标志
    }
}

void App_MotorControl_KeyScan(void)
{
    uint8_t key = Inf_Key_ReadKey();

    if(key == KEY_4) // 按键4用于切换控制方式
    {
        keyFunc = (Key_func_t)((keyFunc + 1) % 3);
        if(keyFunc == SPEED_CONTROL)
        {
            debug_printfln("控制方式: 控制速度");
        }
        else if(keyFunc == ID_CONTROL)
        {
            debug_printfln("控制方式: 设置id");
        }
        else if(keyFunc == MODE_CONTROL)
        {
            debug_printfln("控制方式: 远程or本地");
        }
    }
    //---------------------
    if(keyFunc == SPEED_CONTROL && motor.controlMode == LOCAL)
    {
        if(key == KEY_1)   // 正转反转切换
        {
            motor.runDir *= -1;
            debug_printfln("正转反转切换: %d", motor.runDir);

            motor.expectRPM = 0; // 切换方向后，期望速度清零
        }
        else if(key == KEY_2)   // 减速
        {
            debug_printfln("减速");
            motor.expectRPM -= 20 * motor.runDir;

            if(motor.runDir == 1)
            {
                motor.expectRPM = LIMIT(motor.expectRPM, 0, 280);
            }
            else if(motor.runDir == -1)
            {
                motor.expectRPM = LIMIT(motor.expectRPM, -280, 0);
            }
            debug_printfln("期望转速: %d", motor.expectRPM);
        }
        else if(key == KEY_3)   // 加速
        {
            debug_printfln("加速");
            motor.expectRPM += 20 * motor.runDir;

            if(motor.runDir == 1)
            {
                motor.expectRPM = LIMIT(motor.expectRPM, 0, 280);
            }
            else if(motor.runDir == -1)
            {
                motor.expectRPM = LIMIT(motor.expectRPM, -280, 0);
            }
            debug_printfln("期望转速: %d", motor.expectRPM);
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
            if(isHasId != 1) // 如果之前没有ID, 标记为已有ID
            {
                Inf_M24C02_WriteByte(0, 1);
            }
            Inf_M24C02_WriteByte(1, motor.id); // 写入新ID到EEPROM
        }
        else if(key == KEY_3) // id++
        {
            motor.id++;
            if(motor.id > 247) // Modbus从站地址最大247
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
        /* key2: 本地  key3: 远程 */
        if(key == KEY_2)   // 本地
        {
            debug_printfln("本地");
            motor.controlMode = LOCAL;
        }
        else if(key == KEY_3)   // 远程
        {
            debug_printfln("远程");
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
        pid.integral = 0; /* 电机停止时, 清除pid积分项 */
        pid.result   = 0;
    }

    pid.expect  = motor.expectRPM;
    pid.measure = realRPM;
    pid.dt      = dt;
    Com_PID_Calc(&pid);

    motor.realRPM = realRPM;   // motor的实际转速

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
