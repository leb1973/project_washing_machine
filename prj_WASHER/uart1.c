/*
 * uart1.c
 *
 * Created: 2022-08-08 오후 9:23:06
 *  Author: sikwon
 */ 

#include "uart1.h"

volatile uint8_t rx1String[64]={0,};
volatile uint8_t rx1ReadyFlag=0;
extern int led_command;		// 이 file은 다른 곳에 있다. extern
extern volatile uint8_t rx_ready_flag;

ISR(USART0_RX_vect)
{
	UART1_ISR_Receive();
}

void init_UART1()
{
	UCSR1B |= ((1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1));
	UCSR1A |= (1<<U2X1);
	UBRR1H = 0;
	UBRR1L = 207; // 9600 buadrate
}


void UART1_transmit(unsigned char data)
{	
	while ( !(UCSR1A & (1<<UDRE1)) );
	UDR1 = data;
}


void UART1_print_string(unsigned char *str)
{
	for (int i=0; str[i]; i++)
	UART1_transmit(str[i]);
}

uint8_t UART1_IsRxD()
{
	return (UCSR1A & (1<<7));
}


void UART1_ISR_Receive()
{
	volatile static uint8_t head=0;
	volatile uint8_t data;
	
	data = UDR1;
UART0_transmit(data);
	if (data == '\n' || data == '\r')
	{
		if (head) rx1String[head] = '\0';
		head=0;
		rx1ReadyFlag = 1;
	}
	else
	{
		rx1String[head] = data;
		head++;
	}
}

void bt_command_processing()
{
	if (rx1ReadyFlag)	// if (rx_ready_flag >= 1)
	{
		rx1ReadyFlag=0;
		printf("%s\n", rx1String);
		if (strncmp(rx1String, "ledallon", strlen("ledallon")-1) == 0)
		PORTA = 0xff;
		if (strncmp(rx1String, "ledalloff", strlen("ledalloff")-1) == 0)
		PORTA = 0x00;
		if (strncmp(rx1String, "reset", strlen("reset")-1) == 0)
		led_command=0;
		if (strncmp(rx1String, "ledalltoggle", strlen("ledalltoggle")-1) == 0)
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