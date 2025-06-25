#include "port.h"
#include "mb.h"
#include "mbport.h"
#include "Common_Debug.h"

// 声明输入寄存器缓冲区，用于存储十路输入寄存器的值
uint16_t REG_INPUT_BUF[REG_INPUT_SIZE];

// 声明保持寄存器缓冲区，用于存储十路保持寄存器的值
uint16_t REG_HOLD_BUF[REG_HOLD_SIZE];

// 定义十路线圈的大小
uint8_t REG_COILS_BUF[REG_COILS_SIZE] = {1, 1, 1, 1, 0, 0, 0, 0, 1, 1};

// 声明离散量缓冲区，并初始化，用于存储十路离散量的状态
uint8_t REG_DISC_BUF[REG_DISC_SIZE] = {1,1,1,1,0,0,0,0,1,1};

/**
 * @brief CMD4命令处理回调函数
 * 
 * 该函数用于处理MODBUS协议中的CMD4命令，即读取输入寄存器。
 * 它将指定地址范围内的输入寄存器的值复制到缓冲区中。
 *
 * @param pucRegBuffer 指向用于存储寄存器值的缓冲区的指针。
 * @param usAddress 要读取的起始寄存器地址。
 * @param usNRegs 要读取的寄存器数量。
 *
 * @return 返回执行结果的错误代码。
 */
eMBErrorCode eMBRegInputCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
    // 计算寄存器索引，从0开始
    USHORT usRegIndex = usAddress - 1;

    // 非法检测：检查访问范围是否超出寄存器缓冲区大小
    if ((usRegIndex + usNRegs) > REG_INPUT_SIZE)
    {
        return MB_ENOREG;
    }

    // 循环读取寄存器值并写入缓冲区
    while (usNRegs > 0)
    {
        // 将寄存器的高8位写入缓冲区
        *pucRegBuffer++ = (unsigned char)(REG_INPUT_BUF[usRegIndex] >> 8);
        // 将寄存器的低8位写入缓冲区
        *pucRegBuffer++ = (unsigned char)(REG_INPUT_BUF[usRegIndex] & 0xFF);
        usRegIndex++;
        usNRegs--;
    }

    return MB_ENOERR;
}

/**
 * @brief CMD6、3、16命令处理回调函数
 * 
 * 该函数用于处理Modbus协议中的 Holding Registers 读写请求。
 * 它根据请求的模式（读或写）对指定的寄存器进行相应的操作。
 *
 * @param pucRegBuffer 寄存器数据缓冲区，用于读取或写入寄存器数据。
 * @param usAddress 请求访问的起始寄存器地址。
 * @param usNRegs 请求访问的寄存器数量。
 * @param eMode 访问模式，可以是 MB_REG_WRITE（写寄存器）或 MB_REG_READ（读寄存器）。
 *
 * @return 返回执行结果，如果成功则返回 MB_ENOERR，否则返回相应的错误代码。
 */
eMBErrorCode eMBRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode)
{
    // 计算寄存器索引，Modbus 地址从 1 开始，数组索引从 0 开始，因此需要减 1。
    USHORT usRegIndex = usAddress - 1;

    // 非法检测：检查访问范围是否超出寄存器缓冲区大小。
    if ((usRegIndex + usNRegs) > REG_HOLD_SIZE)
    {
        return MB_ENOREG;
    }

    // 写寄存器
    if (eMode == MB_REG_WRITE)
    {
        // 循环将每个寄存器的数据从缓冲区写入到寄存器中。
        while (usNRegs > 0)
        {
            REG_HOLD_BUF[usRegIndex] = (pucRegBuffer[0] << 8) | pucRegBuffer[1];
            pucRegBuffer += 2;
            usRegIndex++;
            usNRegs--;
        }
        //调试信息
        debug_printfln("eMB RegHoldingCB write: %d %d", usAddress, (int16_t)REG_HOLD_BUF[usAddress-1]);
    }
    // 读寄存器
    else
    {
        // 循环将每个寄存器的数据从寄存器中读取到缓冲区。
        while (usNRegs > 0)
        {
            *pucRegBuffer++ = (unsigned char)(REG_HOLD_BUF[usRegIndex] >> 8);
            *pucRegBuffer++ = (unsigned char)(REG_HOLD_BUF[usRegIndex] & 0xFF);
            usRegIndex++;
            usNRegs--;
        }
    }

    return MB_ENOERR;
}

/**
 * @brief CMD1、5、15命令处理回调函数
 * 
 * 该函数用于处理Modbus协议中的CMD1、CMD5和CMD15命令。
 * 它主要负责读取或写入寄存器中的位数据。
 *
 * @param pucRegBuffer 指向寄存器缓冲区的指针，用于读取或写入数据。
 * @param usAddress 要操作的寄存器起始地址。
 * @param usNCoils 要操作的位数。
 * @param eMode 操作模式，可以是读或写。
 *
 * @return 返回操作结果，如果成功则返回MB_ENOERR，否则返回相应的错误代码。
 */
eMBErrorCode eMBRegCoilsCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode)
{
    // 计算寄存器索引
    USHORT usRegIndex = usAddress - 1;
    // 用于位操作的变量
    UCHAR ucBits = 0;
    // 用于存储位状态的变量
    UCHAR ucState = 0;
    // 用于循环操作的变量
    UCHAR ucLoops = 0;

    // 非法检测：检查访问范围是否超出寄存器缓冲区大小。
    if ((usRegIndex + usNCoils) > REG_COILS_SIZE)
    {
        return MB_ENOREG;
    }

    // 根据操作模式执行相应的操作
    if (eMode == MB_REG_WRITE)
    {
        // 计算需要循环的次数
        ucLoops = (usNCoils - 1) / 8 + 1;
        // 写操作
        while (ucLoops != 0)
        {
            // 获取当前寄存器的状态
            ucState = *pucRegBuffer++;
            // 位操作
            ucBits = 0;
            // 遍历每个位
            while (usNCoils != 0 && ucBits < 8)
            {
                // 将状态写入寄存器缓冲区
                REG_COILS_BUF[usRegIndex++] = (ucState >> ucBits) & 0X01;
                // 更新剩余位数
                usNCoils--;
                // 更新位索引
                ucBits++;
            }
            // 更新循环次数
            ucLoops--;
        }
    }
    else
    {
        // 计算需要循环的次数
        ucLoops = (usNCoils - 1) / 8 + 1;
        // 读操作
        while (ucLoops != 0)
        {
            // 初始化状态变量
            ucState = 0;
            // 位操作
            ucBits = 0;
            // 遍历每个位
            while (usNCoils != 0 && ucBits < 8)
            {
                // 根据寄存器缓冲区的状态更新状态变量
                if (REG_COILS_BUF[usRegIndex])
                {
                    ucState |= (1 << ucBits);
                }
                // 更新剩余位数
                usNCoils--;
                // 更新寄存器索引
                usRegIndex++;
                // 更新位索引
                ucBits++;
            }
            // 将状态写入寄存器缓冲区
            *pucRegBuffer++ = ucState;
            // 更新循环次数
            ucLoops--;
        }
    }

    return MB_ENOERR;
}

/**
 * @brief CMD2命令处理回调函数
 * 
 * 该函数用于处理MODBUS协议中的CMD2命令，主要负责读取离散输入寄存器的值。
 *
 * @param pucRegBuffer 指向存放寄存器数据的缓冲区。
 * @param usAddress 寄存器的起始地址。
 * @param usNDiscrete 要读取的离散输入寄存器的数量。
 *
 * @return 返回执行结果，如果成功则返回MB_ENOERR，否则返回相应的错误代码。
 */
eMBErrorCode eMBRegDiscreteCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNDiscrete)
{
    // 计算寄存器索引，从0开始
    USHORT usRegIndex = usAddress - 1;
    // 用于处理位操作的变量
    UCHAR ucBits = 0;
    // 用于存储当前寄存器状态的变量
    UCHAR ucState = 0;
    // 用于控制循环次数的变量
    UCHAR ucLoops = 0;

    // 非法检测：检查访问范围是否超出寄存器缓冲区大小
    if ((usRegIndex + usNDiscrete) > REG_DISC_SIZE)
    {
        return MB_ENOREG;
    }

    // 计算需要循环的次数，每次循环处理最多8个离散输入
    ucLoops = (usNDiscrete - 1) / 8 + 1;
    // 循环读取离散输入寄存器的值
    while (ucLoops != 0)
    {
        ucState = 0;
        ucBits = 0;
        // 读取每个寄存器的值，并将其状态更新到ucState变量中
        while (usNDiscrete != 0 && ucBits < 8)
        {
            if (REG_DISC_BUF[usRegIndex])
            {
                ucState |= (1 << ucBits);
            }
            usNDiscrete--;
            usRegIndex++;
            ucBits++;
        }
        // 将读取到的状态值存入缓冲区中
        *pucRegBuffer++ = ucState;
        ucLoops--;
    }

    // 模拟离散量输入被改变，这里简单地将每个寄存器的状态取反
    for (usRegIndex = 0; usRegIndex < REG_DISC_SIZE; usRegIndex++)
    {
        REG_DISC_BUF[usRegIndex] = !REG_DISC_BUF[usRegIndex];
    }

    return MB_ENOERR;
}

/**
 * @brief 将 Modbus 库错误码映射为 Modbus 异常码
 * 
 * @param error Modbus 库错误码（eMBErrorCode）
 * @return 对应的 Modbus 异常码
 */
uint8_t mapErrorToException(eMBErrorCode error)
{
    switch (error)
    {
        case MB_ENOREG:     // 非法寄存器地址
            return 0x02;    // 非法数据地址
        case MB_EINVAL:     // 非法参数
            return 0x03;    // 非法数据值
        case MB_ENORES:     // 资源不足
            return 0x04;    // 从站设备故障
        case MB_ETIMEDOUT:  // 超时
            return 0x06;    // 从站设备忙
        case MB_EPORTERR:   // 端口错误
            return 0x04;    // 从站设备故障
        case MB_ENOERR:     // 无错误
            return 0x00;    // 无异常
        default:            // 未知错误
            return 0x04;    // 从站设备故障
    }
}

/**
  * @brief  当 assert() 宏的条件为假时，此函数被调用。
  * @param  expr: 指向断言表达式字符串的指针。
  * @param  file: 指向源文件名的指针。
  * @param  line: 断言失败所在的行号。
  * @retval None
  */
void __aeabi_assert(const char *expr, const char *file, int line)
{
    // 1. 使用你的调试打印函数，将详细的错误信息通过串口打印出来
    //    这对于定位问题至关重要！
    debug_printfln("Assertion Failed: %s, file %s, line %d\n", expr, file, line);
    // 如果您使用了自己的debug_printfln，也可以用它来打印

    // 2. 进入一个死循环，挂起整个系统
    //    这样做的好处是，程序会停在错误发生后的状态，方便您连接调试器（如J-Link/ST-Link）
    //    来查看各个变量的值，分析出错的根本原因。
    while (1)
    {
        // 你也可以在这里控制一个错误指示LED不停地闪烁，
        // 作为一个直观的硬件错误提示。
    }
}
