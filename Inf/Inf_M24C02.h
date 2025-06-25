#ifndef __INT_M24C02__
#define __INT_M24C02__

#include "i2c.h"
#include "Common_Debug.h"
#include "FreeRTOS.h"
#include "task.h"

#define M24C02_ADDR 0xA0
#define M24C02_ADDR_W 0xA0
#define M24C02_ADDR_R 0xA1

void Inf_M24C02_Init(void);
void Inf_M24C02_WriteByte(uint8_t byte_addr, uint8_t data);
void Inf_M24C02_WriteBytes(uint8_t byte_addr, uint8_t *data, uint8_t len);
void Inf_M24C02_ReadByte(uint8_t byte_addr, uint8_t *data);
void Inf_M24C02_ReadBytes(uint8_t byte_addr, uint8_t *data, uint8_t len);

#endif // __INT_M24C02__
