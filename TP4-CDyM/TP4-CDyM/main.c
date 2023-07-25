#include <avr/io.h>
#include "PWM.h"

int main(void)
{
	setupPWM();
	setupTimer1CTC();
	sei();
    /* Replace with your application code */
    while (1) 
    {
    }
}

