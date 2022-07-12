/*
 * dsp.h
 *
 *  Created on: Nov 23, 2015
 *      Author: varcain
 */

#ifndef INC_DSP_H_
#define INC_DSP_H_

void dsp_init(void);
void dsp_process(int *out,  int *in, unsigned int size);
void dsp_convert_bitsize(int *out, int *in, unsigned char outBitSize, unsigned char inBitSize, unsigned int size);
void dsp_loadIR(int *ir, unsigned int length, unsigned int bitSize, unsigned int fs);

#endif /* INC_DSP_H_ */
