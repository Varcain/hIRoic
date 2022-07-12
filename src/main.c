#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "bsp.h"

#include "app_conf.h"

#include "spi_da.h"
#include "spi_stm32f7.h"
#include "i2s_da.h"
#include "i2s_stm32f7.h"
#include "audio_codec_da.h"
#include "dsp.h"
#include "ir_manager.h"
#include "serial_wrapper.h"

#include "cpu_utils.h"

#include "gfx.h"

uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute__((section(".RamData2")));

extern void xPortSysTickHandler(void);

static void prvRxCompleteCallback(void);

static void prvToggleLeds(void *pvParameters);
static void prvProcessAudio(void *pvParameters);
static void prvGraphicsTask(void *pvParameters);
static void prvInputsTask(void *pvParameters);

static TaskHandle_t hProcessAudio;

static int *in;
static int *out;


/* temp */
unsigned char inputBuff[1024];
unsigned int inputLength;
int irbuf[IR_MAX_LEN];
unsigned int irlen;
unsigned int irfs;

int main(void)
{
	bsp_boardInit();

	serial_init();

	printf("Build %s %s\n\r", __DATE__, __TIME__);

	dsp_init();

	/* Platform dependent stuff */
	i2s_set_driver(i2s_stm32f7_get());

	i2s_setRxCompleteCb(prvRxCompleteCallback);
	i2s_init();

	xTaskCreate(prvProcessAudio, "Process Audio", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 4, &hProcessAudio);
	vTaskSuspend(hProcessAudio);
	xTaskCreate(prvToggleLeds, "Toggle LEDs", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(prvGraphicsTask, "Graphics", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(prvInputsTask, "Inputs", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + 3, NULL);

	ir_manager_init();
	ir_manager_getNextIR(irbuf, &irlen, &irfs);

	dsp_loadIR(irbuf, irlen, 32, irfs);

	i2s_startStream();

	vTaskStartScheduler();

	return (0);
}

static void prvRxCompleteCallback(void)
{
	BaseType_t xYieldRequired;

	xYieldRequired = xTaskResumeFromISR(hProcessAudio);
	portYIELD_FROM_ISR(xYieldRequired);
}

static void prvProcessAudio(void *pvParameters)
{
	for (;;) {
		in = (int *)i2s_getRxBuffer();
		out = (int *)i2s_getTxBuffer();
		dsp_process(out, in, DSP_BUFFER_SIZE);
		vTaskSuspend(NULL);
	}
}

static void prvToggleLeds(void *pvParameters)
{
	for (;;) {
		vTaskDelay(portTICK_PERIOD_MS * 1000);
		bsp_toggleLed(0);
		bsp_toggleLed(1);
		printf("CPU usage: %d\r\n", osGetCPUUsage());
	}
}

static void prvGraphicsTask(void *pvParameters)
{
	gfxInit();
	font_t font = gdispOpenFont("DejaVuSans32_aa");

	gdispFillString(1, 1, "CPU usage: ", font, 1, 0);
	for(;;) {
		char buf[32];
		snprintf(buf, 32, "%d    ", osGetCPUUsage());
		gdispFillString(200, 1, buf, font, 1, 0);
		vTaskDelay(portTICK_PERIOD_MS * 500);
	}
}

void prvInputsTask(void *pvParameters)
{
	for(;;) {
		unsigned char c;
		c = serial_getChar();
		if (c == 'a') {
			i2s_pauseStream();
			ir_manager_getNextIR(irbuf, &irlen, &irfs);
			dsp_loadIR(irbuf, irlen, 32, irfs);
			i2s_resumeStream();
		}
		vTaskDelay(portTICK_PERIOD_MS * 25);
	}
}

void NMI_Handler(void)
{
	while (1);
}

void HardFault_Handler(void)
{
	while (1);
}

void MemManage_Handler(void)
{
	while (1);
}

void BusFault_Handler(void)
{
	while (1);
}

void UsageFault_Handler(void)
{
	while (1);
}

void DebugMon_Handler(void)
{
	while (1);
}


void SysTick_Handler(void)
{
	HAL_IncTick();
#if (INCLUDE_xTaskGetSchedulerState  == 1)
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
#endif  /* INCLUDE_xTaskGetSchedulerState */
		xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState  == 1)
	}
#endif  /* INCLUDE_xTaskGetSchedulerState */
}
