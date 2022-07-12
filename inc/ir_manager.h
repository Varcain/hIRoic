/*
 * ir_manager.h
 *
 *  Created on: Dec 9, 2015
 *      Author: varcain
 */

#ifndef INC_IR_MANAGER_H_
#define INC_IR_MANAGER_H_

int ir_manager_init(void);
int ir_manager_getNextIR(int *ir, unsigned int *length, unsigned int *fs);

#endif /* INC_IR_MANAGER_H_ */
