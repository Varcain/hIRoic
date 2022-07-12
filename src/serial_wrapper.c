#include "serial_wrapper.h"
#include "stm32f7xx_hal.h"
#include "FreeRTOS.h"
#include "semphr.h"

static UART_HandleTypeDef uartHandle;
static GPIO_InitTypeDef gpioInit;

static SemaphoreHandle_t mutex;

#define TX_BUFF_SIZE	1024
#define CIRC_BUFF_SIZE	1024
static unsigned char txBuff[TX_BUFF_SIZE];
static unsigned char circBuff[CIRC_BUFF_SIZE];
static unsigned int head, tail;

void serial_txByte(unsigned char c);
void serial_txBuff(unsigned char *data, unsigned int length);

void serial_init(void)
{
	/* Console UART */
	__HAL_RCC_USART1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	gpioInit.Alternate = GPIO_AF7_USART1;
	gpioInit.Mode = GPIO_MODE_AF_PP;
	gpioInit.Pin = GPIO_PIN_9;
	gpioInit.Pull = GPIO_NOPULL;
	gpioInit.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &gpioInit);
	gpioInit.Pin = GPIO_PIN_7;
	HAL_GPIO_Init(GPIOB, &gpioInit);

	uartHandle.Instance          = USART1;
	uartHandle.Init.BaudRate     = 230400;
	uartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
	uartHandle.Init.StopBits     = UART_STOPBITS_1;
	uartHandle.Init.Parity       = UART_PARITY_NONE;
	uartHandle.Init.Mode         = UART_MODE_TX_RX;
	uartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

	if (HAL_UART_Init(&uartHandle) != HAL_OK) {
		while (1);
	}

	HAL_NVIC_SetPriority(USART1_IRQn, 2,0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	__HAL_UART_ENABLE_IT(&uartHandle, UART_IT_RXNE);

	mutex = xSemaphoreCreateMutex();
	if (mutex == NULL) {
		while (1);
	}
}

void serial_comm(Communication_Channel ch, unsigned char *data, unsigned int length)
{
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int chksum = 0;

	xSemaphoreTake(mutex, portMAX_DELAY);

	/* TODO: handle cases where length > 16-bit value */

	if (length > (TX_BUFF_SIZE - 1) ) {
		// todo: split buffer tx logic
		while (1);
	}

	/* Send the 2-byte token first */
	txBuff[i++] = 0xDA;
	txBuff[i++] = 0xBA;

	switch (ch) {
	case E_ID_CONSOLE:
		txBuff[i++] = 0x00; /* Console ID */
		break;
	case E_ID_SET_PIXEL:
		txBuff[i++] = 0x01; /* Set Pixel ID */
		break;
	case E_ID_FILL_AREA:
		txBuff[i++] = 0x02; /* Fill Area ID */
		break;
	default:
		return;
	}

	txBuff[i++] = length & 0xFF;
	txBuff[i++] = (length >> 8) & 0xFF;

	/* checksum the payload length */
	txBuff[i] = txBuff[i-2] ^ txBuff[i-1];
	i++;

	for (j = 0; j < length; j++) {
		txBuff[i++] = data[j];
	}

	for (j = 0; j < i; j++) {
		chksum ^= txBuff[j];
	}

	txBuff[i++] = chksum;

	serial_txBuff(txBuff, i);

	xSemaphoreGive(mutex);
}

void serial_txByte(unsigned char c)
{
	HAL_UART_Transmit(&uartHandle, &c, 1, 1000);
}

void serial_txBuff(unsigned char *data, unsigned int length)
{
	HAL_UART_Transmit(&uartHandle, data, length, 1000);
}

unsigned char serial_getChar(void)
{
	unsigned char c;

	if (head != tail) {
		c = circBuff[tail++];
		tail &= CIRC_BUFF_SIZE - 1;
	} else {
		c = 0;
	}

	return (c);
}

void serial_getBuff(unsigned char *data, unsigned int *length)
{
	int i;

	if (data == NULL)
		return;

	*length = head - tail;
	for (i = 0; i < head - tail; i++) {
		data[i] = circBuff[tail++];
		tail &= CIRC_BUFF_SIZE - 1;
	}
}

void USART1_IRQHandler(void)
{
	circBuff[head++] = uartHandle.Instance->RDR;
	head &= CIRC_BUFF_SIZE - 1;
}
