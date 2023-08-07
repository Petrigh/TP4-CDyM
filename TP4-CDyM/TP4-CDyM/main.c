#include <avr/io.h>
#include <avr/interrupt.h>
#include "PWM.h"
#include "ADC.h"
#define F_CPU 16000000UL

int main(void)
{	
	initADC();
	setupPWM();
	setupTimer0CTC();
	setupTimer2CTC();
	sei();
	
	while (1)
	{
		valorLDR = leerADC_canal3();
		if (valorLDR <= 1000){
			duracionApagado = LONG; // 2 seg
		} else {
			duracionApagado = SHORT; // 5 seg
		}
	}
}
