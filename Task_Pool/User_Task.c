#include "User_Task.h"
#include "key.h"
#include "usb_device.h"
#include "usbd_hid.h"
#include "spiffs_user.h"

TaskHandle_t            Start_Task_Handler = NULL;       //开始任务句柄

TaskHandle_t            ProcessHandler = NULL;           //状态指示灯任务句柄

TaskHandle_t            TestTask_Handler = NULL;         //状态指示灯任务句柄

TaskHandle_t            StartKeyCtrlHandle = NULL;       //按键检测任务句柄

TaskHandle_t            AutoMovingHandle = NULL;         //重复绘制任务句柄

EventGroupHandle_t      KeyEventGroupHandler;	           //按键事件标志组句柄

xTimerHandle            KeyCheckLoopTimer = NULL; 

/* 鼠标报文结构体 */
struct mouseHID_t {
      uint8_t buttons;
      int8_t x;
      int8_t y;
      int8_t wheel;//滚轮
}; 
/* 鼠标报文结构体 */

struct XY_Pro
{
	  int8_t X;
	  int8_t Y;
		uint8_t Botton;
	
}XY_Pro[2000];//动作数据缓存
uint32_t  NUM;

struct Speed_Pro
{
	  uint8_t time;
    uint32_t Process;	
}Speed_Pro[50];//动作数据缓存
int8_t num;
int8_t SpeedNum;
uint8_t AutoFlag=1;
uint8_t SpeedAllProcess=1;
uint8_t AutoStart;

uint8_t LongPressStop;
uint8_t CrossStop;

uint8_t StoreFlag=1;

uint32_t BottonUpdate;
uint8_t MOVE_libUpdate=0;                                 //按下设置键重设动作库
uint8_t Record=0;                                         //记录开始标志
uint16_t AutoSpeedValu=1;                                 //自动重画速度量
int32_t XAbsoluteValu;                                    //画线绝对值坐标
int32_t YAbsoluteValu;                                    //画线绝对值坐标
int32_t AutoCount;                                        //自动重画记录量
uint16_t Stop_ResumeBit=0x01;                             //启停键启停标志位 

unsigned short keytemp;                                   //长短按判断位

extern osTimerId KeyCheckLoopTimer;                       //按键扫描定时器句柄
extern USBD_HandleTypeDef hUsbDeviceFS;

void ReadSpeed(struct Speed_Pro * speed);
void ReadTrace(struct XY_Pro *xy);
void SaveSpeed(struct Speed_Pro *speed);
void SaveTrace(struct XY_Pro *xy) ;
void SaveProcess(uint8_t process);
uint8_t ReadProcess(void);

void KeyCheckloopTimer_Callback  (void const *arg)
{
	   key_check_all_loop_1ms();
}

/**
 *@brief: 创建其他任务    
 *@details: 
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
void Start_Task(void *pvParameters)
{
      taskENTER_CRITICAL();
			
			/* 创建按键检测软件定时器——CMSIS OS*/
			osTimerDef(KeyCheckLoopTimer,KeyCheckloopTimer_Callback);
			KeyCheckLoopTimer = osTimerCreate(osTimer(KeyCheckLoopTimer),osTimerPeriodic,NULL);
			if(KeyCheckLoopTimer == NULL)
			{
					printf("osTimerCreate Failed!\r\n");
			}
			/* 创建按键检测软件定时器 */			
			
			/* 创建按键事件标志组 */
			KeyEventGroupHandler=xEventGroupCreate();
			/* 创建按键事件标志组 */
			
	    /*创建状态指示灯任务*/
      xTaskCreate((TaskFunction_t ) ProcessTask,
	                (const char*    ) "ProcessTask",
									(uint16_t       ) ProcessTASK_SIZE,
									(void*          ) NULL,
									(UBaseType_t    ) ProcessTASK_PRIO,
									(TaskHandle_t   ) &ProcessHandler);
	    /*创建状态指示灯任务*/									
										
 	    /*创建按键检测任务*/
      xTaskCreate((TaskFunction_t ) StartKeyCtrlTask,
	                (const char*    ) "StartKeyCtrlTask",
									(uint16_t       ) StartKeyCtrlTask_SIZE,
									(void*          ) NULL,
									(UBaseType_t    ) StartKeyCtrlTask_PRIO,
									(TaskHandle_t   ) &StartKeyCtrlHandle);
	    /*创建按键检测任务*/
									
 	    /*重复绘制任务*/
      xTaskCreate((TaskFunction_t ) AutoMovingTask,
	                (const char*    ) "AutoMovingTask",
									(uint16_t       ) AutoMovingTask_SIZE,
									(void*          ) NULL,
									(UBaseType_t    ) AutoMovingTask_PRIO,
									(TaskHandle_t   ) &AutoMovingHandle);
	    /*重复绘制任务*/
			
      vTaskSuspend(AutoMovingHandle);									
	    vTaskDelete(Start_Task_Handler);
      taskEXIT_CRITICAL();								
}

/**
 *@brief:   
 *@details: 所有请求处理任务  
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
void ProcessTask(void *pvParameters)
{
		EventBits_t EventValue;
	  struct Speed_Pro;
		for(;;)
		{
				xEventGroupWaitBits((EventGroupHandle_t	)KeyEventGroupHandler,		
										        (EventBits_t			  )EVENTBIT_ALL,
										        (BaseType_t			    )pdTRUE,				
										        (BaseType_t			    )pdFALSE,
								            (TickType_t			    )portMAX_DELAY);
			
			  EventValue=xEventGroupGetBits(KeyEventGroupHandler);
			
			  /* 画键 */
			  if(EventValue==CrossBit)
				{
					  TIM4->CCR4 = 50;
					  vTaskDelay(50);
					  xEventGroupClearBits(KeyEventGroupHandler,CrossBit);
					  if(MOVE_libUpdate == 1)
						{
							  //printf("CrossLine!\r\n");
								CrossLine(10);
						}
				}
				/* 画键 */
				
				/* 启停键 */
				else if(EventValue==StopBit)
				{
					  TIM4->CCR4 =50;
					  vTaskDelay(10);
					  xEventGroupClearBits(KeyEventGroupHandler,StopBit);
					  vTaskSuspend(AutoMovingHandle);
					  //printf("StopBit\r\n");

				}
				else if(EventValue==ResumeBit)
				{
					  TIM4->CCR4 =50;
					  vTaskDelay(10);
					  xEventGroupClearBits(KeyEventGroupHandler,ResumeBit);
					  if(StoreFlag==1)
						{
								//读速度与位置变量进来并一一赋值给XY_Pro,Speed_Pro变量，并且读进来SpeedAllProcess
							ReadSpeed(Speed_Pro);
							ReadTrace(XY_Pro);
							SpeedAllProcess = ReadProcess();
							  //读速度与位置变量进来并一一赋值给XY_Pro,Speed_Pro变量，并且读进来SpeedAllProcess
							  StoreFlag=0;
						}
            if(StoreFlag==1&&MOVE_libUpdate==1)
						{
								//存速度与位置
							  SaveTrace(XY_Pro);
								SaveSpeed(Speed_Pro);
							  //存速度与位置
							  StoreFlag=0;
						}
					  vTaskResume(AutoMovingHandle);
					  //printf("ResumeBit\r\n");

				}
				else if(EventValue==LongResumeBit)
				{
					  TIM4->CCR4 = 50;
					  vTaskDelay(50);
					  LongPressStop=1;
					  xEventGroupClearBits(KeyEventGroupHandler,LongResumeBit);

				}	
				/* 启停键 */
				
				/* 点键 */
				else if(EventValue==PointBit)
				{
					  TIM4->CCR4 = 50;
					  vTaskDelay(50);
					  xEventGroupClearBits(KeyEventGroupHandler,PointBit);
					  if(MOVE_libUpdate == 1)
						{
								Record = 1;
							  Click();
						}

				}	
        /* 点键 */

        /* 右键 */				
				else if(EventValue==MoveRightBit)
				{
					  TIM4->CCR4 = 50;
					  xEventGroupClearBits(KeyEventGroupHandler,MoveRightBit);
            if(MOVE_libUpdate == 1)
						{
								Move_OneUnitRight(1,0,10);
						}

				}	
				/* 右键 */
				
				/* 左键 */
				else if(EventValue==MoveLeftBit)
				{
					  TIM4->CCR4 = 50;
					  xEventGroupClearBits(KeyEventGroupHandler,MoveLeftBit);
            if(MOVE_libUpdate == 1)
						{
								Move_OneUnitLeft(1,0,10);						
						}		
				}	
        /* 左键 */

				/* 上键 */
				else if(EventValue==MoveUpBit)
				{
					  TIM4->CCR4 = 50;
					  xEventGroupClearBits(KeyEventGroupHandler,MoveUpBit);
            if(MOVE_libUpdate == 1)
						{
								Move_OneUnitUp(1,0,10);						
						}
				}	
        /* 上键 */	

				/* 下键 */
				else if(EventValue==MoveDownBit)
				{
					  TIM4->CCR4 = 50;
					  xEventGroupClearBits(KeyEventGroupHandler,MoveDownBit);
            if(MOVE_libUpdate == 1)
						{
								Move_OneUnitDown(1,0,10);						
						}
				}	
        /* 下键 */	

				/* 设置短键 */
				else if(EventValue==PressSetKeyBit)
				{
					  TIM4->CCR4 = 50;
					  vTaskDelay(50);
					  xEventGroupClearBits(KeyEventGroupHandler,PressSetKeyBit);
					  MOVE_libUpdate = 1;
				}	
        /* 设置短键 */	

				/* 设置长键 */
				else if(EventValue==LongPressSetKeyBit)
				{
					  TIM4->CCR4 = 50;
					  vTaskDelay(50);
					  xEventGroupClearBits(KeyEventGroupHandler,LongPressSetKeyBit);
					  
				}	
        /* 设置长键 */

				/* 加速键 */
				else if(EventValue==SpeedUpBit)
				{
					  TIM4->CCR4 = 50;
					  vTaskDelay(50);
					  xEventGroupClearBits(KeyEventGroupHandler,SpeedUpBit);
					  if(AutoStart==1)
						{
							  if(SpeedAllProcess==1)
								{
										SpeedAllProcess=1;
								}
								else SpeedAllProcess--;
								//更新SpeedAllProcess;
								SaveProcess(SpeedAllProcess);
								//更新SpeedAllProcess;
								//printf("SpeedAllProcess=%d\r\n",SpeedAllProcess);
						}
					  if(AutoSpeedValu<=1)
						{
								AutoSpeedValu = 1;
						}
						else 	AutoSpeedValu-=1;
						//printf("AutoSpeed=%d\r\n",AutoSpeedValu);
				}	
        /* 加速键 */		

				/* 减速键 */
				else if(EventValue==SpeedDownBit)
				{
					  TIM4->CCR4 = 50;
					  vTaskDelay(50);
					  xEventGroupClearBits(KeyEventGroupHandler,SpeedDownBit);
					  if(AutoStart==1)
						{
								SpeedAllProcess++;
							  if(SpeedAllProcess==100)
								{
										SpeedAllProcess=100;
								}
								//更新SpeedAllProcess;
								SaveProcess(SpeedAllProcess);
								//更新SpeedAllProcess;
						}
					  AutoSpeedValu+=1;
					  if(AutoSpeedValu==100)
						{
								AutoSpeedValu = 100;
						}
						Speed_Pro[SpeedNum].time=AutoSpeedValu;
						SpeedNum++;
				}	
        /* 减速键 */

       TIM4->CCR4 = 0;				
				
		}	
}

/**
 *@brief:   
 *@details: 按键检测任务  
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
void StartKeyCtrlTask(void *pvParameters)
{
	
	 /* 按键软件定时器每1毫米检测一次 */
	 osTimerStart(KeyCheckLoopTimer,1);
	 /* 按键软件定时器每1毫米检测一次 */
	
	 for(;;)
	 {  
				keytemp = key_read_value();
		 
		    /* 发送启停键通知 */
				if(keytemp == KEY0_SHORT)
				{
					  Stop_ResumeBit=~Stop_ResumeBit;
					  if(Stop_ResumeBit==65534)
						{
							  xEventGroupSetBits(KeyEventGroupHandler,EVENTBIT_ALL);
								xEventGroupSetBits(KeyEventGroupHandler,ResumeBit);
						}
					  if(Stop_ResumeBit==1)
						{
							   xEventGroupSetBits(KeyEventGroupHandler,EVENTBIT_ALL);
								 xEventGroupSetBits(KeyEventGroupHandler,StopBit);
						}							
            				
				}
				/* 发送启停键通知 */
				
		    /* 发送启停键长停通知 */
				if(keytemp == KEY0_LONG)
				{
					  xEventGroupSetBits(KeyEventGroupHandler,EVENTBIT_ALL);
						xEventGroupSetBits(KeyEventGroupHandler,LongResumeBit);
					  /* 长停再按一定是恢复 */
            Stop_ResumeBit = 0x01;					                     
				}
				/* 发送启停键长停通知 */
				
				/* 发送画键通知 */
				if(keytemp == KEY2_UP_SHORT) 
				{ 
					  xEventGroupSetBits(KeyEventGroupHandler,EVENTBIT_ALL);
						xEventGroupSetBits(KeyEventGroupHandler,CrossBit); 					
				}
        /* 发送画键通知 */
				
				/* 发送左键通知 */
				if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10) == 1) 
				{ 					  
					  xEventGroupSetBits(KeyEventGroupHandler,EVENTBIT_ALL);
						xEventGroupSetBits(KeyEventGroupHandler,MoveLeftBit);
					  /* 发送鼠标左移动报文 */
                  					
				}
        /* 发送左键通知 */
				
				/* 发送点键通知 */
				if(keytemp == KEY1_UP_SHORT) 
				{ 					
					  xEventGroupSetBits(KeyEventGroupHandler,EVENTBIT_ALL);
						xEventGroupSetBits(KeyEventGroupHandler,PointBit);	
				}
        /* 发送点键通知 */
				
				/* 发送设置键短通知 */
				if(keytemp == KEY9_UP_SHORT) 
				{ 
					  xEventGroupSetBits(KeyEventGroupHandler,EVENTBIT_ALL);
						xEventGroupSetBits(KeyEventGroupHandler,PressSetKeyBit);					
				}
        /* 发送设置键短通知 */

				/* 发送设置键长通知 */
				if(keytemp == KEY9_UP_LONG) 
				{ 
					  xEventGroupSetBits(KeyEventGroupHandler,EVENTBIT_ALL);
						xEventGroupSetBits(KeyEventGroupHandler,LongPressSetKeyBit);
					  /* 更新库 */
            MOVE_libUpdate=1;					
				}
        /* 发送设置键长通知 */
				
				/* 发送下键通知 */
				if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6) == 1) 
				{ 
					  xEventGroupSetBits(KeyEventGroupHandler,EVENTBIT_ALL);
						xEventGroupSetBits(KeyEventGroupHandler,MoveDownBit);					
				}
        /* 发送下键通知 */
				
				/* 发送上键通知 */
				if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5) == 1) 
				{ 
					  xEventGroupSetBits(KeyEventGroupHandler,EVENTBIT_ALL);
						xEventGroupSetBits(KeyEventGroupHandler,MoveUpBit);
				}
        /* 发送上键通知 */
				
				/* 发送右键通知 */
				if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4) == 1) 
				{ 				
					  xEventGroupSetBits(KeyEventGroupHandler,EVENTBIT_ALL);
						xEventGroupSetBits(KeyEventGroupHandler,MoveRightBit);
				}
        /* 发送右键通知 */
				
				/* 发送减速键通知 */
				if(keytemp == KEY8_UP_SHORT) 
				{ 					
					  xEventGroupSetBits(KeyEventGroupHandler,EVENTBIT_ALL);
						xEventGroupSetBits(KeyEventGroupHandler,SpeedDownBit);
				}
        /* 发送减速键通知 */
				
				/* 发送加速键短按通知 */
				if(keytemp == KEY7_UP_SHORT) 
				{ 					
					  xEventGroupSetBits(KeyEventGroupHandler,EVENTBIT_ALL);
						xEventGroupSetBits(KeyEventGroupHandler,SpeedUpBit);
				}
				/* 发送加速键短按通知 */
				
		}	
}

void AutoMovingTask(void *pvParameters)
{
		for(;;)
	  {
				AutoMoving(1);
				vTaskDelay(1);
		}
}
///////////////////////////////////////////////////////////////////////
/*                       非任务函数                                  */

/**
 *@brief:   
 *@details: 左移报文函数  
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
void Move_OneUnitLeft(int8_t Speed,uint8_t Key,uint8_t time)
{
		struct mouseHID_t mouseHID;
		struct XY_Pro;
	  struct Speed_Pro;
	  /* Speed为单位移动长度 */
		mouseHID.x = -Speed;
		mouseHID.y = 0;
		mouseHID.wheel = 0;	
	  mouseHID.buttons = 0X00;
    
	  
    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t)); 
	  if(Record == 1)
		{
		    XY_Pro[NUM].X = -Speed;
        XY_Pro[NUM].Y = 0;	
			  if(Key == 1)
        XY_Pro[NUM].Botton = 0x01;			
				else XY_Pro[NUM].Botton = 0x00;
				NUM++;
		}
		Speed_Pro[SpeedNum].time=AutoSpeedValu;
    vTaskDelay(time);
}

/**
 *@brief:   
 *@details: 右移报文函数  
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
void Move_OneUnitRight(int8_t Speed,uint8_t Key,uint8_t time)
{
		struct mouseHID_t mouseHID;
		struct XY_Pro;
	  /* Speed为单位移动长度 */
		mouseHID.x = Speed;
		mouseHID.y = 0;
		mouseHID.wheel = 0;	
	  mouseHID.buttons = 0X00;
  
    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t)); 
	  if(Record == 1)
		{
		    XY_Pro[NUM].X = Speed;
        XY_Pro[NUM].Y = 0;	
			  if(Key == 1)
        XY_Pro[NUM].Botton = 0x01;			
				else XY_Pro[NUM].Botton = 0x00;
				NUM++;
		}		
		Speed_Pro[SpeedNum].time=AutoSpeedValu;		
    vTaskDelay(time);
}

/**
 *@brief:   
 *@details: 上移报文函数  
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
void Move_OneUnitUp(int8_t Speed,uint8_t Key,uint8_t time)
{
		struct mouseHID_t mouseHID;
		struct XY_Pro;
	  /* Speed为单位移动长度 */
		mouseHID.x = 0;
		mouseHID.y = -Speed;
		mouseHID.wheel = 0;	
	  mouseHID.buttons = 0X00;
  
    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t)); 
	  if(Record == 1)
		{
		    XY_Pro[NUM].X = 0;
        XY_Pro[NUM].Y = -Speed;	
			  if(Key == 1)
        XY_Pro[NUM].Botton = 0x01;			
				else XY_Pro[NUM].Botton = 0x00;
				NUM++;				
		}
		Speed_Pro[SpeedNum].time=AutoSpeedValu;		
    vTaskDelay(time);
}

/**
 *@brief:   
 *@details: 下移报文函数  
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
void Move_OneUnitDown(int8_t Speed,uint8_t Key,uint8_t time)
{
		struct mouseHID_t mouseHID;
	  /* Speed为单位移动长度 */
		mouseHID.x = 0;
		mouseHID.y = Speed;
		mouseHID.wheel = 0;	
	  mouseHID.buttons = 0X00;
  
    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t)); 
	  if(Record == 1)
		{
		    XY_Pro[NUM].X = 0;
        XY_Pro[NUM].Y = Speed;	
			  if(Key == 1)
        XY_Pro[NUM].Botton = 0x01;			
				else XY_Pro[NUM].Botton = 0x00;
				NUM++;				
		}	
		Speed_Pro[SpeedNum].time=AutoSpeedValu;
    vTaskDelay(time);
}

/**
 *@brief:   
 *@details: 单击鼠标左键函数  
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
void Click()
{
		struct mouseHID_t mouseHID;
    struct XY_Pro;
    struct Speed_Pro;
	  /* Speed为单位移动长度 */
		mouseHID.x = 0;
		mouseHID.y = 0;
		mouseHID.wheel = 0;	
	  mouseHID.buttons = 0X01;
  
    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t)); 
    vTaskDelay(50);

	  mouseHID.buttons = 0X00;
	
    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t)); 
	  BottonUpdate = NUM;
	  Speed_Pro[num].Process=BottonUpdate;
	  num++;


    XY_Pro[NUM].X = 0;
    XY_Pro[NUM].Y = 0;
    XY_Pro[NUM].Botton = 1;		
	  NUM++;
		
		SpeedNum++;	
    AutoSpeedValu=0;	
}

void CrossLine(uint8_t Speed)
{
	   struct mouseHID_t mouseHID;
	   struct XY_Pro;
	   struct Speed_Pro;
	   int32_t Count;
	   int32_t NUMFlag;
	   Count = NUM-BottonUpdate;
	   NUMFlag=NUM;
	   //printf("NUM=%d,Count=%d,BottonUpdate=%d\r\n",NUM,Count,BottonUpdate);
	   /* 返回起点 */
		 while(Count>=0)
		 {
					mouseHID.x = -XY_Pro[NUMFlag].X;
					mouseHID.y = -XY_Pro[NUMFlag].Y;
					mouseHID.buttons = 0x00;	
					USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t));
					NUMFlag--;
          Count--;	
          vTaskDelay(1);			 
		 }
		 /* 返回起点 */
		 Count = NUM-BottonUpdate;
		 //printf("NUMFlag=%d\r\n",NUMFlag); 
		 /* 统计绝对值坐标 */
		 while(Count>=0)
		 {
			    XAbsoluteValu+=XY_Pro[NUMFlag].X;
			    YAbsoluteValu+=XY_Pro[NUMFlag].Y;
			    NUMFlag++;
			    Count--;
			    vTaskDelay(1);	
		 }
		 /* 统计绝对值坐标 */
		 //printf("%d,%d\r\n",XAbsoluteValu,YAbsoluteValu);
		 Count = NUM-BottonUpdate;
		 NUMFlag=NUMFlag-Count;
		 //printf("NUMFlag=%d\r\n",NUMFlag);
		 /* 画线 */
		 if(XAbsoluteValu>0 && YAbsoluteValu ==0)
		 {
					while(1)
					{
							XAbsoluteValu--;
						  if(XAbsoluteValu==0)
							{
								  YAbsoluteValu=0;
									break;
							}	
							mouseHID.x = 1;
							mouseHID.y = 0;
							mouseHID.buttons = 0x01;	
							XY_Pro[NUMFlag].X=1;
							XY_Pro[NUMFlag].Y=0;
							XY_Pro[NUMFlag].Botton=0x01;
							USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t));	
              NUMFlag++;	
 							BottonUpdate++;
              vTaskDelay(Speed);						
					}				
		 }
		 if(XAbsoluteValu<0 && YAbsoluteValu ==0)
		 { 
			    XAbsoluteValu=-XAbsoluteValu;
					while(1)
					{
							XAbsoluteValu--;
						  if(XAbsoluteValu==0)
							{
								  YAbsoluteValu=0;
									break;
							}	
							mouseHID.x = -1;
							mouseHID.y = 0;
							mouseHID.buttons = 0x01;	
							XY_Pro[NUMFlag].X=-1;
							XY_Pro[NUMFlag].Y=0;
							XY_Pro[NUMFlag].Botton=0x01;
							USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t));	
              BottonUpdate++;	
              NUMFlag++;							
              vTaskDelay(Speed);						
					}				
		 }
		 if(XAbsoluteValu==0 && YAbsoluteValu >0)
		 { 
					while(1)
					{
							YAbsoluteValu--;
						  if(YAbsoluteValu==0)
							{
								  XAbsoluteValu=0;
									break;
							}	
							mouseHID.x = 0;
							mouseHID.y = 1;
							mouseHID.buttons = 0x01;	
							XY_Pro[NUMFlag].X=0;
							XY_Pro[NUMFlag].Y=1;
							XY_Pro[NUMFlag].Botton=0x01;
							USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t));	
              BottonUpdate++;		
              NUMFlag++;							
              vTaskDelay(Speed);						
					}				
		 }
		 if(XAbsoluteValu==0 && YAbsoluteValu <0)
		 { 
			    YAbsoluteValu=-YAbsoluteValu;
					while(1)
					{
							YAbsoluteValu--;
						  if(YAbsoluteValu==0)
							{
								  XAbsoluteValu=0;
									break;
							}	
							mouseHID.x = 0;
							mouseHID.y = -1;
							mouseHID.buttons = 0x01;	
							XY_Pro[NUMFlag].X=0;
							XY_Pro[NUMFlag].Y=-1;
							XY_Pro[NUMFlag].Botton=0x01;
							USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t));	
              BottonUpdate++;	
              NUMFlag++;							
              vTaskDelay(Speed);						
					}				
		 }
		 if(XAbsoluteValu>0 && YAbsoluteValu<0)
		 {
					if(XAbsoluteValu>(-YAbsoluteValu))
					{
							while(1)
							{
								  XAbsoluteValu--;
								  if(XAbsoluteValu==0)
									{
										  YAbsoluteValu=0;
										  break;
									}
								  //printf("1\r\n");
									mouseHID.x = 1;
									mouseHID.y = 0;
									mouseHID.buttons = 0x01;	
								  XY_Pro[NUMFlag].X=1;
								  XY_Pro[NUMFlag].Y=0;
								  XY_Pro[NUMFlag].Botton=0x01;
									USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t));		
								  BottonUpdate++;
									NUMFlag++;
                  vTaskDelay(Speed);									
							}
					}
					else
					{
						  YAbsoluteValu=-YAbsoluteValu;
							while(1)
							{
							   YAbsoluteValu--;
						     if(YAbsoluteValu==0)
							   {
									   XAbsoluteValu=0;
									   break;
							    }	
								  //printf("2\r\n");
									mouseHID.x = 0;
									mouseHID.y = -1;
									mouseHID.buttons = 0x01;	
								  XY_Pro[NUMFlag].X=0;
								  XY_Pro[NUMFlag].Y=-1;
								  XY_Pro[NUMFlag].Botton=0x01;
									USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t));	
								  BottonUpdate++;
									NUMFlag++;
                  vTaskDelay(Speed);									
							}					
					}
			}
		  if(XAbsoluteValu>0 && YAbsoluteValu>0)
		  {
					if(XAbsoluteValu>YAbsoluteValu)
					{
							while(1)
							{
							   XAbsoluteValu--;
						     if(XAbsoluteValu==0)
							   {
									   YAbsoluteValu=0;
									   break;
							    }	
								  //printf("3\r\n");
									mouseHID.x = 1;
									mouseHID.y = 0;
									mouseHID.buttons = 0x01;	
								  XY_Pro[NUMFlag].X=1;
								  XY_Pro[NUMFlag].Y=0;
								  XY_Pro[NUMFlag].Botton=0x01;
									USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t));
                  BottonUpdate++;		
                  NUMFlag++;									
                  vTaskDelay(Speed);									
							}
					}
					else
					{
							while(1)
							{
							   YAbsoluteValu--;
						     if(YAbsoluteValu==0)
							   {
									   XAbsoluteValu=0;
									   break;
							    }
								  //printf("4\r\n");
									mouseHID.x = 0;
									mouseHID.y = 1;
									mouseHID.buttons = 0x01;	
								  XY_Pro[NUMFlag].X=0;
								  XY_Pro[NUMFlag].Y=1;
								  XY_Pro[NUMFlag].Botton=0x01;
									USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t));		
								  BottonUpdate++;
									NUMFlag++;
                  vTaskDelay(Speed);									
							}
					}
			}
		  if(XAbsoluteValu<0 && YAbsoluteValu<0)
		  {
					if((-XAbsoluteValu)>(-YAbsoluteValu))
					{
						  XAbsoluteValu=-XAbsoluteValu;
							while(1)
							{
							   XAbsoluteValu--;
						     if(XAbsoluteValu==0)
							   {
									   YAbsoluteValu=0;
									   break;
							    }
								  //printf("5\r\n");
									mouseHID.x = -1;
									mouseHID.y = 0;
									mouseHID.buttons = 0x01;	
								  XY_Pro[NUMFlag].X=-1;
								  XY_Pro[NUMFlag].Y=0;
								  XY_Pro[NUMFlag].Botton=0x01;
									USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t));	
								  BottonUpdate++;
									NUMFlag++;
                  vTaskDelay(Speed);									
							}
					}
					else
					{
		          //printf("%d,%d\r\n",XAbsoluteValu,YAbsoluteValu);
						  YAbsoluteValu=-YAbsoluteValu;
							while(1)
							{
							   YAbsoluteValu--;
						     if(YAbsoluteValu==0)
							   {
									   XAbsoluteValu=0;
									   break;
							    }
								  //printf("6\r\n");
									mouseHID.x = 0;
									mouseHID.y = -1;
									mouseHID.buttons = 0x01;	
								  XY_Pro[NUMFlag].X=0;
								  XY_Pro[NUMFlag].Y=-1;
								  XY_Pro[NUMFlag].Botton=0x01;
									USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t));
                  BottonUpdate++;
                  NUMFlag++;									
                  vTaskDelay(Speed);									
							}
					}
			}
		  if(XAbsoluteValu<0 && YAbsoluteValu>0)
		  {
					if((-XAbsoluteValu)>YAbsoluteValu)
					{
						  XAbsoluteValu=-XAbsoluteValu;
							while(1)
							{
							   XAbsoluteValu--;
						     if(XAbsoluteValu==0)
							   {
									   YAbsoluteValu=0;
									   break;
							    }
								  //printf("7\r\n");
									mouseHID.x = -1;
									mouseHID.y = 0;
									mouseHID.buttons = 0x01;	
								  XY_Pro[NUMFlag].X=-1;
								  XY_Pro[NUMFlag].Y=0;
								  XY_Pro[NUMFlag].Botton=0x01;
									USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t));
								  BottonUpdate++;
									NUMFlag++;
                  vTaskDelay(Speed);									
							}
					}
					else
					{
							while(1)
							{
							   YAbsoluteValu--;
						     if(YAbsoluteValu==0)
							   {
									   XAbsoluteValu=0;
									   break;
							    }
								  //printf("8\r\n");
									mouseHID.x = 0;
									mouseHID.y = 1;
									mouseHID.buttons = 0x01;	
								  XY_Pro[NUMFlag].X=0;
								  XY_Pro[NUMFlag].Y=1;
								  XY_Pro[NUMFlag].Botton=0x01;
									USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t));
                  BottonUpdate++;	
                  NUMFlag++;									
                  vTaskDelay(Speed);									
							}
					}
			}
			NUM=BottonUpdate;
			Speed_Pro[num].Process=NUM;
			num++;
      //printf("NUM=%d\r\n",NUM);		
      SpeedNum++;	
      AutoSpeedValu=0;			
}
/**
 *@brief:   
 *@details: 重复绘制 
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
void AutoMoving(uint8_t Speed)
{
	  struct mouseHID_t mouseHID;
	  struct XY_Pro;	
	  struct Speed_Pro;
    AutoStart=1;
    AutoCount = NUM;	
		while(1)
		{     
			    if(CrossStop==1&&LongPressStop==1)
					{
							vTaskSuspend(NULL);
						  LongPressStop=0;
					}
					if(AutoCount<0) break;
					mouseHID.buttons = 0X00;
					mouseHID.x = -XY_Pro[AutoCount].X;
					mouseHID.y = -XY_Pro[AutoCount].Y;
			    mouseHID.wheel = 0;
					USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t));		
          AutoCount--;		
          vTaskDelay(1);							
		}
		AutoCount=0;
	  while(1)
	  {
			    Speed=Speed_Pro[AutoFlag].time*SpeedAllProcess;
			    if(Speed_Pro[AutoFlag].time==0) {Speed=SpeedAllProcess;AutoFlag=1;}
					if(SpeedAllProcess==1) Speed=1;
			    if(Speed_Pro[AutoFlag].Process==AutoCount && Speed_Pro[AutoFlag].Process!=0)
					{
						  AutoFlag++;
					}
					if(AutoCount>NUM) break;
					mouseHID.x = XY_Pro[AutoCount].X;
					mouseHID.y = XY_Pro[AutoCount].Y;
					mouseHID.buttons = XY_Pro[AutoCount].Botton;
			    mouseHID.wheel = 0;
					USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t));	
          AutoCount++;
          vTaskDelay(Speed);
          CrossStop=1;					
	  }
}


/**
 *@brief:     
 *@details: 断开USB连接
 *@param[in]    
 *@param[out]  
 *@retval:     
 */
void USB_Relink(void)
{
	if((USB->CNTR&0x02) == 0)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();
		USB_StopDevice(USB);
		GPIOA->CRH&=0XFFF00FFF;
		GPIOA->CRH|=0X00033000;
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);
	}
}

void Move_test(int8_t a,int8_t b)
{
		struct mouseHID_t mouseHID;
	  /* Speed为单位移动长度 */
		mouseHID.x = a;
		mouseHID.y = b;
		mouseHID.wheel = 0;	
	  mouseHID.buttons = 0X01;
  
    USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&mouseHID, sizeof(struct mouseHID_t)); 
    HAL_Delay(50);
}

void SaveTrace(struct XY_Pro *xy)  
{
	uint8_t tra_arr[600];
	int n,i;
	char name[8];
	for(n=0;n<15;n++)
	{
		switch(n)
		{
			case 0:
				strcpy(name, "trace0");  break;
			case 1:
				strcpy(name, "trace1");  break;
			case 2:
				strcpy(name, "trace2");  break;
			case 3:
				strcpy(name, "trace3");  break;
			case 4:
				strcpy(name, "trace4");  break;
			case 5:
				strcpy(name, "trace5");  break;
			case 6:
				strcpy(name, "trace6");  break;
			case 7:
				strcpy(name, "trace7");  break;
			case 8:
				strcpy(name, "trace8");  break;
			case 9:
				strcpy(name, "trace9");  break;
			case 10:
				strcpy(name, "trace10");  break;
			case 11:
				strcpy(name, "trace11");  break;
			case 12:
				strcpy(name, "trace12");  break;
			case 13:
				strcpy(name, "trace13");  break;
			case 14:
				strcpy(name, "trace14");  break;
		}
		
		for(i=0;i<200;i++)
		{
			tra_arr[i*3] = xy[n*200+i].X;
			tra_arr[i*3+1] = xy[n*200+i].Y;
			tra_arr[i*3+2] = xy[n*200+i].Botton;
		}
		spiffs_file fd;
		SPIFFS_remove(&fs, name);
		fd = SPIFFS_open(&fs, name, SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
		SPIFFS_write(&fs, fd, (u8_t *)tra_arr, 600);
		SPIFFS_close(&fs, fd);
	}
}

void SaveSpeed(struct Speed_Pro *speed)
{
	uint8_t sp_arr[150];
	int i;
	
	for(i=0;i<50;i++)
	{
		sp_arr[i*3] = speed[i].time;
		sp_arr[i*3+1] = (speed[i].Process >> 8) & 0xff;  //高8位
		sp_arr[i*3+2] = speed[i].Process & 0xff;  //低8位
	}
	spiffs_file fd;
	SPIFFS_remove(&fs, "speed");
	fd = SPIFFS_open(&fs, "speed", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
	SPIFFS_write(&fs, fd, (u8_t *)sp_arr, 150);
	SPIFFS_close(&fs, fd);
}

void ReadTrace(struct XY_Pro *xy)
{
	uint8_t tra_arr[600];
	spiffs_file fd;
	int n,i;
	char name[8];
	for(n=0;n<15;n++)
	{
		switch(n)
		{
			case 0:
				strcpy(name, "trace0");  break;
			case 1:
				strcpy(name, "trace1");  break;
			case 2:
				strcpy(name, "trace2");  break;
			case 3:
				strcpy(name, "trace3");  break;
			case 4:
				strcpy(name, "trace4");  break;
			case 5:
				strcpy(name, "trace5");  break;
			case 6:
				strcpy(name, "trace6");  break;
			case 7:
				strcpy(name, "trace7");  break;
			case 8:
				strcpy(name, "trace8");  break;
			case 9:
				strcpy(name, "trace9");  break;
			case 10:
				strcpy(name, "trace10");  break;
			case 11:
				strcpy(name, "trace11");  break;
			case 12:
				strcpy(name, "trace12");  break;
			case 13:
				strcpy(name, "trace13");  break;
			case 14:
				strcpy(name, "trace14");  break;
		}
		
		fd = SPIFFS_open(&fs, name, SPIFFS_RDWR, 0);
		if (fd < 0) 
		{
			fd = SPIFFS_open(&fs, name, SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
		}
		SPIFFS_read(&fs, fd, (u8_t *)tra_arr, 600);
		SPIFFS_close(&fs, fd);
		
		for(i=0;i<200;i++)
		{
			xy[n*200+i].X = tra_arr[i*3];
			xy[n*200+i].Y = tra_arr[i*3+1];
			xy[n*200+i].Botton = tra_arr[i*3+2];
		}
	}
}

void ReadSpeed(struct Speed_Pro * speed)
{
	uint8_t sp_arr[150];
	spiffs_file fd;
	int i,temp;
	
	fd = SPIFFS_open(&fs, "speed", SPIFFS_RDWR, 0);
  if (fd < 0) 
	{
		fd = SPIFFS_open(&fs, "speed", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
  }
  SPIFFS_read(&fs, fd, (u8_t *)sp_arr, 150);
  SPIFFS_close(&fs, fd);
	
	for(i=0;i<50;i++)
	{
		speed[i].time = sp_arr[i*3];
		temp = sp_arr[i*3+1];
		speed[i].Process = (temp<<8) | sp_arr[i*3+2];
	}
}

void SaveProcess(uint8_t process)
{
	spiffs_file fd;
	SPIFFS_remove(&fs, "pro");
	fd = SPIFFS_open(&fs, "pro", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
	SPIFFS_write(&fs, fd, (u8_t *)&process, 1);
	SPIFFS_close(&fs, fd);
}

uint8_t ReadProcess(void)
{
	uint8_t process;
	spiffs_file fd;
	SPIFFS_remove(&fs, "pro");
	fd = SPIFFS_open(&fs, "pro", SPIFFS_CREAT | SPIFFS_TRUNC | SPIFFS_RDWR, 0);
	SPIFFS_read(&fs, fd, (u8_t *)&process, 1);
	SPIFFS_close(&fs, fd);
	return process;
}


/*                       非任务函数                                  */
///////////////////////////////////////////////////////////////////////

