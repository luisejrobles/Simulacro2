/*
 * Simulacro2.c
 *
 * Created: 13/07/2017 05:05:51 p. m.
 * Author : LuisEduardo
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define UMBRAL_IN 1000

uint8_t ADC_read( void );
uint8_t ADC_result( uint8_t dato );
void ADC_init( void );
void clean_Vars( uint8_t *dato, uint16_t *miliseg );
void Timer0_init( void );
void UART0_init( void );
void UART0_puts( char *str );
void UART0_putchar( char dato );
char UART0_getchar( void );

uint16_t miliseg = 0;
uint8_t datos_adc[8000];
uint8_t verify = 0;

int main(void)
{	
	uint8_t dato;
	ADC_init();
    Timer0_init();
    UART0_init();
    while (1) 
    {
    	UART0_puts("\n\rPresione una tecla para capturar.");
    	UART0_getchar();
    	while(miliseg <= 8000 )
    	{
    		dato = ADC_read();
    		if( verify && ADC_result(dato) )
    		{
    			datos_adc[miliseg--] = dato;
    			verify--;
    		}
    	}
    	clean_Vars( &dato, &miliseg );
    }
}
void clean_Vars( uint8_t *dato, uint16_t *miliseg )
{
	*dato = 0;
	*miliseg = 0;
}
void ADC_init( void )
{
	ADMUX = (2<<REFS0)|(1<<ADLAR);	//INTERNAL REF, MSB
	ADCSRA = (1<<ADEN)|(7<<ADPS0); 	//128 PS
	ADCSRB = 0;						
	DIDR0 = 0;						//DIGITAL input dis
}
uint8_t ADC_read( void )
{
	ADCSRA |= (1<<ADSC);
	while( ADCSRA&(1<<ADSC) );
	return ADCH;	
}
uint8_t ADC_result( uint8_t dato )
{
	return ( ((dato*1.1)/1024) > UMBRAL_IN )? 1:0;
}
void Timer0_init( void )
{
	TCCR0A = (2<<WGM00);
	TCNT0 = 0;
	OCR0A = 250-1;
	TCCR0B = (2<<CS00); 	//8 PS
	TIMSK0 = (1<<OCIE0A);
	sei();
}
void UART0_init( void )
{
	UBRR0 = 103;
	UCSR0A = (1<<U2X0);
	UCSR0B = (1<<TXEN0)|(1<<RXEN0);
	UCSR0C = (3<<UCSZ00);
}
void UART0_puts( char *str )
{
	while( *str )
	{
		UART0_putchar( *str++ );
	}
}
void UART0_putchar( char dato )
{
	while( !(UCSR0A&(1<<UDRE0)) );
	UDR0 = dato;
}
char UART0_getchar( void )
{
	while( !(UCSR0A&(1<<RXC0)) );
	return UDR0;
}
ISR( TIMER0_COMPA_vect )
{
	miliseg++;
	verify++;
}
