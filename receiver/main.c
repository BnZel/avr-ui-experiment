#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>


void uart_init(void)
{
    UBRR0L = (unsigned char)(103 & 0xFF);         // set lower byte
    UBRR0H = (unsigned char)(103 >> 8);           // set higher byte

    // enable receiver
    UCSR0B |= (1 << RXEN0);                 // set to high reading
}

int main(void)
{
    DDRB |= (1 << PINB0);                               
    unsigned char data;

    uart_init();

    while (1)
    {
        while(! (UCSR0A & (1 << RXC0)));
        data = UDR0;

        if(data == 0b00000000){
            PORTB ^= (1 << PINB0); 
        }        
        else if(data == 0b11110000 ){
            PORTB |= (1 << PINB0); 
        }            
    }
}