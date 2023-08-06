#include "PWM.h"

#define REDSHADE 0x0001
#define GREENSHADE 0x01
#define BLUESHADE 0x01
#define PWM_PERDIOD 250
#define PWM_ON PORTB &=~(1<<PORTB5)
#define PWM_OFF PORTB |=(1<<PORTB5)
#define PWM_START DDRB |=(1<<PORTB5)
typedef enum {UP, HOLD, DOWN, OFF} state;


uint8_t fader = 0;
uint8_t count = 0;
state stateFlag;
volatile uint16_t PWM_DELTA = 255;
volatile uint8_t ocrValue = 0;
volatile uint8_t adjustLED = 0;
void PWM_soft_Update(void);

void setupPWM() {
	// Configuraci�n para Timer1 canal A (LED azul) y canal B (LED verde)
	DDRB |= (1 << PORTB1) | (1 << PORTB2); //Canal OCR0A y OCR0B
	OCR1A = 0xFF;
	OCR1B = 0xFF;
	TCCR1A |= (1 << WGM10) | (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1); // Modo PWM no-invertido de 8 bits en OC1A y OC1B
	TCCR1B |= (1 << WGM12) | (1 << CS12); // preescalador de 256; frecuencia de 122Hz
	//configuracion Mef
	stateFlag = UP;
	haltTop = 1;
	//Setup del PWM por soft
	PWM_START;
	PWM_OFF;
}

void setRGBColor() {	
	switch(stateFlag){
		case UP:
			
			// Ajustar brillo del LED azul
			ocrValue = OCR1A - BLUESHADE;
			OCR1A = ocrValue;
			// Ajustar brillo del LED verde
			ocrValue = OCR1B - GREENSHADE;
			OCR1B = ocrValue;
			// Ajustar brillo del LED rojo
			PWM_DELTA -= REDSHADE;
			
			if(!fader){
				stateFlag = HOLD;
			}
		break;
		
		case HOLD:
			if(!fader){
				if(++count == 2){
					stateFlag = DOWN;
					count=0;
				}
			}
		break;
		
		case DOWN:
			
			
			// Ajustar brillo del LED azul
			ocrValue = OCR1A + BLUESHADE;
			OCR1A = ocrValue;
			// Ajustar brillo del LED verde
			ocrValue = OCR1B + GREENSHADE;
			OCR1B = ocrValue;
			// Ajustar brillo del LED rojo
			PWM_DELTA += REDSHADE;
			
			
			if(!fader){
				stateFlag = OFF;
			}
		break;
		
		case OFF:
		
				TCCR0B &= ~(1<<CS01); //apago Timer0
				TCCR1B &= ~(1<<CS12); //apago Timer1
				PORTB |= (1<<PORTB1) | (1<<PORTB2) | (1<<PORTB5); //Enciendo LEDs
			
			if(!fader){
				if(++count == haltTop){
					stateFlag = UP;
					count = 0;
					TCCR0B |= (1<<CS01); //Enciendo Timer0
					TCCR1B |= (1<<CS12); //Enciendo Timer1
					PORTB &= ~((1<<PORTB1) | (1<<PORTB2) | (1<<PORTB5)); //Apago LEDs
				}
			}
		break;
	}
}

void setupTimer0CTC() {
	// Configurar Timer0 en modo CTC con prescaler 256
	TCCR0A |= (1 << WGM01);  // Modo CTC de operaci�n
	TCCR0B = (1 << CS01); // Prescaler 8
	OCR0A = 39;  // Interrupci�n cada 4 ms (250Hz)
	TIMSK0 = (1 << OCIE0A); // Habilitar la interrupci�n de comparaci�n de Timer0 Canal A (PWM software)
}


// Interrupci�n del Timer0 para el PWM por Soft
ISR(TIMER0_COMPA_vect) {
	PWM_soft_Update();
}



//PWM por Software
void PWM_soft_Update(void){
	static uint16_t pwm_position = 0;
	if (pwm_position < PWM_DELTA) {
		PWM_ON; // Establecer el pin en estado alto
	} else {
		PWM_OFF; // Establecer el pin en estado bajo
	}
	pwm_position = (pwm_position + 1) % 256; // Contador circular de 0 a 255
}



void setupTimer2CTC() {
	// Configurar Timer1 en modo CTC con prescaler 256
	TCCR2A |= (1 << WGM21); // Seteo Modo CTC
	TCCR2B |= (1 << CS22); // Prescaler 256
	OCR2A = 249; // Interrupci�n cada 4 ms (250Hz)
	TIMSK2 |= (1 << OCIE2A) ; // Habilitar la interrupci�n de comparaci�n de Timer2 Canal A (Interrupcion periodica)
}

// Interrupci�n del Timer2 para la mef
ISR(TIMER2_COMPA_vect) {
	if(++fader>=250){
		fader = 0;
	}
	setRGBColor(); // Alterar el estado del LED
}