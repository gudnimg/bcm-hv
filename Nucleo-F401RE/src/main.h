#ifndef MAIN_H
#define MAIN_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "vicor.h"

void MX_USART2_UART_Init(void);
void SystemClock_Config(void);
void MX_I2C1_Init(void);

#endif // MAIN_H