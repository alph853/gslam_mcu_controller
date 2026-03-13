#include "bsp_encoder.h"

#include "tim.h"

void BspEncoder_Init(void)
{
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);
}

int32_t BspEncoder_GetLeftCount(void)
{
  return (int32_t)__HAL_TIM_GET_COUNTER(&htim5);
}

int32_t BspEncoder_GetRightCount(void)
{
  return (int32_t)__HAL_TIM_GET_COUNTER(&htim2);
}
