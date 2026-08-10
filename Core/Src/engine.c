/*
 * engine.c
 *
 *  Created on: Dec 30, 2025
 *      Author: user
 */


#include "engine.h"

uint8_t c_step = 16;
uint16_t position1 = 0, position2 = 0;

void setStep(uint8_t step, uint8_t engine){
	switch(step){
		case 16:
			if(engine == 1 || engine == 3){
				HAL_GPIO_WritePin(GPIOB, MS21_Pin|MS11_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOA, MS31_Pin, GPIO_PIN_RESET);
			}
			if(engine == 2 || engine == 3){
				HAL_GPIO_WritePin(GPIOA, MS12_Pin|MS22_Pin|MS32_Pin, GPIO_PIN_RESET);
			}
			break;

		case 8:
			if(engine == 1 || engine == 3){
				HAL_GPIO_WritePin(GPIOB, MS11_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOB, MS21_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOA, MS31_Pin, GPIO_PIN_RESET);
			}
			if(engine == 2 || engine == 3){
				HAL_GPIO_WritePin(GPIOA, MS12_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOA, MS22_Pin|MS32_Pin, GPIO_PIN_RESET);
			}
			break;

		case 4:
			if(engine == 1 || engine == 3){
				HAL_GPIO_WritePin(GPIOB, MS11_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, MS21_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOA, MS31_Pin, GPIO_PIN_RESET);
			}
			if(engine == 2 || engine == 3){
				HAL_GPIO_WritePin(GPIOA, MS12_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOA, MS22_Pin|MS32_Pin, GPIO_PIN_RESET);
			}
			break;

		case 2:
			if(engine == 1 || engine == 3){
				HAL_GPIO_WritePin(GPIOB, MS21_Pin|MS11_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOA, MS31_Pin, GPIO_PIN_RESET);
			}
			if(engine == 2 || engine == 3){
				HAL_GPIO_WritePin(GPIOA, MS12_Pin|MS22_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOA, MS32_Pin, GPIO_PIN_RESET);
			}
			break;

		case 1:
			if(engine == 1 || engine == 3){
				HAL_GPIO_WritePin(GPIOB, MS21_Pin|MS11_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOA, MS31_Pin, GPIO_PIN_SET);
			}
			if(engine == 2 || engine == 3){
				HAL_GPIO_WritePin(GPIOA, MS12_Pin|MS22_Pin|MS32_Pin, GPIO_PIN_SET);
			}
			break;
	}
}
void move(uint16_t distance, uint8_t dir){
	uint16_t distance_left = 0, steps = 0;
	switch(dir) {
		case 1:
			position1 = position1 + distance;
			setStep(16, 1);
			HAL_Delay(5);
			steps = distance / 16;
			distance_left = distance % 16;
			for(uint8_t i = 0; steps < i; i++){
				HAL_GPIA_WritePin(GPIOA, DIR1_Pin, GPIO_PIN_SET);
				HAL_Delay(5);
				HAL_GPIA_WritePin(GPIOA, DIR1_Pin, GPIO_PIN_RESET);
				HAL_Delay(5);
			}
			steps = distance_left /4;
			distance_left = distance_left % 4;
			setStep(4, 1);
			HAL_Delay(5);
			for(uint8_t i = 0; steps < i; i++){
				HAL_GPIA_WritePin(GPIOA, DIR1_Pin, GPIO_PIN_SET);
				HAL_Delay(5);
				HAL_GPIA_WritePin(GPIOA, DIR1_Pin, GPIO_PIN_RESET);
				HAL_Delay(5);
			}

			steps = distance_left;
			setStep(1, 1);
			HAL_Delay(5);
			for(uint8_t i = 0; steps < i; i++){
				HAL_GPIA_WritePin(GPIOA, DIR1_Pin, GPIO_PIN_SET);
				HAL_Delay(5);
				HAL_GPIA_WritePin(GPIOA, DIR1_Pin, GPIO_PIN_RESET);
				HAL_Delay(5);
			}
			break;
		case 2:
			position2 = position2 + distance;
			setStep(16, 2);
			HAL_Delay(5);
			steps = distance / 16;
			distance_left = distance % 16;
			for(uint8_t i = 0; steps < i; i++){
				HAL_GPIA_WritePin(GPIOA, DIR2_Pin, GPIO_PIN_SET);
				HAL_Delay(5);
				HAL_GPIA_WritePin(GPIOA, DIR2_Pin, GPIO_PIN_RESET);
				HAL_Delay(5);
			}
			steps = distance_left /4;
			distance_left = distance_left % 4;
			setStep(4, 2);
			HAL_Delay(5);
			for(uint8_t i = 0; steps < i; i++){
				HAL_GPIA_WritePin(GPIOA, DIR2_Pin, GPIO_PIN_SET);
				HAL_Delay(5);
				HAL_GPIA_WritePin(GPIOA, DIR2_Pin, GPIO_PIN_RESET);
				HAL_Delay(5);
			}

			steps = distance_left;
			setStep(1, 2);
			HAL_Delay(5);
			for(uint8_t i = 0; steps < i; i++){
				HAL_GPIA_WritePin(GPIOA, DIR2_Pin, GPIO_PIN_SET);
				HAL_Delay(5);
				HAL_GPIA_WritePin(GPIOA, DIR2_Pin, GPIO_PIN_RESET);
				HAL_Delay(5);
			}
			break;
		case 3:
			position1 = position1 - distance;
			setStep(16, 3);
			HAL_Delay(5);
			steps = distance / 16;
			distance_left = distance % 16;
			for(uint8_t i = 0; steps < i; i++){
				HAL_GPIA_WritePin(GPIOA, DIR1_Pin, GPIO_PIN_SET);
				HAL_Delay(5);
				HAL_GPIA_WritePin(GPIOA, DIR1_Pin, GPIO_PIN_RESET);
				HAL_Delay(5);
			}
			steps = distance_left /4;
			distance_left = distance_left % 4;
			setStep(4, 3);
			HAL_Delay(5);
			for(uint8_t i = 0; steps < i; i++){
				HAL_GPIA_WritePin(GPIOA, DIR1_Pin, GPIO_PIN_SET);
				HAL_Delay(5);
				HAL_GPIA_WritePin(GPIOA, DIR1_Pin, GPIO_PIN_RESET);
				HAL_Delay(5);
			}

			steps = distance_left;
			setStep(1, 3);
			HAL_Delay(5);
			for(uint8_t i = 0; steps < i; i++){
				HAL_GPIA_WritePin(GPIOA, DIR1_Pin, GPIO_PIN_SET);
				HAL_Delay(5);
				HAL_GPIA_WritePin(GPIOA, DIR1_Pin, GPIO_PIN_RESET);
				HAL_Delay(5);
			}
			break;
		case 4:
			position2 = position2 - distance;
			setStep(16, 4);
			HAL_Delay(5);
			steps = distance / 16;
			distance_left = distance % 16;
			for(uint8_t i = 0; steps < i; i++){
				HAL_GPIA_WritePin(GPIOA, DIR2_Pin, GPIO_PIN_SET);
				HAL_Delay(5);
				HAL_GPIA_WritePin(GPIOA, DIR2_Pin, GPIO_PIN_RESET);
				HAL_Delay(5);
			}
			steps = distance_left /4;
			distance_left = distance_left % 4;
			setStep(4, 4);
			HAL_Delay(5);
			for(uint8_t i = 0; steps < i; i++){
				HAL_GPIA_WritePin(GPIOA, DIR2_Pin, GPIO_PIN_SET);
				HAL_Delay(5);
				HAL_GPIA_WritePin(GPIOA, DIR2_Pin, GPIO_PIN_RESET);
				HAL_Delay(5);
			}

			steps = distance_left;
			setStep(1, 4);
			HAL_Delay(5);
			for(uint8_t i = 0; steps < i; i++){
				HAL_GPIA_WritePin(GPIOA, DIR2_Pin, GPIO_PIN_SET);
				HAL_Delay(5);
				HAL_GPIA_WritePin(GPIOA, DIR2_Pin, GPIO_PIN_RESET);
				HAL_Delay(5);
			}
			break;

	}


}
