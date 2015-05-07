/**
* @file		interrupciones.h
* @author	Manuel Caballero
* @date 	27/4/2015
* @brief 	Variables de las rutinas de interrupción.
* \copyright
* 	AqueronteBlog@gmail.com
*
* Este archivo es propiedad intelectual del blog Aqueronte,
* cuya direccion web, es la siguiente:
*
* 	http://unbarquero.blogspot.com/
*
* Se permite cualquier modificacion del archivo siempre y cuando
* se mantenga la autoria del autor.
*/
#include <msp430.h>
#include <stdint.h>


// PROTOTIPO DE FUNCIONES
void USCIA0_ISR(void) 	__attribute__((interrupt(USCI_A0_VECTOR)));
void TA0_ISR(void) 		__attribute__((interrupt(TIMER0_A1_VECTOR)));
void ADC10_ISR(void) 	__attribute__((interrupt(ADC10_VECTOR)));


// VARIABLES EXTERNAS
extern volatile	uint16_t	TX_NumPackets;
extern volatile uint16_t 	ADC10_Buffer [3];
extern volatile	uint16_t 	contADC;
extern volatile uint16_t 	BuffADC10_sigDATA;
