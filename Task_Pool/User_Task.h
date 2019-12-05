#ifndef __User_Task_H
#define __User_Task_H

#include "cmsis_os.h"
#include "main.h"

#define     Start_Task_PRIO           1                  //开始任务优先级
#define     Start_Task_SIZE           256                //开始任务堆栈
void Start_Task(void *pvParameters);                     //开始任务函数声明

#define     ProcessTASK_PRIO          2                  //请求处理任务优先级
#define     ProcessTASK_SIZE          256                //请求处理任务堆栈
void ProcessTask(void *pvParameters);                    //请求处理任务函数声明

#define     Test_TASK_PRIO            2                 
#define     TestTask_SIZE             50               
void TestTask(void *pvParameters);

#define     StartKeyCtrlTask_PRIO     2                  //按键检测任务优先级            
#define     StartKeyCtrlTask_SIZE     256                //按键检测任务堆栈
void StartKeyCtrlTask(void *pvParameters);               //按键检测任务函数声明

#define     AutoMovingTask_PRIO       3                  //重复绘制任务优先级            
#define     AutoMovingTask_SIZE       128                //重复绘制任务堆栈
void AutoMovingTask(void *pvParameters);                 //重复绘制任务函数声明

/* 按键事件标志位 */
#define     MoveRightBit                  (1<<1)           
#define     MoveLeftBit                   (1<<2)
#define     MoveUpBit                     (1<<3)
#define     MoveDownBit                   (1<<4)
#define     SpeedUpBit                    (1<<5)
#define     SpeedDownBit                  (1<<6)
#define     PressSetKeyBit                (1<<7)
#define     LongPressSetKeyBit            (1<<8)
#define     StopLongBit                   (1<<9)
#define     PointBit                      (1<<10)
#define     CrossBit                      (1<<11)
#define     StopBit                       (1<<12)
#define     ResumeBit                     (1<<13)
#define     LongResumeBit                 (1<<14)

#define     EVENTBIT_ALL                  (1<<0)
/* 按键事件标志位 */

void Move_OneUnitLeft(int8_t Speed,uint8_t Key,uint8_t time);
void Move_OneUnitUp(int8_t Speed,uint8_t Key,uint8_t time);
void Move_OneUnitDown(int8_t Speed,uint8_t Key,uint8_t time);
void Move_OneUnitRight(int8_t Speed,uint8_t Key,uint8_t time);
void Click(void);
void AutoMoving(uint8_t Speed);
void Move_test(int8_t a,int8_t b);
void USB_Relink(void);
void CrossLine(uint8_t Speed);

//void SaveTrace(struct XY_Pro *xy);
//void SaveSpeed(struct Speed_Pro *speed);
//void ReadTrace(struct XY_Pro *xy);
//void ReadSpeed(struct Speed_Pro *speed);
#endif



