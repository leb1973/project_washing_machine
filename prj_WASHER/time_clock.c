/*
 * time_clock.c
 *
 * Created: 2022-08-08 오후 3:15:50
 *  Author: kccistc
 */ 

#include "time_clock.h"		// ""는 현재 dire에 있는 파일을 물고온다.

TIME time_clock = {0,0,0};	

void get_time_clock(TIME *time)
{
	*time  =   time_clock;
}
void set_time_clock(TIME time)
{
	time_clock = time;
}

// 1초에 1번씩 call을 한다. 분초
void inc_time_sec(void)
{
	time_clock.sec--;
	if(time_clock.sec >= 60)
	{
		time_clock.sec = 0;
		time_clock.min++;
		if(time_clock.min  >= 60)
		{
			time_clock.min = 0;
			time_clock.hour++;
			if(time_clock.hour >= 24)
			{
				time_clock.hour=0;
			}
		}
	}
}