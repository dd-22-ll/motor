#ifndef __INF_JOYSTICKKEY_H
#define __INF_JOYSTICKKEY_H
#include "Common_Debug.h"
#include "FreeRTOS.h"
#include "task.h"

typedef enum
{
    KEY_NONE,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4
} Key_t;

Key_t Inf_Key_ReadKey(void);

#endif
