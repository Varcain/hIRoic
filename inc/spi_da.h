/*
 * Driver Abstraction for SPI
 */

#ifndef _SPI_DA_H_
#define _SPI_DA_H_

typedef enum E_SPI  {
		E_SPI_AUDIO_CODEC,
		E_SPI_MMC,
} SPI_Enum;

typedef void (*spi_driver_init)(SPI_Enum spi);
typedef void (*spi_driver_tx8)(SPI_Enum spi, unsigned char txByte);
typedef void (*spi_driver_tx16)(SPI_Enum spi, unsigned short txShort);
typedef void (*spi_driver_tx)(SPI_Enum spi, unsigned char *txData, unsigned long length);
typedef void (*spi_driver_rx)(SPI_Enum spi, unsigned char *rxData, unsigned long length);
typedef void (*spi_driver_txrx)(SPI_Enum spi, unsigned char *txData, unsigned char *rxData, unsigned long length);
typedef void (*spi_driver_cs_active)(SPI_Enum spi);
typedef void (*spi_driver_cs_inactive)(SPI_Enum spi);

struct spi_drv_ops
{
	spi_driver_init init;
	spi_driver_tx8 tx8;
	spi_driver_tx16 tx16;
	spi_driver_tx tx;
	spi_driver_rx rx;
	spi_driver_txrx txrx;
	spi_driver_cs_active cs_active;
	spi_driver_cs_inactive cs_inactive;
};

void spi_set_driver(struct spi_drv_ops *ops);
void spi_init(SPI_Enum spi);
void spi_tx8(SPI_Enum spi, unsigned char txByte);
void spi_tx16(SPI_Enum spi, unsigned short txShort);
void spi_tx(SPI_Enum spi, unsigned char *txBytes, unsigned long length);
void spi_rx(SPI_Enum spi, unsigned char *rxData, unsigned long length);
void spi_txrx(SPI_Enum spi, unsigned char *txData, unsigned char *rxData, unsigned long length);
void spi_cs_active(SPI_Enum spi);
void spi_cs_inactive(SPI_Enum spi);

#endif /* _SPI_DA_H_ */
