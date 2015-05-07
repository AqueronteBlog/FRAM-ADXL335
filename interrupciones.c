/**
* @file		interrupciones.c
* @author	Manuel Caballero
* @date 	27/4/2015
* @brief 	Interrupciones del sistema.
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
#include "interrupciones.h"



/**
 *  \brief     TA0_ISR(void) TimerA0 (TA0) interrupt service routine
 *  \details   Cada, aproximadamente 0.1 segundos, se inicializa una petición de lectura
 *  		   del ADC10 de forma consequtiva de canales.
 *
 *			   El servicio de interrupción del TA0 estará deshabilitado hasta
 *			   que se realice la orden prevista y se envíen los datos a través de la UART.
 *
 *			   Una vez completada la orden, se volverá a habilitar desde el servicio de
 *			   interrupción de la UART.
 *
 *  \author    Manuel Caballero
 *  \version   0.0
 *  \date      27/4/2015
 */
void TA0_ISR(void)
{
  switch ( __even_in_range ( TA0IV, PC ) ){
	case	0x00:
	// Vector 0:	No interrupts
		break;

	case	0x02:
	// Vector 2:	Capture/Compare 1 ( TAxCCR1 )
		break;

	case	0x04:
	// Vector 4:	Capture/Compare 2 ( TAxCCR2 )
		break;

	case	0x06:
	// Vector 6:	Capture/Compare 3 ( TAxCCR3 )
		break;

	case	0x08:
	// Vector 8:	Capture/Compare 4 ( TAxCCR4 )
		break;

	case	0x0A:
	// Vector A:	Capture/Compare 5 ( TAxCCR5 )
		break;

	case	0x0C:
	// Vector C:	Capture/Compare 6 ( TAxCCR6 )
		break;

	case	0x0E:
	// Vector E:	Timer Overflow ( TAxCTL )
		TA0CTL  	&=	~( TAIFG | TAIE );   			// Reset flag y Servicio de interrupción OFF
		ADC10MCTL0 	 =	 ADC10SREF_0 | ADC10INCH_14;	// Secuencia de canales, empieza en A14
		contADC		 = 	 0;								// reset variable
		ADC10CTL0 	|=	 ADC10ENC | ADC10SC ;			// Start conversion
		break;

	default:
		break;
	}
}



/**
 *  \brief     void USCIA0_ISR(void) UART Tx & Rx interrupt service routine
 *  \details   Esta subrutina de interrupción se encarga de transmitir los datos leídos
 * 			   por el módulo ADC10.
 *
 *  \author    Manuel Caballero
 *  \version   0.0
 *  \date      27/4/2015
 */
void USCIA0_ISR(void)
{
	switch ( __even_in_range ( UCA0IV, 18 ) ){
	case	0x00:
	// Vector 0:	No interrupts
		break;

	case	0x02:
	// Vector 2:	UCRXIFG
		break;

	case	0x04:
	// Vector 4:	UCTXIFG

		if (TX_NumPackets < 6)													// Transmite los 5-Bytes restantes
		{
			if ( ( TX_NumPackets % 2 ) == 0 )
				UCA0TXBUF  =  ( ADC10_Buffer [ BuffADC10_sigDATA ] >> 8 );		// Transmite 1-Byte ( Parte Alta )
			else
			{
				UCA0TXBUF  =  ADC10_Buffer [ BuffADC10_sigDATA ];				// Transmite 1-Byte ( Parte Baja )
				BuffADC10_sigDATA++;
			}

			TX_NumPackets++;
		}
		else
		{
		// Se han transmitido todos los datos
			UCA0IE			&=	~UCTXIE;							// Tx interrupt disabled
		    TX_NumPackets  	 =	 0;			 						// Reset TX_NumPackets;
		    TA0CTL 			|= 	 TAIE;								// TA0 interrupt ON
		}
		break;

	case	0x06:
	// Vector 6:	UCSTTIFG
		break;

	case	0x08:
	// Vector 8:	UCTXCPTIFG
		break;

	default:
		break;
	}
}




/**
 *  \brief     void ADC10_ISR ADC10 interrupt service routine
 *  \details   Conversión realizada.
 *
 *  		   Lectura del dispositivo externo ADXL335 ( forma sequencial, sin repetición ):
 *
 *  		   	· Z-Acc:	P3.2 ( A14 )
 *  		   	· Y-Acc:	P3.1 ( A13 )
 *  		   	· X-Acc:	P3.0 ( A12 )
 *
 *  		   Una vez obtenidos dichos datos, se procede a transmitirlos
 *  		   por la UART.
 *
 *  \author    Manuel Caballero
 *  \version   0.0
 *  \date      27/4/2015
 */
void ADC10_ISR (void)
{
	switch ( __even_in_range ( ADC10IV, PC ) ){
	case	0x00:
	// Vector 0:	No interrupts
		break;

	case	0x02:
	// Vector 2:	ADC10OVIFG
		break;

	case	0x04:
	// Vector 4:	ADC10TOVIFG
		break;

	case	0x06:
	// Vector 6:	ADC10HIIFG
		break;

	case	0x08:
	// Vector 8:	ADC10LOIFG
		break;

	case	0x0A:
	// Vector A:	ADC10INIFG
		break;

	case	0x0C:
	// Vector C:	ADC10IFG0

		if ( contADC < 3 )									// Solo se almacenan los datos A14, A13 y A12
			ADC10_Buffer [ contADC ]	 =	 ADC10MEM0;
		else if ( contADC == 3 )
		{
		// Enviamos los datos por la UART
			ADC10IFG	&=	~ADC10IFG0;				// reset flag
			ADC10CTL0 	&=	~ADC10ENC;				// ADC10 OFF


			BuffADC10_sigDATA	 =	  0;
			TX_NumPackets	 	 =	  1;												//	reset variable
			UCA0TXBUF  		 	 =    ( ADC10_Buffer [ BuffADC10_sigDATA ] >> 8 );
			UCA0IE				|=	  UCTXIE;											// Tx interrupt enabled ( empieza a transmitir )
		}

		contADC++;
		break;

	default:
		break;
	}
}
