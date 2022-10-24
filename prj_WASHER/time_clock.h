/*
 * time_clock.h
 *
 * Created: 2022-08-08 오후 3:15:29
 *  Author: kccistc
 */ 


#ifndef TIME_CLOCK_H_
#define TIME_CLOCK_H_
#include <avr/io.h>
// char a[10]
// 서로 다른 변수의 묶음 : 구조체에서 정의 할수 있다.
typedef struct _time	// 사용자 정의
{
	unsigned char hour;
	unsigned char min;
	uint8_t sec;
} TIME;

void get_time_clock(TIME *time);
void set_time_clock(TIME time);
void inc_time_sec(void);


#endif /* TIME_CLOCK_H_ */