#include <avr/io.h>
#include "PWM.h"

int main(void)
{
	setupPWM();
	setupTimer1CTC(1562);  //1562 = 2seg parpadeo; 3905 = 5seg parpadeo;
	sei();
	
	while (1)
	{
		//si ADC detecta Luz minima (tapado) ==> setupTimer1CTC(3905);
		//si ADC detecta Luz ambiente (destapado) ==> setupTimer1CTC(1562);
	}
}
