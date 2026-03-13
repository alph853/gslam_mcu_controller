#include "bsp_uart.h"

#include <string.h>

#include "usart.h"

static uint8_t g_rx_byte;
static uint8_t g_tx_buffer[64];
static volatile uint8_t g_tx_busy;

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

bool BspUart_Write(const uint8_t *data, uint16_t size)
{
  if ((g_tx_busy != 0u) || (size > sizeof(g_tx_buffer)))
  {
    return false;
  }

  memcpy(g_tx_buffer, data, size);
  g_tx_busy = 1u;

  if (HAL_UART_Transmit_IT(&huart1, g_tx_buffer, size) != HAL_OK)
  {
    g_tx_busy = 0u;
    return false;
  }

  return true;
}

void BspUart_OnTxComplete(void)
{
  g_tx_busy = 0u;
}
