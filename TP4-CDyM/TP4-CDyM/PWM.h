#ifndef PWM_H_
#define PWM_H_

#define F_CPU 16000000UL		// 16 MHz

#include <avr/io.h>
#include <avr/interrupt.h>

void setupPWM(void);
void setupTimer0CTC();
void setupTimer2CTC();

uint8_t haltTop;

#endif /* PWM_H_ */