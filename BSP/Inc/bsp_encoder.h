#ifndef BSP_ENCODER_H
#define BSP_ENCODER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void BspEncoder_Init(void);
int32_t BspEncoder_GetLeftCount(void);
int32_t BspEncoder_GetRightCount(void);

#ifdef __cplusplus
}
#endif

#endif
