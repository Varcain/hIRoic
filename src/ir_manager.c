#include "wchar.h"
#include "ir_manager.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include "wav_parser.h"
#include "app_conf.h"

static FRESULT fres;
static char path[128];
static FATFS fs;
static DIR dir;
static FILINFO fno;
static FIL file;
static char lfn[_MAX_LFN + 1];

int ir_manager_init(void)
{
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;

	FATFS_LinkDriver(&SD_Driver, path);
	fres = f_mount(&fs, "", 0);
	if (fres != 0)
		return (-1);
	fres = f_opendir(&dir, "/");
	if (fres != 0)
		return (-1);

	do {
		f_readdir (&dir, &fno);
		printf("%s\r\n", fno.lfname);
	} while (fno.lfname[0] != 0);

	fres = f_closedir(&dir);
	if (fres != 0)
		return (-1);

	return (0);
}

int ir_manager_getNextIR(int *ir, unsigned int *length, unsigned int *fs)
{
	int res, i, j;
	unsigned int br;
	unsigned char *ptr;
	WAV_Data wavData;

	if (!dir.fs) {
		fres = f_findfirst(&dir, &fno, "/", "*");
	} else {
		fres = f_findnext(&dir, &fno);
	}

	if (fno.lfname[0] == 0) {
		fres = f_findfirst(&dir, &fno, "/", "*");
	}

	if (fres != FR_OK) {
		return (-1);
	}

	printf("Selected %s\r\n", fno.lfname);
	fres = f_open(&file, fno.lfname, FA_READ);
	if (fres != FR_OK) {
		return (-1);
	}
	printf("Opened...\r\n");
	if (fno.fsize > (IR_MAX_LEN * 4)) {
		printf("IR will be truncated\r\n");
		fres = f_read(&file, wav_parser_getWavBuff(), WAV_BUF_MAX_LEN, &br);
	} else {
		fres = f_read(&file, wav_parser_getWavBuff(), fno.fsize, &br);
	}
	if (fres != FR_OK) {
		return (-1);
	}
	res = wav_parser_parseWav(&wavData);
	if (res != 0) {
		return (-1);
	}

	if (wavData.formatCode == WAVE_FORMAT_PCM) {
		printf("PCM wave format\r\n");
	} else if (wavData.formatCode == WAVE_FORMAT_IEEE_FLOAT) {
		printf("IEEE FLOAT format\r\n");
		return (-1);
	} else if (wavData.formatCode == WAVE_FORMAT_ALAW) {
		printf("ALAW format\r\n");
		return (-1);
	} else if (wavData.formatCode == WAVE_FORMAT_MULAW) {
		printf("MULAW format\r\n");
		return (-1);
	} else if (wavData.formatCode == WAVE_FORMAT_EXTENSIBLE) {
		printf("Extensible format\r\n");
		return (-1);
	} else {
		printf("Unknown format\r\n");
		return (-1);
	}

	if (wavData.nChannels > 1) {
		printf("Unsupported number of channels %d\r\n", wavData.nChannels);
		return (-1);
	}

	ptr = wavData.samples;
	for (i = 0; i < wavData.n; i ++) {
		ir[i] = 0;
		for (j = 0; j < wavData.blockAlign; j++) {
			ir[i] += ptr[j + (i * wavData.blockAlign)] << (j * 8);
		}
	}

	/* Perform sign extension */
	if (wavData.bps < DSP_BITSIZE) {
		for (i = 0; i < wavData.n; i++) {
			if (ir[i] & (1 << (wavData.bps - 1))) {
				for (j = wavData.bps; j < DSP_BITSIZE; j++) {
					ir[i] |= (1 << j);
				}
			}
		}
	} else if (wavData.bps > DSP_BITSIZE) {
		printf("Unsupported bitsize\r\n");
		return (-1);
	}

	if (wavData.bps != DSP_BITSIZE) {
		unsigned char shift;
		if (DSP_BITSIZE > wavData.bps) {
			shift = DSP_BITSIZE - wavData.bps;
			for (i = 0; i < wavData.n; i++) {
				ir[i] = ir[i] * (1 << shift);
			}
		} else {
			shift = wavData.bps - DSP_BITSIZE;
			for (i = 0; i < wavData.n; i++) {
				ir[i] = ir[i] / (1 << shift);
			}
		}
	}

	*length = wavData.n;
	*fs = wavData.nSamplesPerSec;

	return (0);
}
