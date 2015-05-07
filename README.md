# 11-FRAM-ADXL335

## DESCRIPCIÓN:

Este programa consiste en realizar una lectura de datos del sensor externo ADXL335 ( acelerómetro de 3 ejes ), 
aproximadamente 0.1 segundos ( 10 Hz ).

Una vez obtenidos los datos, se envían al ordenador por medio de un par de dispositivos XBee usando la UART. 
El resto del tiempo, el MCU permanecerá en estado bajo consumo LPM3.

Este firmware está probado para el **MSP430FR5739**.


## Description:

This firmware shows how to get data from the ADXL335 device each 0.1 seconds.


This firmware was used with **MSP430FR5739**.


## Datos Técnicos / Technical Data
* Code Composer Studio, Version: 6.0.1.00104.
* C Compiler, MSP430 GCC GNU v4.9.1.


El proyecto completo se encuentra en la siguiente dirección: [AqueronteBlog Project](http://unbarquero.blogspot.com.es/2015/05/msp430-adxl335-msp430fr5739.html)
