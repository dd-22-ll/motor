#include "Common_Delay.h"

void Delay_ms(uint32_t ms)
{
    uint32_t Delay = ms * 72000 / 9; /* 72Mʱ��Ƶ�ʣ�9��PLL��Ƶ */
    do
    {
        __NOP(); /* ��ָ�NOP����ռ�� CPU ʱ�� */
    } while(Delay--);
}
