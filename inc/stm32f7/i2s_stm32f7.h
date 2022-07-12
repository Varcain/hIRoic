/*
 * i2s_stm32f7.h
 *
 *  Created on: Oct 29, 2015
 *      Author: limak
 */

#ifndef INC_I2S_STM32F7_H_
#define INC_I2S_STM32F7_H_

#include "i2s_da.h"

void i2s_stm32f7_init(void);
unsigned long i2s_stm32f7_getRxBuffer(void);
unsigned long i2s_stm32f7_getTxBuffer(void);
int i2s_stm32f7_rxBufferRdy(void);
unsigned int i2s_stm32f7_xferCnt(void);
void i2s_stm32f7_setRxCompleteCb(i2s_driver_rxCompleteCb cb);
void i2s_stm32f7_startStream(void);
void i2s_stm32f7_pauseStream(void);
void i2s_stm32f7_resumeStream(void);

struct i2s_drv_ops* i2s_stm32f7_get(void);

#endif /* INC_I2S_STM32F7_H_ */
