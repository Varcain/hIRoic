/*
 * Driver Abstraction for I2S
 */

#ifndef _I2S_DA_H_
#define _I2S_DA_H_

typedef void (*i2s_driver_rxCompleteCb)(void);

typedef void (*i2s_driver_init)(void);
typedef unsigned long (*i2s_driver_getRxBuffer)(void);
typedef unsigned long (*i2s_driver_getTxBuffer)(void);
typedef int (*i2s_driver_rxBufferRdy)(void);
typedef unsigned int (*i2s_driver_xferCnt)(void);
typedef void (*i2s_driver_setRxCompleteCb)(i2s_driver_rxCompleteCb cb);
typedef void (*i2s_driver_startStream)(void);
typedef void (*i2s_driver_pauseStream)(void);
typedef void (*i2s_driver_resumeStream)(void);

struct i2s_drv_ops
{
	i2s_driver_init init;
	i2s_driver_getRxBuffer getRxBuffer;
	i2s_driver_getTxBuffer getTxBuffer;
	i2s_driver_rxBufferRdy rxBufferRdy;
	i2s_driver_xferCnt xferCnt;
	i2s_driver_setRxCompleteCb setRxCompleteCb;
	i2s_driver_startStream startStream;
	i2s_driver_pauseStream pauseStream;
	i2s_driver_resumeStream resumeStream;
};

void i2s_set_driver(struct i2s_drv_ops *ops);
void i2s_init(void);
unsigned long i2s_getRxBuffer(void);
unsigned long i2s_getTxBuffer(void);
int i2s_rxBufferRdy(void);
unsigned int i2s_xferCnt(void);
void i2s_setRxCompleteCb(i2s_driver_rxCompleteCb cb);
void i2s_startStream(void);
void i2s_pauseStream(void);
void i2s_resumeStream(void);

#endif /* _I2S_DA_H_ */
