#include "PWM.h"


const uint8_t REDSHADE = 0x17;
const uint8_t GREENSHADE = 0x02;
const uint8_t BLUESHADE = 0x0C;
volatile uint32_t interruptCount = 0;
volatile uint8_t ocrValue = 0;
volatile uint8_t adjustLED = 0;

void setupPWM() {
	// Configuración para Timer0 canal A (LED rojo) y canal B (LED verde)
	DDRD |= (1 << PORTD6) | (1 << PORTD5); //Canal OCR0A y OCR0B
	OCR0A = 0x00;
	OCR0B = 0x00;
	TCCR0A |= (1 << WGM00) | (1 << WGM01) | (1 << COM0A1) | (1 << COM0A0) | (1 << COM0B1) | (1 << COM0B0); // Modo PWM invertido de 8 bits, Fast PWM en OC0A y OC0B
	TCCR0B |= (1 << CS02); // preescalador de 256; frecuencia de 122Hz
	
	// PROBABLEMENTE SE BORRE Y SE HAGA POR SOFT
	 // Configuración para Timer1 (LED azul)
	 DDRB |= (1 << PORTB1); //Canal OCR1A
	 TCCR1A |= (1 << WGM12) | (1 << WGM10) | (1 << COM1A1) | (1 << COM1A0); // Modo PWM de 8 bits (modo 5) para Timer1, Fast PWM en OC1A
	 TCCR1B |= (1 << CS12); // preescalador de 256; frecuencia de 122Hz

}

void setRGBColor() {
	if (adjustLED >= 10){ //Se prende
		// Ajustar brillo del LED rojo
		ocrValue = OCR0A + REDSHADE;
		OCR0A = ocrValue;

		// Ajustar brillo del LED verde
		ocrValue = OCR0A + GREENSHADE;
		OCR0B = ocrValue;

		//PROBABLEMENTE SE HAGA POR SOFT
		// Ajustar brillo del LED azul
		ocrValue = OCR1A + BLUESHADE;
		OCR1A = ocrValue;
	}else{ //Se apaga
		// Ajustar brillo del LED rojo
		ocrValue = OCR0A - REDSHADE;
		OCR0A = ocrValue;

		// Ajustar brillo del LED verde
		ocrValue = OCR0A - GREENSHADE;
		OCR0B = ocrValue;

		//PROBABLEMENTE SE HAGA POR SOFT
		// Ajustar brillo del LED azul
		ocrValue = OCR1A - BLUESHADE;
		OCR1A = ocrValue;
	}
}

/*
Fijarse como solucionar el parpadeo, teniendo 20 colores, en 2 segundos debe cambiar de color cada 50ms (ROSA -> NEGRO -> ROSA),
Pensaba en interrupcion de timer1 por ctc, pero el canal A esta ocupado y nose usar el canal B,
hay que despues calcular para 5 seg, que debe cambiar cada 125ms.

Posiblemente haya que generar un PWM por software, asi liberamos el Timer1 y generamos  la interrupcion en modo CTC
*/

void setupTimer1CTC() {
	// Configurar Timer1 en modo CTC con prescaler 256
	TCCR1A = 0; // Desactivar las salidas en modo CTC
	TCCR1B = (1 << WGM12) | (1 << CS12); // Modo CTC con prescaler 256
	OCR1A = 6250; // Establecer el valor de comparación para generar una interrupción cada 100 ms
	TIMSK1 = (1 << OCIE1A); // Habilitar la interrupción de comparación de Timer1
}

// Rutina de interrupción del Timer1 (CTC)
ISR(TIMER1_COMPA_vect) {
	interruptCount++;  // Incrementar el contador de interrupciones

	// Si el contador de interrupciones llega a 2 (2 segundos), hacer que el LED parpadee
	if (interruptCount >= 2) {
		adjustLED++;
		if(adjustLED>=20){
			adjustLED=0;
		}
		setRGBColor(); // Alternar el estado del LED
		interruptCount = 0;  // Reiniciar el contador de interrupciones
	}
}