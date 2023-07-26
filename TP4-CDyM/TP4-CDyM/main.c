#include <avr/io.h>
#include "PWM.h"
#include "ADC.h"

int main(void)
{	
	uint16_t valorLDR;
	initADC();
	setupPWM();
	setupTimer1CTC(1562);  //1562 = 2seg parpadeo; 3905 = 5seg parpadeo;
	sei();
	
	while (1)
	{	
		valorLDR = leerADC_canal3();
		if (valorLDR == 1024){
			setupTimer1CTC(3905);
		} else {
			setupTimer1CTC(1562);
		}
		//si ADC detecta Luz minima (tapado) ==> setupTimer1CTC(3905);
		//si ADC detecta Luz ambiente (destapado) ==> setupTimer1CTC(1562);
	}
}
