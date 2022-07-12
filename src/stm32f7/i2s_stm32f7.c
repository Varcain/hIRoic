#include <string.h>
#include "i2s_stm32f7.h"
#include "stm32746g_discovery_audio.h"
#include "stm32f7xx_hal.h"
#include "dsp.h"
#include "app_conf.h"

static AUDIO_DrvTypeDef *audio_drv;
static SAI_HandleTypeDef haudio_out_sai={0};
static SAI_HandleTypeDef haudio_in_sai={0};
static DMA_HandleTypeDef hdma_sai_tx;
static DMA_HandleTypeDef hdma_sai_rx;

#define HALF_BUF_SIZE (DSP_BUFFER_SIZE)
#define BUF_SIZE	(HALF_BUF_SIZE * 2)

unsigned int rxData[BUF_SIZE] ;
unsigned int txData[BUF_SIZE];
volatile unsigned int *pTx;
volatile unsigned int *pRx;
volatile unsigned int rxHalf;
volatile unsigned int txHalf;

int i;

volatile int rxDone;
volatile int txDone;

static struct i2s_drv_ops driver = {
	.init = i2s_stm32f7_init,
	.getRxBuffer = i2s_stm32f7_getRxBuffer,
	.getTxBuffer = i2s_stm32f7_getTxBuffer,
	.rxBufferRdy = i2s_stm32f7_rxBufferRdy,
	.xferCnt = i2s_stm32f7_xferCnt,
	.setRxCompleteCb = i2s_stm32f7_setRxCompleteCb,
	.startStream = i2s_stm32f7_startStream,
};

static i2s_driver_rxCompleteCb rxCompleteCallback;

static void i2s_stm32f7_hw_init(void);

void i2s_stm32f7_init(void)
{
	i2s_stm32f7_hw_init();
}

unsigned long i2s_stm32f7_getRxBuffer()
{
	if (rxHalf == 1) {
		pRx = rxData;
	} else {
		pRx = &rxData[HALF_BUF_SIZE] ;
	}

	return (unsigned long)pRx;
}

unsigned long i2s_stm32f7_getTxBuffer()
{
	if (txHalf == 0) {
		pTx = txData;
		txHalf = 1;
	} else {
		pTx = &txData[HALF_BUF_SIZE] ;
		txHalf = 0;
	}

	return (unsigned long)pTx;
}

int i2s_stm32f7_rxBufferRdy(void)
{
	if (rxDone) {
		rxDone = 0;
		return 1;
	} else {
		return 0;
	}
}

unsigned int i2s_stm32f7_xferCnt(void)
{
	return hdma_sai_rx.Instance->NDTR;
}

void i2s_stm32f7_setRxCompleteCb(i2s_driver_rxCompleteCb cb)
{
	rxCompleteCallback = cb;
}

void i2s_stm32f7_startStream(void)
{
	HAL_SAI_Receive_DMA(&haudio_in_sai, (uint8_t *)rxData, BUF_SIZE);
//	while (!rxDone);
//	rxDone = 0;
	pTx = rxData;
	HAL_SAI_Transmit_DMA(&haudio_out_sai, (uint8_t *)pTx, HALF_BUF_SIZE);
}

void i2s_stm32f7_pauseStream(void)
{
	HAL_SAI_DMAPause(&haudio_in_sai);
	HAL_SAI_DMAPause(&haudio_out_sai);
}

void i2s_stm32f7_resumeStream(void)
{
	HAL_SAI_DMAResume(&haudio_in_sai);
	HAL_SAI_DMAResume(&haudio_out_sai);
}

struct i2s_drv_ops* i2s_stm32f7_get(void)
{
	return &driver;
}

void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
	rxDone = 1;
	rxHalf = 1;
	if (rxCompleteCallback)
		rxCompleteCallback();
}

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
	rxDone = 1;
	rxHalf = 0;
	if (rxCompleteCallback)
		rxCompleteCallback();
}

void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{

}

void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
	HAL_SAI_Transmit_DMA(&haudio_out_sai, (uint8_t *) pTx, HALF_BUF_SIZE);
}

void AUDIO_OUT_SAIx_DMAx_IRQHandler(void)
{
	HAL_DMA_IRQHandler(haudio_out_sai.hdmatx);
}

void AUDIO_IN_SAIx_DMAx_IRQHandler(void)
{
	HAL_DMA_IRQHandler(haudio_in_sai.hdmarx);
}

static void i2s_stm32f7_hw_init(void)
{
	  RCC_PeriphCLKInitTypeDef rcc_ex_clk_init_struct;
	  haudio_out_sai.Instance = AUDIO_OUT_SAIx;
	  GPIO_InitTypeDef  gpio_init_structure;
	  uint8_t ret = AUDIO_ERROR;
	  uint32_t deviceid = 0x00;

	  HAL_RCCEx_GetPeriphCLKConfig(&rcc_ex_clk_init_struct);

	  /* Set the PLL configuration according to the audio frequency */
	  /* Configure PLLI2S prescalers */
	  /* PLLI2S_VCO: VCO_429M
	    I2S_CLK(first level) = PLLI2S_VCO/PLLI2SQ = 429/2 = 214.5 Mhz
	    I2S_CLK_x = I2S_CLK(first level)/PLLI2SDIVQ = 214.5/19 = 11.289 Mhz */
	  rcc_ex_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
	  rcc_ex_clk_init_struct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLI2S;
	  rcc_ex_clk_init_struct.PLLI2S.PLLI2SN = 429;
	  rcc_ex_clk_init_struct.PLLI2S.PLLI2SQ = 2;
	  rcc_ex_clk_init_struct.PLLI2SDivQ = 19;

	  HAL_RCCEx_PeriphCLKConfig(&rcc_ex_clk_init_struct);

	  /* Enable SAI clock */
	  AUDIO_OUT_SAIx_CLK_ENABLE();

	  /* Enable GPIO clock */
	  AUDIO_OUT_SAIx_MCLK_ENABLE();
	  AUDIO_OUT_SAIx_SCK_SD_ENABLE();
	  AUDIO_OUT_SAIx_FS_ENABLE();
	  /* CODEC_SAI pins configuration: FS, SCK, MCK and SD pins ------------------*/
	  gpio_init_structure.Pin = AUDIO_OUT_SAIx_FS_PIN;
	  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
	  gpio_init_structure.Pull = GPIO_NOPULL;
	  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
	  gpio_init_structure.Alternate = AUDIO_OUT_SAIx_FS_SD_MCLK_AF;
	  HAL_GPIO_Init(AUDIO_OUT_SAIx_FS_GPIO_PORT, &gpio_init_structure);

	  gpio_init_structure.Pin = AUDIO_OUT_SAIx_SCK_PIN;
	  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
	  gpio_init_structure.Pull = GPIO_NOPULL;
	  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
	  gpio_init_structure.Alternate = AUDIO_OUT_SAIx_SCK_AF;
	  HAL_GPIO_Init(AUDIO_OUT_SAIx_SCK_SD_GPIO_PORT, &gpio_init_structure);

	  gpio_init_structure.Pin =  AUDIO_OUT_SAIx_SD_PIN;
	  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
	  gpio_init_structure.Pull = GPIO_NOPULL;
	  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
	  gpio_init_structure.Alternate = AUDIO_OUT_SAIx_FS_SD_MCLK_AF;
	  HAL_GPIO_Init(AUDIO_OUT_SAIx_SCK_SD_GPIO_PORT, &gpio_init_structure);

	  gpio_init_structure.Pin = AUDIO_OUT_SAIx_MCLK_PIN;
	  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
	  gpio_init_structure.Pull = GPIO_NOPULL;
	  gpio_init_structure.Speed = GPIO_SPEED_HIGH;
	  gpio_init_structure.Alternate = AUDIO_OUT_SAIx_FS_SD_MCLK_AF;
	  HAL_GPIO_Init(AUDIO_OUT_SAIx_MCLK_GPIO_PORT, &gpio_init_structure);

	  /* Enable the DMA clock */
	  AUDIO_OUT_SAIx_DMAx_CLK_ENABLE();

	  /* Configure the hdma_saiTx handle parameters */
	  hdma_sai_tx.Init.Channel             = AUDIO_OUT_SAIx_DMAx_CHANNEL;
	  hdma_sai_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	  hdma_sai_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
	  hdma_sai_tx.Init.MemInc              = DMA_MINC_ENABLE;
	  hdma_sai_tx.Init.PeriphDataAlignment = AUDIO_OUT_SAIx_DMAx_PERIPH_DATA_SIZE;
	  hdma_sai_tx.Init.MemDataAlignment    = AUDIO_OUT_SAIx_DMAx_MEM_DATA_SIZE;
	  hdma_sai_tx.Init.Mode                = DMA_CIRCULAR;
	  hdma_sai_tx.Init.Priority            = DMA_PRIORITY_HIGH;
	  hdma_sai_tx.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
	  hdma_sai_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	  hdma_sai_tx.Init.MemBurst            = DMA_MBURST_SINGLE;
	  hdma_sai_tx.Init.PeriphBurst         = DMA_PBURST_SINGLE;

	  hdma_sai_tx.Instance = AUDIO_OUT_SAIx_DMAx_STREAM;

	  /* Associate the DMA handle */
	  __HAL_LINKDMA(&haudio_out_sai, hdmatx, hdma_sai_tx);

	  /* Deinitialize the Stream for new transfer */
	  HAL_DMA_DeInit(&hdma_sai_tx);

	  /* Configure the DMA Stream */
	  HAL_DMA_Init(&hdma_sai_tx);

	  /* SAI DMA IRQ Channel configuration */
	  HAL_NVIC_SetPriority(AUDIO_OUT_SAIx_DMAx_IRQ, AUDIO_OUT_IRQ_PREPRIO, 0);
	  HAL_NVIC_EnableIRQ(AUDIO_OUT_SAIx_DMAx_IRQ);

	  /* Enable SAI clock */
	  AUDIO_IN_SAIx_CLK_ENABLE();

	  /* Enable SD GPIO clock */
	  AUDIO_IN_SAIx_SD_ENABLE();
	  /* CODEC_SAI pin configuration: SD pin */
	  gpio_init_structure.Pin = AUDIO_IN_SAIx_SD_PIN;
	  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
	  gpio_init_structure.Pull = GPIO_NOPULL;
	  gpio_init_structure.Speed = GPIO_SPEED_FAST;
	  gpio_init_structure.Alternate = AUDIO_IN_SAIx_SD_AF;
	  HAL_GPIO_Init(AUDIO_IN_SAIx_SD_GPIO_PORT, &gpio_init_structure);

	  /* Enable Audio INT GPIO clock */
	  AUDIO_IN_INT_GPIO_ENABLE();
	  /* Audio INT pin configuration: input */
	  gpio_init_structure.Pin = AUDIO_IN_INT_GPIO_PIN;
	  gpio_init_structure.Mode = GPIO_MODE_INPUT;
	  gpio_init_structure.Pull = GPIO_NOPULL;
	  gpio_init_structure.Speed = GPIO_SPEED_FAST;
	  HAL_GPIO_Init(AUDIO_IN_INT_GPIO_PORT, &gpio_init_structure);

	  /* Enable the DMA clock */
	  AUDIO_IN_SAIx_DMAx_CLK_ENABLE();

	  /* Configure the hdma_sai_rx handle parameters */
	  hdma_sai_rx.Init.Channel             = AUDIO_IN_SAIx_DMAx_CHANNEL;
	  hdma_sai_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	  hdma_sai_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
	  hdma_sai_rx.Init.MemInc              = DMA_MINC_ENABLE;
	  hdma_sai_rx.Init.PeriphDataAlignment = AUDIO_IN_SAIx_DMAx_PERIPH_DATA_SIZE;
	  hdma_sai_rx.Init.MemDataAlignment    = AUDIO_IN_SAIx_DMAx_MEM_DATA_SIZE;
	  hdma_sai_rx.Init.Mode                = DMA_CIRCULAR;
	  hdma_sai_rx.Init.Priority            = DMA_PRIORITY_HIGH;
	  hdma_sai_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	  hdma_sai_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	  hdma_sai_rx.Init.MemBurst            = DMA_MBURST_SINGLE;
	  hdma_sai_rx.Init.PeriphBurst         = DMA_MBURST_SINGLE;

	  hdma_sai_rx.Instance = AUDIO_IN_SAIx_DMAx_STREAM;

	  /* Associate the DMA handle */
	  __HAL_LINKDMA(&haudio_in_sai, hdmarx, hdma_sai_rx);

	  /* Deinitialize the Stream for new transfer */
	  HAL_DMA_DeInit(&hdma_sai_rx);

	  /* Configure the DMA Stream */
	  HAL_DMA_Init(&hdma_sai_rx);

	  /* SAI DMA IRQ Channel configuration */
	  HAL_NVIC_SetPriority(AUDIO_IN_SAIx_DMAx_IRQ, AUDIO_IN_IRQ_PREPRIO, 0);
	  HAL_NVIC_EnableIRQ(AUDIO_IN_SAIx_DMAx_IRQ);

	  /* Audio INT IRQ Channel configuration */
	  HAL_NVIC_SetPriority(AUDIO_IN_INT_IRQ, AUDIO_IN_IRQ_PREPRIO, 0);
	  HAL_NVIC_EnableIRQ(AUDIO_IN_INT_IRQ);

	  /* Disable SAI peripheral to allow access to SAI internal registers */
	  __HAL_SAI_DISABLE(&haudio_out_sai);

	  /* Configure SAI_Block_x
	  LSBFirst: Disabled
	  DataSize: 16 */
	  haudio_out_sai.Init.AudioFrequency = 44100;
	  haudio_out_sai.Init.AudioMode = SAI_MODEMASTER_TX;
	  haudio_out_sai.Init.NoDivider = SAI_MASTERDIVIDER_ENABLED;
	  haudio_out_sai.Init.Protocol = SAI_FREE_PROTOCOL;
	  haudio_out_sai.Init.DataSize = SAI_DATASIZE_16;
	  haudio_out_sai.Init.FirstBit = SAI_FIRSTBIT_MSB;
	  haudio_out_sai.Init.ClockStrobing = SAI_CLOCKSTROBING_RISINGEDGE;
	  haudio_out_sai.Init.Synchro = SAI_ASYNCHRONOUS;
	  haudio_out_sai.Init.OutputDrive = SAI_OUTPUTDRIVE_ENABLED;
	  haudio_out_sai.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_1QF;

	  /* Configure SAI_Block_x Frame
	  Frame Length: 64
	  Frame active Length: 32
	  FS Definition: Start frame + Channel Side identification
	  FS Polarity: FS active Low
	  FS Offset: FS asserted one bit before the first bit of slot 0 */
	  haudio_out_sai.FrameInit.FrameLength = 64;
	  haudio_out_sai.FrameInit.ActiveFrameLength = 32;
	  haudio_out_sai.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;
	  haudio_out_sai.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
	  haudio_out_sai.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;

	  /* Configure SAI Block_x Slot
	  Slot First Bit Offset: 0
	  Slot Size  : 16
	  Slot Number: 4
	  Slot Active: All slot actives */
	  haudio_out_sai.SlotInit.FirstBitOffset = 0;
	  haudio_out_sai.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
	  haudio_out_sai.SlotInit.SlotNumber = 4;
	  haudio_out_sai.SlotInit.SlotActive = CODEC_AUDIOFRAME_SLOT_02;

	  HAL_SAI_Init(&haudio_out_sai);

	  /* Initialize SAI2 block B in SLAVE RX synchronous from SAI2 block A */
	  /* Initialize the haudio_in_sai Instance parameter */
	  haudio_in_sai.Instance = AUDIO_IN_SAIx;

	  /* Disable SAI peripheral to allow access to SAI internal registers */
	  __HAL_SAI_DISABLE(&haudio_in_sai);

	  /* Configure SAI_Block_x
	  LSBFirst: Disabled
	  DataSize: 16 */
	  haudio_in_sai.Init.AudioFrequency = 44100;
	  haudio_in_sai.Init.AudioMode = SAI_MODESLAVE_RX;
	  haudio_in_sai.Init.NoDivider = SAI_MASTERDIVIDER_ENABLED;
	  haudio_in_sai.Init.Protocol = SAI_FREE_PROTOCOL;
	  haudio_in_sai.Init.DataSize = SAI_DATASIZE_16;
	  haudio_in_sai.Init.FirstBit = SAI_FIRSTBIT_MSB;
	  haudio_in_sai.Init.ClockStrobing = SAI_CLOCKSTROBING_RISINGEDGE;
	  haudio_in_sai.Init.Synchro = SAI_SYNCHRONOUS;
	  haudio_in_sai.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLED;
	  haudio_in_sai.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_1QF;

	  /* Configure SAI_Block_x Frame
	  Frame Length: 64
	  Frame active Length: 32
	  FS Definition: Start frame + Channel Side identification
	  FS Polarity: FS active Low
	  FS Offset: FS asserted one bit before the first bit of slot 0 */
	  haudio_in_sai.FrameInit.FrameLength = 64;
	  haudio_in_sai.FrameInit.ActiveFrameLength = 32;
	  haudio_in_sai.FrameInit.FSDefinition = SAI_FS_CHANNEL_IDENTIFICATION;
	  haudio_in_sai.FrameInit.FSPolarity = SAI_FS_ACTIVE_LOW;
	  haudio_in_sai.FrameInit.FSOffset = SAI_FS_BEFOREFIRSTBIT;

	  /* Configure SAI Block_x Slot
	  Slot First Bit Offset: 0
	  Slot Size  : 16
	  Slot Number: 4
	  Slot Active: All slot active */
	  haudio_in_sai.SlotInit.FirstBitOffset = 0;
	  haudio_in_sai.SlotInit.SlotSize = SAI_SLOTSIZE_DATASIZE;
	  haudio_in_sai.SlotInit.SlotNumber = 4;
	  haudio_in_sai.SlotInit.SlotActive = CODEC_AUDIOFRAME_SLOT_02;

	  HAL_SAI_Init(&haudio_in_sai);

	  /* Enable SAI peripheral to generate MCLK */
	  __HAL_SAI_ENABLE(&haudio_out_sai);

	  /* Enable SAI peripheral */
	  __HAL_SAI_ENABLE(&haudio_in_sai);

	  /* wm8994 codec initialization */
	  deviceid = wm8994_drv.ReadID(AUDIO_I2C_ADDRESS);

	  if((deviceid) == WM8994_ID)
	  {
		  /* Reset the Codec Registers */
		  wm8994_drv.Reset(AUDIO_I2C_ADDRESS);
		  /* Initialize the audio driver structure */
		  audio_drv = &wm8994_drv;
		  ret = AUDIO_OK;
	  }
	  else
	  {
		  ret = AUDIO_ERROR;
	  }

	  if(ret == AUDIO_OK)
	  {
		  /* Initialize the codec internal registers */
		  audio_drv->Init(AUDIO_I2C_ADDRESS, INPUT_DEVICE_INPUT_LINE_1 | OUTPUT_DEVICE_HEADPHONE, 50, 44100);
	  }
}
