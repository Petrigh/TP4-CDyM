#include "PWM.h"

#define REDSHADE 0x066
#define GREENSHADE 0x66
#define BLUESHADE 0x66
#define PASO 250
#define PWM_ON PORTB &=~(1<<PORTB5)
#define PWM_OFF PORTB |=(1<<PORTB5)
#define PWM_START DDRB |=(1<<PORTB5)
typedef enum {UP, HOLD, DOWN, OFF} state;


volatile uint8_t fader = 0;
volatile uint8_t count = 0;
volatile state stateFlag;
volatile uint16_t PWM_DELTA = 255;
volatile uint8_t intensidadRoja = 255;
volatile uint8_t intensidadAzul = 255;
volatile uint8_t intensidadVerde = 255;
volatile uint8_t pasoRojo = 255/PASO;
volatile uint8_t pasoAzul = 255/PASO;
volatile uint8_t pasoVerde = 255/PASO;
volatile uint8_t ocrValue;

void PWM_soft_Update(void);

void setupPWM() {
	// Configuración para Timer1 canal A (LED azul) y canal B (LED verde)
	DDRB |= (1 << PORTB1) | (1 << PORTB2); //Canal OCR0A y OCR0B
	OCR1A = 0xFF;
	OCR1B = 0xFF;
	TCCR1A |= (1 << WGM10) | (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1); // Modo PWM no-invertido de 8 bits en OC1A y OC1B
	TCCR1B |= (1 << WGM12) | (1 << CS12); // preescalador de 256; frecuencia de 122Hz
	//configuracion Mef
	stateFlag = UP;
	duracionApagado = SHORT;
	//Setup del PWM por soft
	PWM_START;
	PWM_OFF;
}

void setRGBColor() {	
	switch(stateFlag){
		case UP:
			if(intensidadRoja > REDSHADE){
				// Ajustar brillo del LED rojo
				intensidadRoja -= pasoRojo;
				PWM_DELTA = intensidadRoja;
			}
			if(intensidadAzul > BLUESHADE){
				// Ajustar brillo del LED azul
				intensidadAzul -= pasoAzul;
				OCR1A = intensidadAzul;
			}
			if(intensidadVerde > GREENSHADE){
				// Ajustar brillo del LED verde
				intensidadVerde -= pasoVerde;
				OCR1B = intensidadVerde;
			}
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
			if(intensidadRoja < 255){
				// Ajustar brillo del LED rojo
				intensidadRoja += pasoRojo;
				PWM_DELTA = intensidadRoja;
			}
			if(intensidadAzul < 255){
				// Ajustar brillo del LED azul
				intensidadAzul += pasoAzul;
				OCR1A = intensidadAzul;
			}
			if(intensidadVerde < 255){
				// Ajustar brillo del LED verde
				intensidadVerde += pasoVerde;
				OCR1B = intensidadVerde;
			}
			
			if(!fader){
				stateFlag = OFF;
			}
		break;
		
		case OFF:
		
			TCCR0B &= ~(1<<CS01); //Apago Timer0
			TCCR1B &= ~(1<<CS12); //Apago Timer1
			PORTB |= (1<<PORTB1) | (1<<PORTB2) | (1<<PORTB5); //Apago LEDs
			
			switch(duracionApagado){
				case LONG:
					if(!fader){
						if(++count == 6){
							count = 0;
							stateFlag = UP;
							TCCR0B |= (1<<CS01); //Enciendo Timer0
							TCCR1B |= (1<<CS12); //Enciendo Timer1
							PORTB &= ~((1<<PORTB1) | (1<<PORTB2) | (1<<PORTB5)); //Enciendo LEDs
						}
					}
				break;
				case SHORT:
					count = 0;
					stateFlag = UP;
					TCCR0B |= (1<<CS01); //Enciendo Timer0
					TCCR1B |= (1<<CS12); //Enciendo Timer1
					PORTB &= ~((1<<PORTB1) | (1<<PORTB2) | (1<<PORTB5)); //Enciendo LEDs
				break;
				default:
					count = 0;
					stateFlag = UP;
					TCCR0B |= (1<<CS01); //Enciendo Timer0
					TCCR1B |= (1<<CS12); //Enciendo Timer1
					PORTB &= ~((1<<PORTB1) | (1<<PORTB2) | (1<<PORTB5)); //Enciendo LEDs
				break;
			}
		break;
	}
}

void setupTimer0CTC() {
	// Configurar Timer0 en modo CTC con prescaler 256
	TCCR0A |= (1 << WGM01);  // Modo CTC de operación
	TCCR0B = (1 << CS01); // Prescaler 8
	OCR0A = 10;  // Interrupción cada 160 us (6250Hz)
	TIMSK0 = (1 << OCIE0A); // Habilitar la interrupción de comparación de Timer0 Canal A (PWM software)
}


// Interrupción del Timer0 para el PWM por Soft
ISR(TIMER0_COMPA_vect) {
	PWM_soft_Update();
}



//PWM por Software
void PWM_soft_Update(void){
	static uint16_t pwm_position = 0;
	if (pwm_position > PWM_DELTA) {
		PWM_ON; // Establecer el pin en estado bajo
	} else {
		PWM_OFF; // Establecer el pin en estado alto
	}
	pwm_position = (pwm_position + 1) % 256; // Contador circular de 0 a 255
}



void setupTimer2CTC() {
	// Configurar Timer1 en modo CTC con prescaler 256
	TCCR2A |= (1 << WGM21); // Seteo Modo CTC
	TCCR2B |= (1 << CS22); // Prescaler 256
	OCR2A = 240; // Interrupción cada 2 ms (500Hz)
	TIMSK2 |= (1 << OCIE2A) ; // Habilitar la interrupción de comparación de Timer2 Canal A (Interrupcion periodica)
}

// Interrupción del Timer2 para la mef
ISR(TIMER2_COMPA_vect) {
	if(++fader>=250){
		fader = 0;
	}
	setRGBColor(); // Alterar el estado del LED
}