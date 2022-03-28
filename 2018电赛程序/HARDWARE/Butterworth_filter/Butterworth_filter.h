
/*************************************************************************************************
                                   °ÍÌØÎÖË¹ÂË²¨Æ÷  v1.0
*************************************************************************************************/
#ifndef __BUTTERWORTH_FILTER_H
#define __BUTTERWORTH_FILTER_H
#include <stm32f10x.h>
#include "sys.h"

#if defined __BUTTERWORTH_FILTER_C



float Butterworth_filter(float input,u8 num);

#else

extern float Butterworth_filter(float input,u8 num);

#endif
#endif





     

