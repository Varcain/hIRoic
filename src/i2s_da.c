#include "i2s_da.h"

static struct i2s_drv_ops *driver;

void i2s_set_driver(struct i2s_drv_ops *ops)
{
	driver = ops;
}

void i2s_init(void)
{
	if (driver->init)
		driver->init();
}

unsigned long i2s_getRxBuffer(void)
{
	if (driver->getRxBuffer)
		return (driver->getRxBuffer());
	else
		return (0);
}

unsigned long i2s_getTxBuffer(void)
{
	if (driver->getTxBuffer)
		return (driver->getTxBuffer());
	else
		return (0);
}

int i2s_rxBufferRdy(void)
{
	if (driver->rxBufferRdy)
		return (driver->rxBufferRdy());
	else
		return (-1);
}

unsigned int i2s_xferCnt(void)
{
	if (driver->xferCnt)
		return (driver->xferCnt());
	else
		return (0);
}

void i2s_setRxCompleteCb(i2s_driver_rxCompleteCb cb)
{
	if (driver->setRxCompleteCb)
		driver->setRxCompleteCb(cb);
}

void i2s_startStream(void)
{
	if (driver->startStream)
		driver->startStream();
}

void i2s_pauseStream(void)
{
	if (driver->pauseStream)
		driver->pauseStream();
}

void i2s_resumeStream(void)
{
	if (driver->resumeStream)
		driver->resumeStream();
}
