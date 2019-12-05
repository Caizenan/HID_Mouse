#ifndef __led_H
#define __led_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "cmsis_os.h"

extern osThreadId LedCtrlTaskHandle;

#define LEDBREATH 0x01  // 呼吸灯
#define LEDFLICKER50MS 0x02  // 50ms闪烁
#define LEDFLICKER100MS 0x04  // 100ms闪烁
#define LEDFLICKER500MS 0x08  // 500ms闪烁
#define LEDON 0x010  // 常亮
#define LEDOFF 0x20  // 关灯

void LedCtrlFreertosInit(void);


#ifdef __cplusplus
}
#endif

#endif

