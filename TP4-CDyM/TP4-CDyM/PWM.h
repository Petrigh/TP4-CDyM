#ifndef PWM_H_
#define PWM_H_

#define F_CPU 16000000UL		// 16 MHz

#include <avr/io.h>
#include <avr/interrupt.h>

void setupPWM(void);
void setupTimer0CTC();
void setupTimer2CTC();

typedef enum {LONG, SHORT} apagado;
volatile apagado duracionApagado;

#endif /* PWM_H_ */