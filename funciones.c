/**
* @file		funciones.c
* @author	Manuel Caballero
* @date 	27/4/2015
* @brief 	Funciones del sistema.
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
#include "funciones.h"


/**
 *  \brief     void conf_CLK (void)
 *  \details   Configura los relojes del MCU.
 *  				- Reloj Principal:  MCLK  = DCO    ~ 20 MHz.
 *  				- Reloj Secundario: SMCLK = DCO    ~ 20 MHz.
 *  				- Reloj Auxiliar: 	ACLK  = VLOCLK ~ 8.3 kHz ( datasheet msp430FR5739 5.14. Internal Very-Low-Power Low-Frequency Oscillator ).
 *  \author    Manuel Caballero
 *  \version   0.0
 *  \date      20/3/2015
 */
void conf_CLK (void)
{  
  CSCTL0_H	 =	CSKEY_H;				// Unlock register
  CSCTL1     =	DCORSEL + DCOFSEL_1;	// DC0 ~ 20 MHz
  CSCTL3	 =	DIVS_0 + DIVM_0;
  CSCTL2    |=	SELA_1;					// ACLK = VLOCLK
  CSCTL0_H 	 =	0x01;					// Lock register
}



/**
 *  \brief     void conf_WDT (void)
 *  \details   Desactiva el Watchdog del MCU.
 *  \author    Manuel Caballero
 *  \version   0.0
 *  \date      2/2/2015
 */
void conf_WDT (void)
{
  WDTCTL = WDTPW + WDTHOLD;     // Stop watchdog timer
}



/**
 *  \brief     void conf_IO (void)
 *  \details   Configura los pines del MCU a usar en este proyecto.
 *  				- Puerto 2: VDD_NTC & COM, UART.
 *  					- P2.7: Salida.
 *  					- P2.0: Salida  ( TX UART ).
 *  					- P2.1: Entrada ( RX UART ).
 *  				- Puerto 3: Acelerómetro (), LEDs.
 *  					- P3.0: Entrada Analógica ( X_out ).
 *  					- P3.1: Entrada Analógica ( Y_out ).
 *  					- P3.2: Entrada Analógica ( Z_out ).
 *  \author    Manuel Caballero
 *  \version   0.0
 *  \date      27/4/2015
 */
void conf_IO (void)
{
	P2DIR	|=	 BIT7;
	P2OUT	|=	 BIT7;					// ADXL335 enabled


	// P3.0 --> A12 ( Xout ADXL335), P3.1 --> A13 ( Yout ADXL335), P3.2 --> A14 ( Zout ADXL335)
	P3SEL1	|=	 BIT0 + BIT1 + BIT2;
	P3SEL0	|=	 BIT0 + BIT1 + BIT2;

	P3DIR 	&= 	~( BIT0 + BIT1 + BIT2 );


	// Configure UART pins P2.0 ( Tx ) & P2.1 ( Rx )
	P2SEL1 	|= 	 BIT0 + BIT1;
	P2SEL0 	&= 	~( BIT0 + BIT1 );
}



/**
 *  \brief     void conf_UART (void)
 *  \details   Configura el módulo UART a 115200 Baud Rate.
 *
 * 		· Reloj UART: SMCLK ~ 20 MHz.
 * 		· Buad Rate ~ 115200:
 *
 *     		N = f_BRCLK/BaudRate = 20MHz/115200 ~ 173.61 = {Parte entera} = 173
 *
 *      	N >= 16 -->  Oversampling ON (UCOS16 = 1)
 *
 * 		Por lo tanto:
 *
 *     		UCBRx = INT(N/16) = INT(173/16) = 10
 *     		UCBRFx = ROUND[((N/16) - INT(N/16))·16] = ROUND[((20MHz/115200)/16 - INT((20MHz/115200)/16))·16] ~ 13.61 = 14
 *
 * 		· Activamos UART
 * 	\pre	   Para ampliar información: Documentos slau272c.pdf, apartado 18 Enhanced Universal Serial Communication
 *			   Interface ( eUSCI ), concretamente 18.3.10 Setting a Baud Rate, se pueden encontrar datos tabulados a distintas frecuencias.
 * 	\pre	   El reloj SMCLK debe estar a 20 MHz.
 *  \author    Manuel Caballero
 *  \version   0.0
 *  \date      2/2/2015
 */
void conf_UART (void)
{
	UCA0CTLW0	|=	 UCSWRST;
	UCA0CTLW0	|=	 UCSSEL_2;					// SMCLK ~ 20MHz

	UCA0BRW		 =	 10;						// 20MHz 115200 (Oversampling)
	UCA0MCTLW	 =	 0xADE1;					// UCBRS = 0xAD , UCBRF = 14 ( 0x0E ) and UCOS16 = 1

	UCA0CTLW0	&=	~UCSWRST;					// **Initialize USCI state machine**

	// UCA0IFG		&=	~( UCRXIFG );				// reset flag
	UCA0IE		|=	 UCRXIE;					// Rx interrupt enabled
}



/**
 *  \brief     void conf_ADC10 (void)
 *  \details   Configura ADC10 para la lectura del sensor externo ADXL335 de forma
 *  		   sequencial sin repetición.
 *
 * 			   Según la hoja de características de esta familia
 *			   de microcontroladores, el documento: slau272c.pdf, la lectura binaria que
 *			   obtendremos en el registro ADC10MEM evndrá dada por la siguiente expresión:
 *
 *			     N_ADC = 1023·(V_IN - V_R-)/(V_R+ - V_R-)
 *
 *			   Teniendo en cuenta nuestra configuración: V_R+ = VDD ~ 3.6 V y V_R- = 0 V, tendremos
 *			   una lectura analógica tal y como se expresa a continuación:
 *
 *			     V_IN = N_ADC/284.17 = (6/1705)·N_ADC ~ N_ADC·0.003519
 *
 * 	\pre	   Voltajes de referencia:
 * 					· Vref+ = VDD ~ 3.6 V
 * 					· Vref- = Vss.
 * 	\pre	   Hay que asegurarse que el tiempo de muestreo sea mayor a 2.18us:
 * 				    · Reloj interno: ADC10OSC ( MODOSC ): 4.5 MHz
 * 				    · Tiempo muestreo en 64CLK: 64/(~4.5 MHz) ~ 14.2us -> Cumple la especificación.
 *  \author    Manuel Caballero
 *  \version   0.0
 *  \date      27/4/2015
 */
void conf_ADC10 (void)
{
  uint16_t degC = 0;

  ADC10CTL0 	&=	~ADC10ENC;
  ADC10CTL1  	 =   ADC10DIV_0 | ADC10SHP | ADC10SSEL_0 | ADC10CONSEQ_1;		// f_ADC10 ~ 4.5MHz
  ADC10CTL2		 =	 ADC10RES;													// ADC10 10-bit
  ADC10MCTL0 	 =	 ADC10SREF_0 | ADC10INCH_14;
  ADC10CTL0 	 =   ADC10SHT_4 | ADC10MSC | ADC10ON;							// 64/(4.5MHz), Sequence-of-channels, ADC10 enabled
  ADC10IE		|=	 ADC10IE0;													// Completed ADC10 conversion enabled

  for(degC = 30; degC > 0; degC-- );											// delay to allow reference to settle
}



/**
 *  \brief     void conf_TimerA (void)
 *  \details   Configura el timer TA0 como Up Mode.
 *
 * 			   El TA0 será encargado de despertar al MCU del estado
 * 			   bajo consumo en, aproximadamente 0.1 s (830/(~8.3kHz) ~ 0.1s).
 *
 * 	\pre	   El reloj ACLK debe estar a 8.3 kHz.
 *  \author    Manuel Caballero
 *  \version   0.0
 *  \date      27/4/2015
 */
void conf_TA0 (void)
{
  TA0CCR0 = 830;		                	// TAIFG on around ~ 0.1s
  TA0CTL  = TASSEL_1 + MC_1 + TACLR + TAIE;	// ACLK, upmode, TA0 interrupt ON
}
