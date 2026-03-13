#include "bsp_motor.h"

#include "main.h"
#include "tim.h"

static uint32_t duty_to_compare(float duty)
{
  if (duty < 0.0f)
  {
    duty = -duty;
  }
  if (duty > 1.0f)
  {
    duty = 1.0f;
  }

  return (uint32_t)((float)__HAL_TIM_GET_AUTORELOAD(&htim1) * duty);
}

static void set_half_bridge(uint32_t forward_channel, uint32_t reverse_channel, float duty)
{
  uint32_t compare = duty_to_compare(duty);

  if (duty >= 0.0f)
  {
    __HAL_TIM_SET_COMPARE(&htim1, forward_channel, compare);
    __HAL_TIM_SET_COMPARE(&htim1, reverse_channel, 0);
  }
  else
  {
    __HAL_TIM_SET_COMPARE(&htim1, forward_channel, 0);
    __HAL_TIM_SET_COMPARE(&htim1, reverse_channel, compare);
  }
}

void BspMotor_Init(void)
{
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

  BspMotor_Enable(false);
  BspMotor_SetLeftDuty(0.0f);
  BspMotor_SetRightDuty(0.0f);
}

void BspMotor_Enable(bool enabled)
{
  HAL_GPIO_WritePin(EnableLeft_GPIO_Port, EnableLeft_Pin, enabled ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(EnableRight_GPIO_Port, EnableRight_Pin, enabled ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void BspMotor_SetLeftDuty(float duty)
{
  set_half_bridge(TIM_CHANNEL_1, TIM_CHANNEL_2, duty);
}

void BspMotor_SetRightDuty(float duty)
{
  set_half_bridge(TIM_CHANNEL_3, TIM_CHANNEL_4, duty);
}
