#include "bsp_uart.h"

#include "usart.h"

static uint8_t g_rx_byte;

void BspUart_Init(void)
{
  HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
  BspUart_ArmRxIT();
}

void BspUart_ArmRxIT(void)
{
  HAL_UART_Receive_IT(&huart1, &g_rx_byte, 1);
}

uint8_t BspUart_GetRxByte(void)
{
  return g_rx_byte;
}

void BspUart_Write(const uint8_t *data, uint16_t size)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)data, size, 10);
}
