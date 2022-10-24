/*
 * ultrasonic.c
 *
 * Created: 2022-08-10 오후 4:24:14
 *  Author: kccistc
 */ 

#include "ultrasonic.h"

void init_ultrasonic();
void ultrasonic_trigger();

volatile int ultrasonic_distance;
char scm[50];
// PE4 : 외부 INT4 초음파 센서 상승, 하강 엣지 둘다 이쪽으로 들어온다.
ISR(INT4_vect)
{
	if (ECHO_PIN & (1 << ECHO))		// 상승 엣지
	{
		TCNT1=0;
	}
	else  // 하강 엣지
	{
		// echo pin 의 펄스 길이를 us 단위로 환산
		ultrasonic_distance = 1000000.0 * TCNT1 * 1024 / F_CPU;
		// 1cm : 58us 소요
		sprintf(scm, "distance: %dcm", ultrasonic_distance/58);	// cm 로 환산
	}
}
// PG4 : trigger
// PE4 : echo (INT4) 외부 인터럽트 4번
void init_ultrasonic()
{
	TRIG_DDR |= (1 << TRIG);	// output mode 로 설정
	ECHO_DDR &=	~(1 << ECHO);	// input  mode 로 설정
	
	// 0 1 조건 : 어떠한 형태로던지 신호 변화가 발생되면 INT 요청 (상승, 하강엣지 둘다 INT)
	EICRB |= (0 << ISC41) | (1 << ISC40);
	TCCR1B |= (1 << CS12) | (1 << CS10);	// 1024 로 분주
	// 16bit timer 에서 1번을 1024 분주 해서 공급한다.
	// 16MHz 를 1024 로 분주 --> 16000000/1024 ==> 15625Hz ==> 15.625KHz
	// 1펄스의 길이 : t = 1/f --> 1/15625 = 0.000064sec ==> 64us
	
	EIMSK |= (1 << INT4);	// 외부 interrupt 4번 INT4(ECHO) 사용
}
void ultrasonic_trigger()
{
	TRIG_PORT &= ~(1 << TRIG);	// LOW
	_delay_us(1);
	TRIG_PORT |= (1 << TRIG);	// 1: HIGH
	_delay_us(15);
	TRIG_PORT &= ~(1 << TRIG);	// LOW
}