/*
 * serial_wrapper.h
 *
 *  Created on: Dec 16, 2015
 *      Author: varcain
 */

#ifndef INC_SERIAL_WRAPPER_H_
#define INC_SERIAL_WRAPPER_H_

typedef enum {
	E_ID_CONSOLE,
	E_ID_SET_PIXEL,
	E_ID_FILL_AREA
} Communication_Channel;

void serial_init(void);
void serial_comm(Communication_Channel ch, unsigned char *data, unsigned int length);
unsigned char serial_getChar(void);
void serial_getBuff(unsigned char *data, unsigned int *length);

#endif /* INC_SERIAL_WRAPPER_H_ */
