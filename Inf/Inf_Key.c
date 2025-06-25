#include "Inf_Key.h"

#define READ_KEY_1 HAL_GPIO_ReadPin(KEY_1_GPIO_Port, KEY_1_Pin)
#define READ_KEY_2 HAL_GPIO_ReadPin(KEY_2_GPIO_Port, KEY_2_Pin)
#define READ_KEY_3 HAL_GPIO_ReadPin(KEY_3_GPIO_Port, KEY_3_Pin)
#define READ_KEY_4 HAL_GPIO_ReadPin(KEY_4_GPIO_Port, KEY_4_Pin)

Key_t Inf_Key_ReadKey(void)
{
    if(READ_KEY_1 == 0 || READ_KEY_2 == 0 || READ_KEY_3 == 0 || READ_KEY_4 == 0)
    {
        vTaskDelay(20);
        if(READ_KEY_1 == 0)
        {
            while(READ_KEY_1 == 0);
            return KEY_1;
        }
        else if(READ_KEY_2 == 0)
        {
            while(READ_KEY_2 == 0);
            return KEY_2;
        }
        else if(READ_KEY_3 == 0)
        {
            while(READ_KEY_3 == 0);
            return KEY_3;
        }
        else if(READ_KEY_4 == 0)
        {
            while(READ_KEY_4 == 0);
            return KEY_4;
        }
    }
    return KEY_NONE;
}
