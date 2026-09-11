#ifndef ENGINE_H_
#define ENGINE_H_

#include "stm32l4xx_hal.h"

/* --- Silnik 1 --- */
#define STEP1_Pin        GPIO_PIN_0
#define STEP1_GPIO_Port  GPIOA
#define DIR1_Pin         GPIO_PIN_1
#define DIR1_GPIO_Port   GPIOA
#define MS11_Pin         GPIO_PIN_10
#define MS11_GPIO_Port   GPIOA
#define MS21_Pin         GPIO_PIN_4
#define MS21_GPIO_Port   GPIOA
#define MS31_Pin         GPIO_PIN_5
#define MS31_GPIO_Port   GPIOB

/* --- Silnik 2 --- */
#define STEP2_Pin        GPIO_PIN_10
#define STEP2_GPIO_Port  GPIOB
#define DIR2_Pin         GPIO_PIN_4
#define DIR2_GPIO_Port   GPIOB
#define MS12_Pin         GPIO_PIN_8
#define MS12_GPIO_Port   GPIOA
#define MS22_Pin         GPIO_PIN_9
#define MS22_GPIO_Port   GPIOA
#define MS32_Pin         GPIO_PIN_7
#define MS32_GPIO_Port   GPIOC

/* --- Wspólne --- */
#define RESET_Pin        GPIO_PIN_0
#define RESET_GPIO_Port  GPIOC
#define ENNABLE_Pin      GPIO_PIN_1
#define ENNABLE_GPIO_Port GPIOC
#define SLEEP_Pin        GPIO_PIN_0
#define SLEEP_GPIO_Port  GPIOB

extern uint16_t position1, position2;

void engineInit(void);
void setStep(uint8_t step, uint8_t engine);
void move(uint16_t steps, uint8_t engine, uint8_t dir);
void test(void);

#endif /* ENGINE_H_ */
