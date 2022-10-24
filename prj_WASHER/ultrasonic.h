/*
 * ultrasonic.h
 *
 * Created: 2022-08-10 오후 4:24:05
 *  Author: kccistc
 */ 


#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define FALSE 0
#define TRUE 1

#define US_TCNT	TCNT1	// timer1 16bit

#define TRIG 4
#define TRIG_DDR	DDRG
#define TRIG_PORT	PORTG

#define ECHO 4
#define ECHO_DDR	DDRE
#define ECHO_PIN	PINE

#endif /* ULTRASONIC_H_ */