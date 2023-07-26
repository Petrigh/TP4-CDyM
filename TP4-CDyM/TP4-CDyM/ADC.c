#include "ADC.h"

void initADC() {
	// Configurar el pin del LED como salida.
	DDRB |= (1 << DDB5);
	
	// Configurar la referencia de voltaje del ADC a AVCC con ajuste a la izquierda.
	ADMUX |=  (0 << REFS1) | (1 << REFS0);
	ADMUX &= ~(1 << ADLAR);

	// Habilitar el ADC y configurar el preescalador a 64 (125 kHz a 8 MHz de frecuencia de reloj).
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0);
}

uint16_t leerADC_canal3() {
	// Seleccionar el canal 3 del ADC (PIN A3).
	ADMUX &= 0xF0; // Limpiar los bits MUX4:0 para configurarlos en 0.
	ADMUX |= 0x03; // Establecer los bits MUX4:0 en 011 para seleccionar el canal 3 (PIN A3).

	// Iniciar una conversión ADC.
	ADCSRA |= (1 << ADSC);

	// Esperar hasta que se complete la conversión.
	while ((ADCSRA & (1 << ADIF))==0);
	
	//Limpia el flag ADIF
	ADCSRA |= (1<<ADIF);

	// Leer y retornar el resultado de la conversión.
	uint8_t lowByte = ADCL;
	uint8_t highByte = ADCH;
	uint16_t valorADC = (highByte << 8) | lowByte;
	return valorADC;
	/*
	Una vez que la conversión ha finalizado, el resultado de la conversión se encuentra en los registros ADCL (registro ADC de menor orden) y 
	ADCH (registro ADC de mayor orden).

	Para obtener el resultado de la conversión de 10 bits (0 a 1023), primero se lee el valor de ADCL y luego se lee el valor de ADCH. El valor de ADCH 
	contiene los 2 bits más significativos de la conversión de 10 bits, mientras que el valor de ADCL contiene los 8 bits menos significativos. 
	Estos dos valores se combinan para obtener el resultado de la conversión de 10 bits.
	*/
}