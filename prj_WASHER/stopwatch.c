/*
 * stopwatch.c
 *
 * Created: 2022-08-08 오후 4:07:46
 *  Author: kccistc
 */ 

# include "stopwatch.h"

int stopwatch_state = 0;
uint32_t stopwatchclock = 0;
int stopwatchclock_hour = 0;
int stopwatchclock_min = 0;
int stopwatchclock_sec = 0;
uint8_t count = 0;
// 1초에 1번씩 call을 한다. 분초
void inc_stopwatch_clock(void)
{
	
	count++;
	if(count >= 100)
	{
		count = 0;
		stopwatchclock_sec++;
	if(stopwatchclock_sec >= 60)
	{
		stopwatchclock_sec = 0;
		stopwatchclock_min++;
		if(stopwatchclock_min  >= 60)
		{
			stopwatchclock_min = 0;
			stopwatchclock_hour++;
			if(stopwatchclock_hour >= 24)
			{
				stopwatchclock_hour=0;
			}
		}
	}
	stopwatchclock = (stopwatchclock_sec+stopwatchclock_min *100);
	}
}

stopwatch_run()
{
	set_fnd_data(stopwatchclock);	// FND에 출력 할 data가 넘어간다.
	
	if(get_button2())
	{
		stopwatch_state = STOP;
	}
}
stopwatch_stop()
{
	set_fnd_data(stopwatchclock);	//FND에 출력 할  data가 넘어간다
	
	if (get_button2())
	{
		stopwatch_state = RUN;
	}
	if (get_button3())
	{
		stopwatch_state = RESET;
	}
}
stopwatch_reset()
{
	stopwatchclock=0;
	stopwatchclock_hour=0;
	stopwatchclock_min=0;
	stopwatchclock_sec=0;
	count=0;
	
	set_fnd_data(stopwatchclock);
	stopwatch_state=STOP;
	
}