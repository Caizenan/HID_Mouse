#ifndef __key_H
#define __key_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "main.h"
#include "cmsis_os.h"
#define shortmaxclickms (300)
#define shortminclickms (10)
#define longclickms (800)

#define KEY_EVENT_NULL                     0x0000  
#define KEY_EVENT_DOWN                     0x0001
#define KEY_EVENT_UP_SHORT                 0x0002  // 短按后松开事件
#define KEY_EVENT_UP_LONG                  0x0003  // 长按后松开事件
#define KEY_EVENT_UP_DOUBLE                0x0004  // 双击后松开事件
#define KEY_EVENT_SHORT                    0x0005
#define KEY_EVENT_LONG                     0x0006
#define KEY_EVENT_DOUBLE                   0x0007



__packed typedef struct
{
	
	uint16_t ResTapMax;
	uint16_t ResTapMin;
	uint8_t datasum;
	
}keyset_t;

/******************************************************************************
                           User Interface [START]
*******************************************************************************/

/* 注册启停按键 */ 
#define KEY0_NUM                           0x0001
 
#define KEY0_DOWN               (KEY_EVENT_DOWN      | KEY0_NUM<<8)
#define KEY0_UP_SHORT           (KEY_EVENT_UP_SHORT  | KEY0_NUM<<8)
#define KEY0_UP_LONG            (KEY_EVENT_UP_LONG   | KEY0_NUM<<8)
#define KEY0_UP_DOUBLE          (KEY_EVENT_UP_DOUBLE | KEY0_NUM<<8)
#define KEY0_SHORT              (KEY_EVENT_SHORT     | KEY0_NUM<<8)
#define KEY0_LONG               (KEY_EVENT_LONG      | KEY0_NUM<<8) 
#define KEY0_DOUBLE             (KEY_EVENT_DOUBLE    | KEY0_NUM<<8)
 /* 注册启停按键 */ 

/* 注册点按键 */ 
#define KEY1_NUM                           0x0002
 
#define KEY1_DOWN               (KEY_EVENT_DOWN      | KEY1_NUM<<8)
#define KEY1_UP_SHORT           (KEY_EVENT_UP_SHORT  | KEY1_NUM<<8)
#define KEY1_UP_LONG            (KEY_EVENT_UP_LONG   | KEY1_NUM<<8)
#define KEY1_UP_DOUBLE          (KEY_EVENT_UP_DOUBLE | KEY1_NUM<<8)
#define KEY1_SHORT              (KEY_EVENT_SHORT     | KEY1_NUM<<8)
#define KEY1_LONG               (KEY_EVENT_LONG      | KEY1_NUM<<8) 
#define KEY1_DOUBLE             (KEY_EVENT_DOUBLE    | KEY1_NUM<<8)
 /* 注册点按键 */ 
 
 /* 注册画按键 */ 
#define KEY2_NUM                           0x0003
 
#define KEY2_DOWN               (KEY_EVENT_DOWN      | KEY2_NUM<<8)
#define KEY2_UP_SHORT           (KEY_EVENT_UP_SHORT  | KEY2_NUM<<8)
#define KEY2_UP_LONG            (KEY_EVENT_UP_LONG   | KEY2_NUM<<8)
#define KEY2_UP_DOUBLE          (KEY_EVENT_UP_DOUBLE | KEY2_NUM<<8)
#define KEY2_SHORT              (KEY_EVENT_SHORT     | KEY2_NUM<<8)
#define KEY2_LONG               (KEY_EVENT_LONG      | KEY2_NUM<<8) 
#define KEY2_DOUBLE             (KEY_EVENT_DOUBLE    | KEY2_NUM<<8)
 /* 注册画按键 */ 
 
 /* 注册加速按键 */ 
#define KEY7_NUM                           0x0004
 
#define KEY7_DOWN               (KEY_EVENT_DOWN      | KEY7_NUM<<8)
#define KEY7_UP_SHORT           (KEY_EVENT_UP_SHORT  | KEY7_NUM<<8)
#define KEY7_UP_LONG            (KEY_EVENT_UP_LONG   | KEY7_NUM<<8)
#define KEY7_UP_DOUBLE          (KEY_EVENT_UP_DOUBLE | KEY7_NUM<<8)
#define KEY7_SHORT              (KEY_EVENT_SHORT     | KEY7_NUM<<8)
#define KEY7_LONG               (KEY_EVENT_LONG      | KEY7_NUM<<8) 
#define KEY7_DOUBLE             (KEY_EVENT_DOUBLE    | KEY7_NUM<<8)
/* 注册加速按键 */ 

 /* 注册减速按键 */ 
#define KEY8_NUM                           0x0005
 
#define KEY8_DOWN               (KEY_EVENT_DOWN      | KEY8_NUM<<8)
#define KEY8_UP_SHORT           (KEY_EVENT_UP_SHORT  | KEY8_NUM<<8)
#define KEY8_UP_LONG            (KEY_EVENT_UP_LONG   | KEY8_NUM<<8)
#define KEY8_UP_DOUBLE          (KEY_EVENT_UP_DOUBLE | KEY8_NUM<<8)
#define KEY8_SHORT              (KEY_EVENT_SHORT     | KEY8_NUM<<8)
#define KEY8_LONG               (KEY_EVENT_LONG      | KEY8_NUM<<8) 
#define KEY8_DOUBLE             (KEY_EVENT_DOUBLE    | KEY8_NUM<<8)
/* 注册减速按键 */ 

 /* 注册设置按键 */ 
#define KEY9_NUM                           0x0006
 
#define KEY9_DOWN               (KEY_EVENT_DOWN      | KEY9_NUM<<8)
#define KEY9_UP_SHORT           (KEY_EVENT_UP_SHORT  | KEY9_NUM<<8)
#define KEY9_UP_LONG            (KEY_EVENT_UP_LONG   | KEY9_NUM<<8)
#define KEY9_UP_DOUBLE          (KEY_EVENT_UP_DOUBLE | KEY9_NUM<<8)
#define KEY9_SHORT              (KEY_EVENT_SHORT     | KEY9_NUM<<8)
#define KEY9_LONG               (KEY_EVENT_LONG      | KEY9_NUM<<8) 
#define KEY9_DOUBLE             (KEY_EVENT_DOUBLE    | KEY9_NUM<<8)
/* 注册设置按键 */ 

/******************************************************************************
                           User Interface [END]
*******************************************************************************/

void KeyCtrlFreertosInit(void);
uint32_t key_adc2set(uint8_t setmax);


void key_check_all_loop_1ms(void);
unsigned short key_read_value(void);

#ifdef __cplusplus
}
#endif

#endif

