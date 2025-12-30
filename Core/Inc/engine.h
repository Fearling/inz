/*
 * engine.h
 *
 *  Created on: Dec 30, 2025
 *      Author: user
 */

#ifndef INC_ENGINE_H_
#define INC_ENGINE_H_

#include "main.h"

// move (number of 1/16 steps, direction)
// direction  1 | + steps to engine1
// direction  2 | + steps to engine2
// direction  3 | - steps to engine1
// direction  4 | - steps to engine2

void move(uint16_t, uint8_t);



#endif /* INC_ENGINE_H_ */
