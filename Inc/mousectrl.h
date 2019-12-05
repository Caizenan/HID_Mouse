#ifndef __mousectrl_H
#define __mousectrl_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "cmsis_os.h"

#define MouseLeftShortClick   0x01
#define MouseLeftLong			0x02
//#define MouseLeftLong			0x02
#define MouseUpdate 0x01
extern osThreadId MouseCtrlTaskHandle;

void MouseCtrlFreertosInit(void);


#ifdef __cplusplus
}
#endif

#endif

