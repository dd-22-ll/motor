#include "App_Display.h"
#include "App_MotorControl.h"
void App_Display_Start(void)
{
    /* 1. 初始化OLED */
    Inf_OLED_Init();

    /* 2. 显示标题 */
    for(size_t i = 0; i < 7; i++)
    {
        Inf_OLED_ShowChinese(8 + i * 16, 0, i, 16, 1);
    }
    Inf_OLED_Refresh();   // 把缓冲区的数据写到oled
}

static char buff[128];
void        App_Display_Show(void)
{
    if(keyFunc == SPEED_CONTROL)
    {
        memset(buff, 0, sizeof(buff));
        sprintf(buff, "expectRPM = %d          ", motor.expectRPM);
        Inf_OLED_ShowString(0, 18, (uint8_t *)buff, 12, 1);

        memset(buff, 0, sizeof(buff));
        sprintf(buff, "  realRPM = %d          ", motor.realRPM);
        Inf_OLED_ShowString(0, 32, (uint8_t *)buff, 12, 1);
    }
    else if(keyFunc == ID_CONTROL)
    {
        memset(buff, 0, sizeof(buff));
        sprintf(buff, "id = %d                  ", motor.id);
        Inf_OLED_ShowString(0, 18, (uint8_t *)buff, 12, 1);

        memset(buff, 0, sizeof(buff));
        sprintf(buff, "                         ");
        Inf_OLED_ShowString(0, 32, (uint8_t *)buff, 12, 1);
    }
    else if(keyFunc == MODE_CONTROL)
    {
        memset(buff, 0, sizeof(buff));
        sprintf(buff, "mode = %s                  ", motor.controlMode == LOCAL ? "LOCAL" : "REMOTE");
        Inf_OLED_ShowString(0, 18, (uint8_t *)buff, 12, 1);

        memset(buff, 0, sizeof(buff));
        sprintf(buff, "                         ");
        Inf_OLED_ShowString(0, 32, (uint8_t *)buff, 12, 1);
    }

    Inf_OLED_Refresh();
}
