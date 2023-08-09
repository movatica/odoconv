#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

/*
 * Uses Timer0 with Output Compare Match Interrupt to break down the input frequency
 * -> TO pin as external signal source
 * -> Counter0 Output Compare Match A interrupt
 */


// configuration
static const uint8_t INPUT_PIN = PB2;
static const uint8_t OUTPUT_PIN = PB3;

static const uint8_t DIVIDER = 12;
static const uint8_t OUTPUT_MS = 5;


// globals
volatile uint8_t output = 0;


// helper functions for readability
static inline void enable_pin(volatile uint8_t& port, const uint8_t pin)
{
    port |= _BV(pin);
}


static inline void disable_pin(volatile uint8_t& port, const uint8_t pin)
{
    port &= ~_BV(pin);
}


// pin change interrupt routine
// called for changes on all pins enabled in PCMSK
ISR(TIMER0_COMPA_vect) {
    output = OUTPUT_MS;
}


int main(void)
{
    cli();

    // disable unused features to reduce power consumption
    ADCSRA = 0; // disable ADC
    WDTCR = 0; // disable watchdog
    
    // configure PB2 (0xf - 0x4 = 0xb) as input, others as output
    DDRB = ~_BV(INPUT_PIN);
    
    // configure timer/counter0
    TIMSK = _BV(OCIE0A); // enable output compare 0A interrupt
    OCR0A = DIVIDER; // set output compare value
    TCCR0A = _BV(WGM01); // use CTC mode
    TCCR0B = _BV(CS00) | _BV(CS01) | _BV(CS02); // use external clock on TO pin, rising edge
    
    sei();
    
    while(1)
    {
        cli();
        if (output)
        {
            --output;
            enable_pin(PORTB, OUTPUT_PIN); // enable output pin
        }
        else
        {
            disable_pin(PORTB, OUTPUT_PIN); // disable output pin
        }
        sei();
        
        _delay_ms(1);
    }
}
