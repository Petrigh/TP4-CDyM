

#ifndef ADC_H_
#define ADC_H_

#define MIN_TIME 5000 // en milisegundos (ms)
#define MAX_TIME 2000 // en milisegundos (ms)

#include <avr/io.h>

void initADC(void); // Función para inicializar el ADC.

uint16_t leerADC_canal3(void); // Función para leer el valor del ADC en el canal 3.


#endif /* ADC_H_ */