/*
 * UART0.h
 *
 * Created: 2022-08-09 오전 10:45:45
 *  Author: kccistc
 */ 


#ifndef UART0_H_
#define UART0_H_

void init_uart0(void);
void UART0_transmit(uint8_t data);
void pc_command_processing();

// UART0로부터 1 byte 가 들어오면 RX interrupt 발생 되어 이곳으로 들어온다.
// pg278 표 12-3 참조
volatile unsigned char rx_buff[100];
volatile uint8_t rx_ready_flag=0;	// 완전한  문장이 들어왔을 때 1 로 set 된다.
volatile int i=0;

#endif /* UART0_H_ */