#include "PWM.h"

#define REDSHADE 0x17
#define GREENSHADE 0x02
#define BLUESHADE 0x0C
#define PWM_PERIO 240
#define PWM_OFF PORTB &=~(1<<PORTB1)
#define PWM_ON PORTB |=(1<<PORTB1)
#define PWM_START DDRB |=(1<<PORTB1)


static uint8_t PWM_DELTA = 0;
volatile uint8_t ocrValue = 0;
volatile uint8_t adjustLED = 0;


void setupPWM() {
	// Configuración para Timer0 canal A (LED rojo) y canal B (LED verde)
	DDRD |= (1 << PORTD6) | (1 << PORTD5); //Canal OCR0A y OCR0B
	OCR0A = 0x00;
	OCR0B = 0x00;
	TCCR0A |= (1 << WGM00) | (1 << WGM01) | (1 << COM0A1) | (1 << COM0A0) | (1 << COM0B1) | (1 << COM0B0); // Modo PWM invertido de 8 bits, Fast PWM en OC0A y OC0B
	TCCR0B |= (1 << CS02); // preescalador de 256; frecuencia de 122Hz
	//Setup del PWM por soft
	PWM_START;
	PWM_OFF;

}

void setRGBColor() {
	if (adjustLED <= 20){ //Se prende
		// Ajustar brillo del LED rojo
		ocrValue = OCR0A + REDSHADE;
		OCR0A = ocrValue;

		// Ajustar brillo del LED verde
		ocrValue = OCR0A + GREENSHADE;
		OCR0B = ocrValue;
		
		// Ajustar brillo del LED azul
		PWM_DELTA += BLUESHADE;
	}else{ //Se apaga
		// Ajustar brillo del LED rojo
		ocrValue = OCR0A - REDSHADE;
		OCR0A = ocrValue;

		// Ajustar brillo del LED verde
		ocrValue = OCR0A - GREENSHADE;
		OCR0B = ocrValue;

		// Ajustar brillo del LED azul
		PWM_DELTA -= BLUESHADE;
	}
}

void setupTimer1CTC(uint16_t outputCompare) { //outputCompare -> 1562 = 50ms; 3905 = 125ms;
	// Configurar Timer1 en modo CTC con prescaler 256
	TCCR1A = 0; // Desactivar las salidas en modo CTC
	TCCR1B |= (1 << WGM12) | (1 << CS12); // Modo CTC con prescaler 256
	OCR1B = outputCompare; // Interrupción cada 50 ms o cada 125 ms
	OCR1A = 499; // Interrupción cada 16 ms (60Hz)
	TIMSK1 |= (1 << OCIE1B) | (1 << OCIE1A); // Habilitar la interrupción de comparación de Timer1 Canal A (PWM software), Canal B (Interrupcion periodica)
}

// Interrupción del Timer1 (CTC)
ISR(TIMER1_COMPB_vect) {
	adjustLED++;
	setRGBColor(); // Alterar el estado del LED
	if(adjustLED>=40){
		adjustLED=0;
	}
}

// Interrupción del Timer1 para el PWM por Soft
ISR(TIMER1_COMPA_vect) {
	PWM_soft_Update();
}

//PWM por Software
void PWM_soft_Update(void){
	static uint8_t PWM_position = 0;
	
	if(++PWM_position>=PWM_DELTA){
		PWM_position=0;
		PWM_OFF;
	}else{
		if(PWM_position<PWM_DELTA){
			PWM_ON;
		}else{
			PWM_OFF;
		}
	}
}