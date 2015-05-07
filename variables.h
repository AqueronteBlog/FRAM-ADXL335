/**
* @file		variables.h
* @author	Manuel Caballero
* @date 	27/4/2015
* @brief 	Variables del sistema.
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


/**
 *  \brief     Variables Globales
 *  \details
 * 				- TX_NumPackets:	Dicha variable se emplea conjutamente en el servicio de interrupción
 *                  	 			de la transmisión de datos UART, para indicar el número de Bytes a
 *                  				enviar y que datos se van a enviar.
 */
volatile uint16_t TX_NumPackets = 0;


/**
 *  \brief     Variables Globales
 *  \details
 *  			- ADC10_Buffer [3]:	Dicha variable es encargada de almacenar los datos leídos del
 *  								módulo ADC10 para, posteriormente, ser transmitidos por la UART.
 */
volatile uint16_t ADC10_Buffer [3] = {0};


/**
 *  \brief     Variables Globales
 *  \details
 *  			- contADC:			Dicha variable es encargada de llevar la cuenta de los canales
 *  								del módulo ADC10 que deben ser leidos.
 */
volatile uint16_t contADC = 0;


/**
 *  \brief     Variables Globales
 *  \details
 *  			- BuffADC10_sigDATA:	Dicha variable se emplea junto a la variable ADC10_Buffer[3]
 *  									para indicar que dato debe ser enviado por la UART.
 */
volatile uint16_t BuffADC10_sigDATA = 0;
