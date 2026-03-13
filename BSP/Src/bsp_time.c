#include "bsp_time.h"

#include "tim.h"

void BspTime_Init(void)
{
}

void BspTime_StartControlTick(void)
{
  HAL_TIM_Base_Start_IT(&htim6);
}

uint32_t BspTime_NowMs(void)
{
  return HAL_GetTick();
}

float BspTime_ControlDtSeconds(void)
{
  return 0.00525f;
}
