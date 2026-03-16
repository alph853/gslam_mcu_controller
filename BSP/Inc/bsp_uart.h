#ifndef BSP_UART_H
#define BSP_UART_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Start the DMA-backed UART transport and clear local software buffers. */
void BspUart_Init(void);

/**
 * @brief Move any newly received DMA bytes into the software RX ring buffer.
 *
 * Call this regularly from a non-interrupt context before draining bytes with
 * `BspUart_ReadByte()`.
 */
void BspUart_PollRx(void);

/**
 * @brief Pop one byte from the software RX ring buffer.
 * @param byte Output storage for the next byte.
 * @retval `true` if a byte was returned.
 * @retval `false` if the ring buffer is empty or `byte` is null.
 */
bool BspUart_ReadByte(uint8_t *byte);

/**
 * @brief Queue one complete frame for UART transmission using DMA.
 * @param data Pointer to the frame bytes to send.
 * @param size Number of bytes to send.
 * @retval `true` if the transfer was started.
 * @retval `false` if UART TX is busy, the input is invalid, or the frame is too large.
 */
bool BspUart_Write(const uint8_t *data, uint16_t size);

/** @brief Release the UART TX busy flag after DMA transmit completion. */
void BspUart_OnTxComplete(void);

#ifdef __cplusplus
}
#endif

#endif
