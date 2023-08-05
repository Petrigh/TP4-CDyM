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
			haltTop = 1; // 2 seg
		} else {
			haltTop = 6; // 5 seg
		}
	}
}
