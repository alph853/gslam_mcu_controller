#ifndef BSP_TIME_H
#define BSP_TIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void BspTime_Init(void);
void BspTime_StartControlTick(void);
uint32_t BspTime_NowMs(void);
float BspTime_ControlDtSeconds(void);

#ifdef __cplusplus
}
#endif

#endif
