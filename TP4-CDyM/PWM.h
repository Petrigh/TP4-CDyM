#ifndef PWM_H_
#define PWM_H_

#define F_CPU 16000000UL		// 16 MHz

#include <avr/io.h>
#include <avr/interrupt.h>

void setupPWM(void);
void setupTimer1CTC(void);

#endif /* PWM_H_ */