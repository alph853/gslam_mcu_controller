#ifndef APP_ENTRY_H
#define APP_ENTRY_H

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @brief Initialize the application layer and start its runtime dependencies. 
 */
void App_Init(void);

/** 
 * @brief Run one control-tick iteration from the fast timer context. 
 */
void App_RunFastTick(void);

/** 
 * @brief Run one background iteration from the main loop context. 
 */
void App_RunSlowTick(void);

/**
 * @brief Forward a received UART byte into the application protocol path.
 * @param byte Received UART byte.
 */
void App_OnUartRxByte(unsigned char byte);

#ifdef __cplusplus
}
#endif

#endif
