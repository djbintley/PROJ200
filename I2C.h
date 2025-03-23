#ifndef I2C_H
#define I2C_H

#include "stm32f4xx.h"

#define IMU_Addr 0x69 << 1

// Simple functions
void IMU_INIT(void);
int MovementDetected(void);
int Absolute(int x);

#endif
