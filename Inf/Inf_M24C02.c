#include "Inf_M24C02.h"

void Int_M24C02_Init(void)
{
    // 已经在main方法中执行
    // MX_I2C2_Init();
}

void Inf_M24C02_WriteByte(uint8_t byte_addr, uint8_t data)
{
    taskENTER_CRITICAL();
    HAL_I2C_Mem_Write(&hi2c2, M24C02_ADDR_W, byte_addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000);
    taskEXIT_CRITICAL();
    vTaskDelay(10);
}
void Inf_M24C02_WriteBytes(uint8_t byte_addr, uint8_t *data, uint8_t len)
{
    taskENTER_CRITICAL();
    HAL_I2C_Mem_Write(&hi2c2, M24C02_ADDR_W, byte_addr, I2C_MEMADD_SIZE_8BIT, data, len, 1000);
    taskEXIT_CRITICAL();
    vTaskDelay(10);
}

void Inf_M24C02_ReadByte(uint8_t byte_addr, uint8_t *data)
{
    taskENTER_CRITICAL();
    HAL_I2C_Mem_Read(&hi2c2, M24C02_ADDR_R, byte_addr, I2C_MEMADD_SIZE_8BIT, data, 1, 1000);
    taskEXIT_CRITICAL();
}
void Inf_M24C02_ReadBytes(uint8_t byte_addr, uint8_t *data, uint8_t len)
{
    taskENTER_CRITICAL();
    HAL_I2C_Mem_Read(&hi2c2, M24C02_ADDR_R, byte_addr, I2C_MEMADD_SIZE_8BIT, data, len, 1000);
    taskEXIT_CRITICAL();
}
