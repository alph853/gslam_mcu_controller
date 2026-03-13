#ifndef BSP_MOTOR_H
#define BSP_MOTOR_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void BspMotor_Init(void);
void BspMotor_Enable(bool enabled);
void BspMotor_SetLeftDuty(float duty);
void BspMotor_SetRightDuty(float duty);

#ifdef __cplusplus
}
#endif

#endif
