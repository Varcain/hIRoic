#include "spi_da.h"

static struct spi_drv_ops *driver;

void spi_set_driver(struct spi_drv_ops *ops)
{
	driver = ops;
}

void spi_init(SPI_Enum spi)
{
	if (driver->init)
		driver->init(spi);
}

void spi_tx8(SPI_Enum spi, unsigned char txByte)
{
	if (driver->tx8)
		driver->tx8(spi, txByte);
}

void spi_tx16(SPI_Enum spi, unsigned short txShort)
{
	if (driver->tx16)
		driver->tx16(spi, txShort);
}

void spi_tx(SPI_Enum spi, unsigned char *txBytes, unsigned long length)
{
	if (driver->tx)
		driver->tx(spi, txBytes, length);
}

void spi_rx(SPI_Enum spi, unsigned char *rxData, unsigned long length)
{
	if (driver->rx)
		driver->rx(spi, rxData, length);
}

void spi_txrx(SPI_Enum spi, unsigned char *txData, unsigned char *rxData, unsigned long length)
{
	if (driver->txrx)
		driver->txrx(spi, txData, rxData, length);
}

void spi_cs_active(SPI_Enum spi)
{
	if (driver->cs_active)
		driver->cs_active(spi);
}

void spi_cs_inactive(SPI_Enum spi)
{
	if (driver->cs_inactive)
		driver->cs_inactive(spi);
}
