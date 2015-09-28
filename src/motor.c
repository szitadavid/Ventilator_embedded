#include "motor.h"

void M_Default(void)
{
	//HBridge not used -> disable
	GPIO_SetBits(M_D1_PORT, M_D1_PIN);		//HBridge disabled
	GPIO_ResetBits(M_EN_PORT, M_EN_PIN);	//HBridge disabled
	M_Set_DC(0);
}

void M_Rotate_CW(void)
{
	GPIO_SetBits(M_D1_PORT, M_D1_PIN);
	TIM3->CCR3 = M_DutyCycle;
}

void M_Set_DC(uint16_t speed)
{
	M_DutyCycle = speed;
	M_Rotate_CW();
}

uint16_t M_Get_DC(void)
{
	return M_DutyCycle;
}
