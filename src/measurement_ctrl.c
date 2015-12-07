#include "measurement_ctrl.h"


void Start_Ping(void)
{
	Init_Ping_OUT();			//PB8 output
	GPIO_SetBits(GPIOB,GPIO_Pin_8);
	TIM_Cmd(TIM7, ENABLE);		//start 5 us pulse
}

void Init_Ping_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static uint8_t pingInit = 0;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	//ez csak egyszer fut le, a legels� alkalommal (lehetne k�l�n f�ggv�ny vagy belef�r a start_PINGbe is)
	if(pingInit == 0)
	{
		TIM_ITConfig(TIM16, TIM_IT_Update, DISABLE);
		TIM_ITConfig(TIM16, TIM_IT_CC1, DISABLE);

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);

		// TIM1 (Ttick = 5 us)
		TIM_TimeBaseStructInit(&TIM_InitStructure);
		TIM_InitStructure.TIM_Prescaler = 119;
		TIM_TimeBaseInit(TIM16, &TIM_InitStructure);

		TIM_ICStructInit(&TIM_ICInitStructure);
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
		TIM_ICInitStructure.TIM_ICFilter = 0x0;
		TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
		TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
		TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
		TIM_ICInit(TIM16, &TIM_ICInitStructure);

		//highest priority
		NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM16_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_Init(&NVIC_InitStructure);
		TIM_Cmd(TIM16, ENABLE);
		pingInit = 1;
	}
	TIM_ClearITPendingBit(TIM16, TIM_IT_CC1);
	TIM_ITConfig(TIM16, TIM_IT_CC1, ENABLE);

}

void Init_Ping_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	TIM_ClearITPendingBit(TIM16, TIM_IT_CC1);
	TIM_ITConfig(TIM16, TIM_IT_CC1, DISABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB, GPIO_Pin_8);
}

void Init_CNY70(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1 | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// TICK 10 us
	TIM_TimeBaseStructInit(&TIM_InitStructure);
	TIM_InitStructure.TIM_Prescaler = 359;
	TIM_TimeBaseInit(TIM1, &TIM_InitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM1, &TIM_ICInitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	TIM_ITConfig(TIM1, TIM_IT_CC4, ENABLE);

	TIM_Cmd(TIM1, ENABLE);
}


void Set_Distance(uint32_t time)
{
	time = time * SPEED_OF_SOUND / 20000;		// s_object = t_impulse * v_sound / 2    [cm]

	//ezen k�ne jav�tani, valami�rt nem tudom letiltani a megszak�t�sokat
	// am�g a trigger 5 us-os t�sk�t kiadom
	//update: siker�lt, de az�rt maradjon itt az ellen�rz�s
	if(time != 0)
		Distance = (uint16_t) time;
}

void Set_RPM(uint16_t data)
{
	RPM = data;
}

uint16_t Get_Distance(void)
{
	return Distance;
}
uint16_t Get_RPM(void)
{
	return RPM;
}

void Set_RefPos(uint16_t data)
{
	RefPos = data;
}

uint16_t Get_RefPos(void)
{
	return RefPos;
}

float MyPIDControl(float error)
{
	float val;

	val = myPID.b0 * error + myPID.b1 * myPID.e_prev + myPID.b2 * myPID.e_prevprev
			+ myPID.u_prev;
	if(val > 300)
		val = 300;
	if(val < 0)
		val = 0;

	myPID.e_prevprev = myPID.e_prev;
	myPID.e_prev = error;

	myPID.u_prevprev = myPID.u_prev;
	myPID.u_prev = val;

	return val + 600;
}

void Default_MyPIDParam(void)
{
	myPID.u_prev = 0;
	myPID.u_prevprev = 0;
	myPID.e_prev = 0;
	myPID.e_prevprev = 0;

	myPID.Kc = 5;
	myPID.Ti = 1;
	myPID.Td = 1;
	myPID.Ts = T_SAMPLE;
//	myPID.b0 = 2.016;
//	myPID.b1 = -1.8;
//	myPID.b2 = 0;
//
//	myPID.a1 = -1;
//	myPID.a2 = 0;

	SetMyPIDParameters();

	RefPos = 80;
}

void SetMyPIDParameters(void)
{
	myPID.a1 = -1;
	myPID.b0 = myPID.Kc*(1 + myPID.Ts/myPID.Ti + myPID.Td/myPID.Ts);
	myPID.b1 = -1*myPID.Kc*(1 + 2*myPID.Td/myPID.Ts);
	myPID.b2 = myPID.Kc * myPID.Td/myPID.Ts;
}
