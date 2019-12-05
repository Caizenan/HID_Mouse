#ifndef __User_Task_H
#define __User_Task_H

#include "cmsis_os.h"
#include "main.h"

#define     Start_Task_PRIO           1                  //��ʼ�������ȼ�
#define     Start_Task_SIZE           256                //��ʼ�����ջ
void Start_Task(void *pvParameters);                     //��ʼ����������

#define     ProcessTASK_PRIO          2                  //�������������ȼ�
#define     ProcessTASK_SIZE          256                //�����������ջ
void ProcessTask(void *pvParameters);                    //����������������

#define     Test_TASK_PRIO            2                 
#define     TestTask_SIZE             50               
void TestTask(void *pvParameters);

#define     StartKeyCtrlTask_PRIO     2                  //��������������ȼ�            
#define     StartKeyCtrlTask_SIZE     256                //������������ջ
void StartKeyCtrlTask(void *pvParameters);               //�����������������

#define     AutoMovingTask_PRIO       3                  //�ظ������������ȼ�            
#define     AutoMovingTask_SIZE       128                //�ظ����������ջ
void AutoMovingTask(void *pvParameters);                 //�ظ���������������

/* �����¼���־λ */
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
/* �����¼���־λ */

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



