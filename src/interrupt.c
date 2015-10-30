#include "interrupt.h"
#include "global.h"
#include "measurement_ctrl.h"

extern char BT_message[100];
extern char Wifi_message[100];




__IO uint16_t TIM16_IC1ReadValue1 = 0;
__IO uint16_t TIM16_IC1ReadValue2 = 0;
__IO uint32_t PING_Response = 0;
__IO uint16_t TIM1_IC4ReadValue1 = 0;
__IO uint16_t TIM1_IC4ReadValue2 = 0;
__IO uint32_t Motor_Freq = 0;

uint16_t edgecntr = 0;

void TIM2_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

//Data sending frequency
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		SendData = 1;
	}
}


//timer to trigger distance measurement
void TIM6_DAC_IRQHandler(void)
{
	static uint8_t tim6_cnt = 0;
	if(TIM_GetFlagStatus(TIM6, TIM_IT_Update));
	{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
		tim6_cnt++;
		if(tim6_cnt == 2)
			if(!HAND_CTRL)
				control = 2;
		if(tim6_cnt == 8)
		{
			Start_Ping();
			tim6_cnt = 0;
		}
	}
}

//timer for 5 us PING output
void TIM7_IRQHandler(void)
{
	static uint16_t cnt_PING = 0;

	if(TIM_GetFlagStatus(TIM7, TIM_IT_Update));
	{
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
		if(cnt_PING == 0)
		{
			cnt_PING++;
		}
		else if(cnt_PING == 1)
		{
			TIM_Cmd(TIM7, DISABLE);
			GPIO_ResetBits(GPIOB,GPIO_Pin_8);
			TIM_SetCounter(TIM7, 0);
			cnt_PING = 0;
			Init_Ping_IN();

		}
	}
}

//Distance measurement - interrupt handler
void TIM1_UP_TIM16_IRQHandler(void)
{
	if(TIM_GetFlagStatus(TIM16, TIM_IT_CC1));
	{
		TIM_ClearITPendingBit(TIM16, TIM_IT_CC1);

		if(edgecntr == 0)
		{
			TIM16_IC1ReadValue1 = TIM_GetCapture1(TIM16);
			edgecntr++;
		}
		else
		{
			TIM16_IC1ReadValue2 = TIM_GetCapture1(TIM16);
			edgecntr = 0;

			if (TIM16_IC1ReadValue2 > TIM16_IC1ReadValue1)
			{
				PING_Response = (TIM16_IC1ReadValue2 - TIM16_IC1ReadValue1);
			}
			else if (TIM16_IC1ReadValue2 < TIM16_IC1ReadValue1)
			{
				PING_Response = ((0xFFFF - TIM16_IC1ReadValue1+1) + TIM16_IC1ReadValue2);
			}
			else
			{
				PING_Response = 0;
			}
			if(PING_Response != 0)
			{
				PING_Response = PING_Response * 5;
				Set_Distance(PING_Response);
				//control = 1;
			}
		}
	}
}

void USART1_IRQHandler(void)
{
	char ch;
	static uint8_t cnt = 1;
	uint8_t Usart3RxBuffer;
	static uint16_t overrun_cntr = 0;

	if ((USART1->SR & USART_FLAG_RXNE) != (u16)RESET)
	{
		ch = (char) USART_ReceiveData(USART1);

		//backspace
		if(ch == 8)
		{

			if(cnt > 1)
			{
				BT_message[cnt]='\0';
				cnt--;
				USART_SendData(USART1, '\r');
			}
		}

		else if(ch == '\n')
		{
			//parancs vége
			UserTyping = 0;

			if(TERM_FBCK)
			{
				SendString_USART("\r\n", USART1);
			}

			BT_messagearrived = cnt;
			cnt = 1;
		}
		else
		{
			BT_message[cnt] = ch;
			if(TERM_FBCK)
			{
				USART_SendData(USART1, ch);
				UserTyping = 1;
			}
//			UserTyping = 1;
			cnt++;
		}

		//if overrun, read status register first ...
		if(USART1->SR & USART_FLAG_ORE)
		{
			Usart3RxBuffer = (int8_t)(USART1->SR & (uint8_t)0xFF);  //read status
			Usart3RxBuffer = (int8_t)(USART1->DR & (uint8_t)0xFF);
			overrun_cntr++;
		}
	}
}

void USART3_IRQHandler(void)
{
	char ch;
	static uint16_t usart3_cnt = 1;

	if ((USART3->SR & USART_FLAG_RXNE) != (u16)RESET)
	{
		ch = (char) USART_ReceiveData(USART3);

		Wifi_message[usart3_cnt] = ch;
		if(Wifi_message[usart3_cnt] == '\n')
		{
			Wifi_messagearrived = usart3_cnt;
			Wifi_message[usart3_cnt] = '\0';
			usart3_cnt = 1;
		}

		else
			usart3_cnt++;
	}
}

//Motor speed measurement - interrupt handler
void TIM1_CC_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM1, TIM_IT_CC4) == SET)
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_CC4);


		TIM1_IC4ReadValue1 = TIM1_IC4ReadValue2;
		TIM1_IC4ReadValue2=TIM_GetCapture4(TIM1);

		if (TIM1_IC4ReadValue2 > TIM1_IC4ReadValue1)
		{
			Motor_Freq = (TIM1_IC4ReadValue2 - TIM1_IC4ReadValue1);
		}
		else if (TIM1_IC4ReadValue2 < TIM1_IC4ReadValue1)
		{
			Motor_Freq = ((0xFFFF - TIM1_IC4ReadValue1+1) + TIM1_IC4ReadValue2);
		}
		else
		{
			Motor_Freq = 0;
		}

		Motor_Freq = SystemCoreClock * 60 / 360 / Motor_Freq; // unit: rpm
		Set_RPM( (uint16_t) Motor_Freq);
	}
}
