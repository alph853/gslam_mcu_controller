#ifndef BSP_UART_H
#define BSP_UART_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void BspUart_Init(void);
void BspUart_PollRx(void);
bool BspUart_ReadByte(uint8_t *byte);
bool BspUart_Write(const uint8_t *data, uint16_t size);
void BspUart_OnTxComplete(void);

#ifdef __cplusplus
}
#endif

#endif
