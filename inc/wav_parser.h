/*
 * wav_parser.h
 *
 *  Created on: Dec 10, 2015
 *      Author: varcain
 */

#ifndef INC_WAV_PARSER_H_
#define INC_WAV_PARSER_H_

typedef enum {
	WAVE_FORMAT_PCM = 0x0001,
	WAVE_FORMAT_IEEE_FLOAT = 0x0003,
	WAVE_FORMAT_ALAW = 0x0006,
	WAVE_FORMAT_MULAW = 0x0007,
	WAVE_FORMAT_EXTENSIBLE = 0xFFFE,
} Format_Code;

typedef struct E_WAV_Data {
	Format_Code formatCode;
	unsigned int nChannels;
	unsigned int nSamplesPerSec;
	unsigned int blockAlign;
	unsigned int bps;
	unsigned int n;
	void *samples;
} WAV_Data;

unsigned char *wav_parser_getWavBuff(void);
int wav_parser_parseWav(WAV_Data *wavData);

#endif /* INC_WAV_PARSER_H_ */
