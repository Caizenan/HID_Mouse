#include "mousectrl.h"
#include "usbd_hid.h"
#include "usb_device.h"
#include "led.h"
#include "key.h"

osThreadId MouseCtrlTaskHandle;
uint8_t HID_Mouse_Buffer[4];
uint16_t Dis_X = 0,Dis_Y = 0; 
char Store = 0;
void StartMouseCtrlTask(void const * argument);  
static void GetPointerData(uint8_t * pbuf);


void MouseCtrlFreertosInit(void)
{	
  osThreadDef(MouseCtrlTask, StartMouseCtrlTask, osPriorityRealtime, 0, 128);
  MouseCtrlTaskHandle = osThreadCreate(osThread(MouseCtrlTask), NULL);	
}

void StartMouseCtrlTask(void const * argument)
{
	osEvent MouseEvent;
	uint8_t count;
	uint8_t Leftkeystate;
	HID_Mouse_Buffer[0] = 0x08;
	osDelay(1000);
	//osSignalSet(MouseCtrlTaskHandle,MouseUpdate);
	//USBD_HID_SendReport(&hUsbDeviceFS, HID_Mouse_Buffer, 4);
	for(;;)
	{
		MouseEvent = osSignalWait(0xff,2);
		if(MouseEvent.status == osEventSignal)
		{
			if(MouseEvent.value.signals&MouseLeftShortClick)
			{
				SET_BIT(HID_Mouse_Buffer[0],1<<0);
				Leftkeystate = 0;
				osSignalSet(LedCtrlTaskHandle,LEDBREATH);
			}
			if(MouseEvent.value.signals&MouseLeftLong)
			{
				SET_BIT(HID_Mouse_Buffer[0],1<<0);
				Leftkeystate = 0xff;
				osSignalSet(LedCtrlTaskHandle,LEDFLICKER50MS);
			}
		}
		
		if(count ++ >= USBD_HID_GetPollingInterval(&hUsbDeviceFS))
		{
			GetPointerData(HID_Mouse_Buffer);
			if ((HID_Mouse_Buffer[0] != 0x08) || (HID_Mouse_Buffer[1] != 0) || (HID_Mouse_Buffer[2] != 0) || Leftkeystate == 0x80)
			{
				if(Leftkeystate == 0x80) Leftkeystate = 0xff;
				//if(HID_Mouse_Buffer[0] == 0x01) HID_Mouse_Buffer[0] = 0x00;
				//else HID_Mouse_Buffer[0] = 0x01;
				USBD_HID_SendReport(&hUsbDeviceFS, HID_Mouse_Buffer, 4);				
				if(Leftkeystate == 0 && (READ_BIT(HID_Mouse_Buffer[0],1<<0) == 1)) CLEAR_BIT(HID_Mouse_Buffer[0],1<<0),Leftkeystate = 0x80;
			}
			count = 0;  
		}
		//osDelay(1);
	}
	
}

static void GetPointerData(uint8_t * pbuf)
{
  int8_t x = 0, y = 0/*,set = 0x08*/;
  #define CURSOR_STEP     key_adc2set(10)
	
  if(HAL_GPIO_ReadPin(Key_UP_GPIO_Port,Key_UP_Pin)) y -= CURSOR_STEP;
  if(HAL_GPIO_ReadPin(Key_DOWN_GPIO_Port,Key_DOWN_Pin)) y += CURSOR_STEP;
  if(HAL_GPIO_ReadPin(Key_RIGHT_GPIO_Port,Key_RIGHT_Pin)) x += CURSOR_STEP;
  if(HAL_GPIO_ReadPin(Key_LEFT_GPIO_Port,Key_LEFT_Pin)) x -= CURSOR_STEP;
  //if(HAL_GPIO_ReadPin(Key_CENTER_GPIO_Port,Key_CENTER_Pin)) SET_BIT(set,1<<0),pbuf[4] = 1;
  //else CLEAR_BIT(set,1<<0),pbuf[4] ++;
  //pbuf[0] = set;
  pbuf[1] = x;
  pbuf[2] = y;
  pbuf[3] = 0;
	
	if(Store==1)
	{
		Dis_X += x;
		Dis_Y += y;
	}
}

void back_to_origin(void)
{
	
	
}

void auto_move(void)
{

}

