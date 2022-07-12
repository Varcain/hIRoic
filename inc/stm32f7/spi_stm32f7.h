/*
 * Driver Instance for SPI
 */

#ifndef _SPI_STM32F7_H_
#define _SPI_STM32F7_H_

#include "spi_da.h"

void spi_stm32f7_init(SPI_Enum spi);
void spi_stm32f7_tx16(SPI_Enum spi, unsigned short txShort);
void spi_stm32f7_tx8(SPI_Enum spi, unsigned char txByte);
void spi_stm32f7_tx(SPI_Enum spi, unsigned char *txBytes, unsigned long length);
void spi_stm32f7_rx(SPI_Enum spi, unsigned char *rxData, unsigned long length);
void spi_stm32f7_txrx(SPI_Enum spi, unsigned char *txData, unsigned char *rxData, unsigned long length);
void spi_stm32f7_cs_active(SPI_Enum spi);
void spi_stm32f7_cs_inactive(SPI_Enum spi);

struct spi_drv_ops* spi_stm32f7_get(void);

#endif /* _SPI_STM32F7_H_ */
