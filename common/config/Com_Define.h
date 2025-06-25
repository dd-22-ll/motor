#ifndef __COM_DEFINE_H
#define __COM_DEFINE_H
#include "Common_Debug.h"



#define LIMIT(x, min, max) ((x) > (max) ? (max) : ((x) < (min) ? (min) : (x)))

typedef enum
{
    Status_OK = 0,
    Status_ERROR,
    Status_BUSY,
    Status_TIMEOUT
} Status_t;

#endif
