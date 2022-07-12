#include <stdio.h>
#include <string.h>
#include "wav_parser.h"
#include "app_conf.h"

static unsigned char wavBuffer[WAV_BUF_MAX_LEN];

unsigned char *wav_parser_getWavBuff(void)
{
	return (wavBuffer);
}

int wav_parser_parseWav(WAV_Data *wavData)
{
	unsigned int ckId, ckSize, waveId, nChunks;
	unsigned int fmtCkSize, fmtSamplesPerSec, fmtAvgBytesPerSec, dataCkSize;
	unsigned short fmtTag, fmtNoChannels, fmtBlockAlign, fmtBps;
	unsigned int i = 0;
	unsigned char *samples = 0;

	memcpy(&ckId, &wavBuffer[i], sizeof(unsigned int));
	i += 4;
	if (ckId != 0x46464952) { /* RIFF */
		printf("Bad RIFF chunk ID\r\n");
		return (-1);
	}
	memcpy(&ckSize, &wavBuffer[i], sizeof(unsigned int));
	i += 4;
	nChunks = ckSize - 4;
	printf("nChunks %u\r\n", nChunks);
	memcpy(&waveId , &wavBuffer[i], sizeof(unsigned int));
	i += 4;
	if (waveId != 0x45564157) { /* WAVE */
		printf("Bad WAVE ID\r\n");
		return (-1);
	}

	while (i < nChunks) {
		memcpy(&ckId , &wavBuffer[i], sizeof(unsigned int));
		i += 4;

		if (ckId == 0x20746D66) { /* fmt */
			printf("fmt chunk\r\n");
			memcpy(&fmtCkSize , &wavBuffer[i], sizeof(unsigned int));
			i += 4;
			printf("Format chunk size: %u\r\n", fmtCkSize);

			memcpy(&fmtTag , &wavBuffer[i], sizeof(unsigned int));
			i += 2;
			printf("Format tag: %x\r\n", fmtTag);

			memcpy(&fmtNoChannels , &wavBuffer[i], sizeof(fmtNoChannels));
			i += 2;
			printf("Number of channels: %d\r\n", fmtNoChannels);

			memcpy(&fmtSamplesPerSec , &wavBuffer[i], sizeof(fmtSamplesPerSec));
			i += 4;
			printf("Samples per sec: %u\r\n", fmtSamplesPerSec);

			memcpy(&fmtAvgBytesPerSec , &wavBuffer[i], sizeof(fmtAvgBytesPerSec));
			i += 4;
			printf("Avg bytes per sec: %u\r\n", fmtAvgBytesPerSec);

			memcpy(&fmtBlockAlign , &wavBuffer[i], sizeof(fmtBlockAlign));
			i += 2;
			printf("Block align: %d\r\n", fmtBlockAlign);

			memcpy(&fmtBps , &wavBuffer[i], sizeof(fmtBps));
			i += 2;
			printf("Bps: %d\r\n", fmtBps);

			if (fmtCkSize  > 16) {
				printf("Unsupported format chunk size %u\r\n", fmtCkSize);
				return (-1);
			}

		} else if(ckId == 0x74636166) { /* fact */
			printf("fact chunk\r\n");
			return (-1);
		} else if(ckId == 0x61746164) { /* data */
			printf("data chunk\r\n");
			memcpy(&dataCkSize , &wavBuffer[i], sizeof(unsigned int));
			i += 4;
			printf("Data chunk size: %u\r\n", dataCkSize);
			samples = &wavBuffer[i];
			i += dataCkSize;
			break;
		} else {
			printf("unidentified chunk\r\n");
			return (-1);
		}
	}

	printf("Done parsing!\r\n");

	if (dataCkSize > IR_MAX_LEN) {
		printf("WAV truncated from %u to %u\r\n", dataCkSize, IR_MAX_LEN);
		dataCkSize = IR_MAX_LEN;
	}

	wavData->formatCode = fmtTag;
	wavData->nChannels = fmtNoChannels;
	wavData->nSamplesPerSec = fmtSamplesPerSec;
	wavData->blockAlign = fmtBlockAlign;
	wavData->bps = fmtBps;
	wavData->n = dataCkSize;
	wavData->samples = samples;

	return (0);
}
