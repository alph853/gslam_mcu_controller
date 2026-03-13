#ifndef APP_ENTRY_H
#define APP_ENTRY_H

#ifdef __cplusplus
extern "C" {
#endif

void App_Init(void);
void App_RunFastTick(void);
void App_RunSlowTick(void);
void App_OnUartRxByte(unsigned char byte);

#ifdef __cplusplus
}
#endif

#endif
