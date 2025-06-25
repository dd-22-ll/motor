#include "Common_Delay.h"

void Delay_ms(uint32_t ms)
{
    uint32_t Delay = ms * 72000 / 9; /* 72M时钟频率，9是PLL倍频 */
    do
    {
        __NOP(); /* 空指令（NOP）来占用 CPU 时间 */
    } while(Delay--);
}
