/*
 * UART0.c
 *
 * Created: 2022-08-09 오전 10:45:11
 *  Author: kccistc
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>	//strncmp, strcpy 등이 들어있다.
#include "UART0.h"

ISR(USART0_RX_vect)
{
	unsigned char data;
	
	data = UDR0;	// UART0의 HW register(UDR0)로 1 byte 읽어들인다.
	if (data == '\r' || data == '\n')	// 문장의 끝이면
	{
		rx_buff[i] = '\0';	// 문장의 끝을 가리키는 NULL
		i=0;				// 개선점 : circular queue (환형큐)로 개선을 해야한다.
							// 이렇게 짜면 새로운 메세지가 오면 덮어 쓴다.
		rx_ready_flag=1;	// 완전한  문장이 들어왔을 때 1 로 set 된다.
	}
	else
	{
		rx_buff[i++] = data;	// 1. rx_buff[i] = data  2. i++
	}
}
// 1. 전송속도: 9600bps - 총 byte(글자) 수: 9600 / 10 ==> 960자 (1글자 송수신 시간 : 약 1ms)
// 2. 비동기식, data 8bit, non-parity
// 3. Rx(수신) Interrupt 활성화
void init_uart0(void)
{
	UBRR0H = 0x00;
	UBRR0L = 207;	// 9600bps  pg219 표9-9
	
	UCSR0A |= (1 << U2X0);	// 2배속 통신
	UCSR0C |= 0x06;			// ASYNC(비동기) data 8bit non-parity
	
	// RXEN0 : UART0로부터 수신이 가능하도록 설정
	// TXEN0 : UART0로부터 송신이 가능하도록 설정
	// RXCIE0 : UART0로부터 1 byte 가 들어오면 RX Interrupt 띄워 달라 
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
}

// UART0 로 1 byte 를 보내는 함수
void UART0_transmit(uint8_t data)
{
	while ( !(UCSR0A & (1 << UDRE0)) )	// data 가 전송 중이면 data 가 다 전송될 때까지 기다린다.
		;	// no operation 아무런 action 도 취하지 않는다.
	UDR0 = data;	// HW 전송 register 에 data 를 쏴준다.
}

// 1. command 를 함수화 하여 led.c 에 배치한다.
// ledallon
// ledalloff
// ledalltoggle : 300ms 주기로 led 전체를 on/off 반복
extern int led_command;
void pc_command_processing()
{
	if (rx_ready_flag)	// if (rx_ready_flag >= 1)
	{
		rx_ready_flag=0;
		printf("%s\n", rx_buff);
		if (strncmp(rx_buff, "ledallon", strlen("ledallon")-1) == 0)
			PORTA = 0xff;
		if (strncmp(rx_buff, "ledalloff", strlen("ledalloff")-1) == 0)
			PORTA = 0x00;
		if (strncmp(rx_buff, "reset", strlen("reset")-1) == 0)
			led_command=0;
		if (strncmp(rx_buff, "ledalltoggle", strlen("ledalltoggle")-1) == 0)
			led_command=1;

		
	}
	switch(led_command)
	{
		case 1:
		ledalltoggle();
		break;
		break;
		case 2:
		break;
	}
}