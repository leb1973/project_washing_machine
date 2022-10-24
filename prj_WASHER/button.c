/*
 * button.c
 *
 * Created: 2022-08-04 오후 4:12:17
 *  Author: kccistc
 */ 
#include "button.h"
int get_button1();
int get_button2();
int get_button3();
int get_button4();


void init_button()
{
	BUTTON_DDR &= ~((1 << BUTTON1 ) | (1 << BUTTON2) | (1 << BUTTON3 ) | (1 << BUTTON4 ));	// input : 0으로 설정
}

uint8_t prev_state = 0;		// 버튼의 상태 0: off 1: on
int get_button1()
{
	static uint8_t prev_state = 0;	// 버튼의 상태 0: off 1: on
	uint8_t  current_state;			// unsigned char current state
	
	current_state = BUTTON_PIN & (1 << BUTTON1);	// button1값을 익힌다. 0x10
	if (prev_state == 0 && current_state !=0)		//버튼이 처음 눌려진 상태
	{
		_delay_ms(60);	// 노이즈가 지나가기를 기다린다.
		prev_state =1;	// 처음 눌려진 상태가 아니다.
		return 0;		// 아직은 진짜로 눌려진 상태가 아니다.
	}
	else if(prev_state == 1 && current_state == 0 )	// 버튼이 눌렸다 prev_state ==1 떼어진 상태
	{
		prev_state = 0;
		_delay_ms(30);
		return 1;		// 완전히 눌렸다 떼어진 상태로 인정
	}
		return 0;		// 아직 완전히 s/w를 눌렀다 떼지않은 상태
}


int get_button2()
{
	static uint8_t prev_state = 0;	// 버튼의 상태 0: off 1: on
	uint8_t  current_state;			// unsigned char current state
	
	current_state = BUTTON_PIN & (1 << BUTTON2);	// buttoon1값을 익힌다. 0x10
	if (prev_state == 0 && current_state !=0)		//버튼이 처음 눌려진 상태
	{
		_delay_ms(60);	// 노이즈가 지나가기를 기다린다.
		prev_state =1;	// 처음 눌려진 상태가 아니다.
		return 0;		// 아직은 진짜로 눌려진 상태가 아니다.
	}
	else if(prev_state == 1 && current_state == 0 )	// 버튼이 눌렸다 prev_state ==1 떼어진 상태
	{
		prev_state = 0;
		_delay_ms(30);
		return 1;		// 완전히 눌렸다 떼어진 상태로 인정
	}
	return 0;		// 아직 완전히 s/w를 눌렀다 떼지않은 상태
}

int get_button3()
{
	static uint8_t prev_state = 0;	// 버튼의 상태 0: off 1: on
	uint8_t  current_state;			// unsigned char current state
	
	current_state = BUTTON_PIN & (1 << BUTTON3);	// buttoon1값을 익힌다. 0x10
	if (prev_state == 0 && current_state !=0)		//버튼이 처음 눌려진 상태
	{
		_delay_ms(60);	// 노이즈가 지나가기를 기다린다.
		prev_state =1;	// 처음 눌려진 상태가 아니다.
		return 0;		// 아직은 진짜로 눌려진 상태가 아니다.
	}
	else if(prev_state == 1 && current_state == 0 )	// 버튼이 눌렸다 prev_state ==1 떼어진 상태
	{
		prev_state = 0;
		_delay_ms(30);
		return 1;		// 완전히 눌렸다 떼어진 상태로 인정
	}
	return 0;		// 아직 완전히 s/w를 눌렀다 떼지않은 상태
}

int get_button4()
{
	static uint8_t prev_state = 0;	// 버튼의 상태 0: off 1: on
	uint8_t  current_state;			// unsigned char current state
	
	current_state = BUTTON_PIN & (1 << BUTTON1);	// button1값을 익힌다. 0x10
	if (prev_state == 0 && current_state !=0)		//버튼이 처음 눌려진 상태
	{
		_delay_ms(60);	// 노이즈가 지나가기를 기다린다.
		prev_state =1;	// 처음 눌려진 상태가 아니다.
		return 0;		// 아직은 진짜로 눌려진 상태가 아니다.
	}
	else if(prev_state == 1 && current_state == 0 )	// 버튼이 눌렸다 prev_state ==1 떼어진 상태
	{
		prev_state = 0;
		_delay_ms(30);
		return 1;		// 완전히 눌렸다 떼어진 상태로 인정
	}
	return 0;		// 아직 완전히 s/w를 눌렀다 떼지않은 상태
}