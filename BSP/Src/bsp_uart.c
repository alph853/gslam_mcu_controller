#include "bsp_uart.h"

#include <string.h>

#include "usart.h"

enum
{
  UART_RX_DMA_BUFFER_SIZE = 64u,
  UART_RX_RING_BUFFER_SIZE = 256u,
  UART_TX_BUFFER_SIZE = 64u,
};

static uint8_t g_rx_dma_buffer[UART_RX_DMA_BUFFER_SIZE];
static uint8_t g_rx_ring_buffer[UART_RX_RING_BUFFER_SIZE];
static uint8_t g_tx_buffer[UART_TX_BUFFER_SIZE];
static uint16_t g_rx_dma_read_index;
static uint16_t g_rx_ring_head;
static uint16_t g_rx_ring_tail;
static volatile uint8_t g_tx_busy;

static void ring_push(uint8_t byte)
{
  const uint16_t next_head = (uint16_t)((g_rx_ring_head + 1u) % UART_RX_RING_BUFFER_SIZE);
  if (next_head == g_rx_ring_tail)
  {
    g_rx_ring_tail = (uint16_t)((g_rx_ring_tail + 1u) % UART_RX_RING_BUFFER_SIZE);
  }

  g_rx_ring_buffer[g_rx_ring_head] = byte;
  g_rx_ring_head = next_head;
}

void BspUart_Init(void)
{
  g_rx_dma_read_index = 0u;
  g_rx_ring_head = 0u;
  g_rx_ring_tail = 0u;
  g_tx_busy = 0u;

  if (HAL_UART_Receive_DMA(&huart1, g_rx_dma_buffer, sizeof(g_rx_dma_buffer)) == HAL_OK)
  {
    __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
    __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_TC);
  }
}

void BspUart_PollRx(void)
{
  if (huart1.hdmarx == NULL)
  {
    return;
  }

  const uint16_t dma_write_index =
      (uint16_t)(UART_RX_DMA_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart1.hdmarx));

  while (g_rx_dma_read_index != dma_write_index)
  {
    ring_push(g_rx_dma_buffer[g_rx_dma_read_index]);
    g_rx_dma_read_index = (uint16_t)((g_rx_dma_read_index + 1u) % UART_RX_DMA_BUFFER_SIZE);
  }
}

bool BspUart_ReadByte(uint8_t *byte)
{
  if ((byte == NULL) || (g_rx_ring_head == g_rx_ring_tail))
  {
    return false;
  }

  *byte = g_rx_ring_buffer[g_rx_ring_tail];
  g_rx_ring_tail = (uint16_t)((g_rx_ring_tail + 1u) % UART_RX_RING_BUFFER_SIZE);
  return true;
}

bool BspUart_Write(const uint8_t *data, uint16_t size)
{
  if ((g_tx_busy != 0u) || (data == NULL) || (size > sizeof(g_tx_buffer)))
  {
    return false;
  }

  memcpy(g_tx_buffer, data, size);
  g_tx_busy = 1u;

  if (HAL_UART_Transmit_DMA(&huart1, g_tx_buffer, size) != HAL_OK)
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
