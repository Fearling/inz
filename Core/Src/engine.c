/*
 * engine.c
 *
 * Prosty sterownik silników krokowych na driverach A4988.
 * Poprawiona wersja: impulsowany jest pin STEP (nie DIR),
 * DIR ustawiane jest raz, przed rozpoczęciem ruchu.
 *
 * Motor 1: STEP1 (PA2), DIR1 (PA3), MS11 (PA10), MS21 (PB3), MS31 (PB5)
 * Motor 2: STEP2 (PB10), DIR2 (PB4), MS12 (PA8), MS22 (PA9), MS32 (PC7)
 * Wspólne: RESET (PC0), SLEEP (PB0), ENABLE (PC1) - jeden driver / oba drivery na wspólnej linii
 */

#include "engine.h"

/* Kierunki ruchu (możesz odwrócić, jeśli silnik kręci w złą stronę) */
#define DIR_FORWARD  GPIO_PIN_SET
#define DIR_REVERSE  GPIO_PIN_RESET

uint16_t position1 = 0, position2 = 0;

/* Ustawia mikrokrokowanie dla danego silnika.
 * step: 1, 2, 4, 8 lub 16 (mikrokroki na krok pełny)
 * engine: 1 lub 2
 */
void setStep(uint8_t step, uint8_t engine)
{
	GPIO_PinState ms1, ms2, ms3;

	switch(step){
		case 1:  ms1 = GPIO_PIN_RESET; ms2 = GPIO_PIN_RESET; ms3 = GPIO_PIN_RESET; break; // pełny krok
		case 2:  ms1 = GPIO_PIN_SET;   ms2 = GPIO_PIN_RESET; ms3 = GPIO_PIN_RESET; break; // 1/2
		case 4:  ms1 = GPIO_PIN_RESET; ms2 = GPIO_PIN_SET;   ms3 = GPIO_PIN_RESET; break; // 1/4
		case 8:  ms1 = GPIO_PIN_SET;   ms2 = GPIO_PIN_SET;   ms3 = GPIO_PIN_RESET; break; // 1/8
		case 16: ms1 = GPIO_PIN_SET;   ms2 = GPIO_PIN_SET;   ms3 = GPIO_PIN_SET;   break; // 1/16
		default: return;
	}

	if(engine == 1){
		HAL_GPIO_WritePin(MS11_GPIO_Port, MS11_Pin, ms1);
		HAL_GPIO_WritePin(MS21_GPIO_Port, MS21_Pin, ms2);
		HAL_GPIO_WritePin(MS31_GPIO_Port, MS31_Pin, ms3);
	} else if(engine == 2){
		HAL_GPIO_WritePin(MS12_GPIO_Port, MS12_Pin, ms1);
		HAL_GPIO_WritePin(MS22_GPIO_Port, MS22_Pin, ms2);
		HAL_GPIO_WritePin(MS32_GPIO_Port, MS32_Pin, ms3);
	}
}

/* Budzi drivery i włącza wyjścia. Wywołać raz na starcie programu. */
void engineInit(void)
{
	HAL_GPIO_WritePin(RESET_GPIO_Port, RESET_Pin, GPIO_PIN_SET);     /* wyjście z resetu */
	HAL_GPIO_WritePin(SLEEP_GPIO_Port, SLEEP_Pin, GPIO_PIN_SET);     /* wybudzenie drivera */
	HAL_GPIO_WritePin(ENNABLE_GPIO_Port, ENNABLE_Pin, GPIO_PIN_RESET); /* ENABLE jest aktywny stanem niskim */

	setStep(16, 1); /* domyślnie 1/16 kroku, zmień wg potrzeb */
	setStep(16, 2);

	HAL_Delay(1); /* chwila na ustabilizowanie się drivera */
}

/* Pojedynczy impuls STEP dla wskazanego silnika.
 * delay_us jest tu zamieniony na HAL_Delay w ms (min. praktyczna wartość to 1 ms,
 * jeśli potrzebujesz mikrosekund, użyj timera/DWT zamiast HAL_Delay).
 */
static void pulseStep(GPIO_TypeDef *port, uint16_t pin)
{
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
	HAL_Delay(100);
}

/* Rusza silnikiem o zadaną liczbę kroków (mikrokroków, zgodnie z ustawieniem setStep)
 * w zadanym kierunku.
 *
 * engine: 1 lub 2
 * dir:    1 = do przodu, 0 = do tyłu
 * steps:  liczba impulsów STEP
 */
void move(uint16_t steps, uint8_t engine, uint8_t dir)
{
	GPIO_TypeDef *step_port, *dir_port;
	uint16_t step_pin, dir_pin;

	if(engine == 1){
		step_port = STEP1_GPIO_Port; step_pin = STEP1_Pin;
		dir_port  = DIR1_GPIO_Port;  dir_pin  = DIR1_Pin;
	} else if(engine == 2){
		step_port = STEP2_GPIO_Port; step_pin = STEP2_Pin;
		dir_port  = DIR2_GPIO_Port;  dir_pin  = DIR2_Pin;
	} else {
		return;
	}

	/* Ustaw kierunek RAZ, przed rozpoczęciem impulsowania STEP */
	HAL_GPIO_WritePin(dir_port, dir_pin, dir ? DIR_FORWARD : DIR_REVERSE);
	HAL_Delay(1); /* krótka pauza wymagana przez A4988 po zmianie DIR (t_setup) */

	if(engine == 1){
		if(dir) position1 += steps; else position1 -= steps;
	} else {
		if(dir) position2 += steps; else position2 -= steps;
	}

	for(uint16_t i = 0; i < steps; i++){
		pulseStep(step_port, step_pin);
	}
}

void test(void){
	//pulseStep(STEP1_GPIO_Port, STEP1_Pin);
	HAL_GPIO_WritePin(STEP1_GPIO_Port, STEP1_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(STEP1_GPIO_Port, STEP1_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
}
