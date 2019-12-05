#include "led.h"
#include "tim.h"

osThreadId LedCtrlTaskHandle;

void StartLedCtrlTask(void const * argument);  
static void SetBrightness(uint32_t brightness);

void LedCtrlFreertosInit(void)
{	
  osThreadDef(LedCtrlTask, StartLedCtrlTask, osPriorityNormal, 0, 128);
  LedCtrlTaskHandle = osThreadCreate(osThread(LedCtrlTask), NULL);	

}

static void SetBrightness(uint32_t brightness)
{
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_4,brightness);
}
void StartLedCtrlTask(void const * argument)
{
	osEvent ledevent;
	uint8_t setlight = LEDBREATH;
	uint8_t setBright = 0;
	uint8_t timetemp;
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);
	for(;;)
	{
		ledevent = osSignalWait(0xff,25);
		if(ledevent.status == osEventSignal)
		{
			timetemp = 0;
			setBright = 0;
			if(ledevent.value.signals&LEDBREATH)
			{
				setlight = LEDBREATH;
			}
			if(ledevent.value.signals&LEDFLICKER50MS)
			{
				setlight = LEDFLICKER50MS;
			}
			if(ledevent.value.signals&LEDFLICKER100MS)
			{
				
				setlight = LEDFLICKER100MS;
			}				
			if(ledevent.value.signals&LEDFLICKER500MS)
			{
				setlight = LEDFLICKER500MS;
			}			
			if(ledevent.value.signals&LEDON)
			{
				setlight = LEDON;
			}		
			if(ledevent.value.signals&LEDOFF)
			{
				setlight = LEDOFF;
			}		
		}
		switch(setlight)
		{
			case LEDBREATH:
			   if(timetemp == 0)
				{
					if(setBright == 60) timetemp = 1;
					else setBright ++;
				}
				else
				{
					if(setBright == 0) timetemp = 0;
					else setBright --;
				}
				break;
			case LEDFLICKER50MS:
			   if(timetemp == 2)
				{
					timetemp = 0;
					if(setBright == 80) setBright = 0;
					else setBright = 80;
				}
				else timetemp ++;
				break;	
			case LEDFLICKER100MS:
			   if(timetemp == 4)
				{
					timetemp = 0;
					if(setBright == 80) setBright = 0;
					else setBright = 80;
				}
				else timetemp ++;
				break;			
			case LEDFLICKER500MS:
				if(timetemp == 20)
				{
					timetemp = 0;
					if(setBright == 80) setBright = 0;
					else setBright = 80;
				}
				else timetemp ++;
				break;		
			case LEDON:
				setBright = 80;
				break;
			case LEDOFF:
				setBright = 0;
				break;
			default :
				setBright = 0;
				break;
		}
		SetBrightness(setBright);
	}
}


