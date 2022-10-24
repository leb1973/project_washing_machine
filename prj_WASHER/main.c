/*
 * FND_COUNTER.c
 *  초 counter 를 작성 한다. 
 * Created: 2022-08-05 오전 11:46:29
 * Author : kcci
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h> 
#include <avr/interrupt.h>
#include <stdio.h>	// printf, scanf 등이 정의되어 있다.
#include <string.h>	// strcpy, strncmp, strcat 등이 들어 있다.

#include "fnd.h"
#include "time_clock.h"
#include "button.h"
#include "stopwatch.h"

volatile int tim_16ms =0;

 int mode_state = 0;
 int auto_state = 0;
 int manual_state = 0;
 int timer_state = 0;
 uint32_t timerclock=0;
 int timer_hour=0;
 int timer_min=0;
 int timer_sec=0;
 uint8_t count =0;
	uint8_t mode;
enum _mode{AUTO,SELF};
static uint8_t wash_RIN_dry =0;

extern volatile uint8_t rx_ready_flag;	// 완전한  문장이 들어왔을 때 1 로 set 된다. 
extern void UART0_transmit(uint8_t data);
extern void init_UART1();
extern void pc_command_processing(); // extern 은 다른 화일에 있다는 것을 compiler 에게 알려주는 것.
extern void init_led();

static uint8_t check_finish =0;	 // 세탁기가 다 돌고 종료되었을때를 확인 : 0 은 안끝났다  1은 끝났다.
static uint8_t check_running =0;	 // 세탁기가 시작 버튼을 누르고 모든 과정이 종료되기 전까지를 확인하기 위한 변수
static uint8_t before_finishing =0;  // 완료되기 전까지는 1이다. 완료 되었을때 0 이된다.


static uint32_t total_time=1;// 빨래하는데 걸리는 총 소요시간
static int  running_lcd= 0;		// to display running state dynamic
static uint8_t  blink_select= 0;	  // to make options blink
int set_time=0;

static int wash;
static int RIN;
static int dry;

#define AUTO 0
#define SELF 1



// 1. for printf  : printf 해주기 위한 작업
FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);

uint32_t ms_count=0;  // ms를 재는 변수
uint32_t sec_count=0;  // sec를 재는 변수
uint32_t fast_sec_count=0;  // fast  sec를 재는 변수
uint32_t common_sec_count=0;
uint32_t MANUAL_sec_count=0;
volatile fs_ms_count=0;	// 분초 시계 display 하기 위한 변수
volatile common_ms_count=0;
volatile MANUAL_ms_count=0;
volatile int ms_clock_display=0;	// 분초 시계 display 하기 위한 변수
int fast_indicator=0;
int common_indicator =0;
int MANUAL_indicator =0;
int wash_count = 0;
int rinse_count = 0;
int dry_count = 0;
int wash_time = 0;
int rinse_time = 0;
int dry_time = 0;
int processState = 0;
int spin = 0;

extern char scm[50];

int state = 0;


// PORTB.0 : 500ms  on/off
// PORTB.1 : 300ms on/off
// 1. 분주: 64분주 ==> 16000000/64 ==> 250,000HZ
// 2. T 계산 ==> 1/f = 1/250000 = > 0.000004 sec (4us) : 0.004ms
// 3. 8 bit Timer OV: 0.004ms x 256 ==> 0.00124sec ==> 약 1.24ms
//                    0.004ms x 250 ==> 1ms
// 256개의 pulse 를 count 를 하면 이곳으로 온다.
// 1ms 마다 ISR(TIMER0_OVF_vect) 이곳으로 들어 온다
ISR(TIMER0_OVF_vect)
{
	TCNT0 =0;
	ms_count++;
	fs_ms_count++;
	common_ms_count++;
	 MANUAL_ms_count++;
	
	
	if(ms_count >=1000)
	{
		ms_count = 0;
		sec_count++;
		if (!fast_indicator)
			fast_sec_count++;
		if (!common_indicator)
			common_sec_count++;
		if (!MANUAL_indicator)
		MANUAL_sec_count++;
		
		inc_time_sec();
	}
		if (ms_count%4 == 0)  // 4ms 마다 FND를 display
		display_fnd();
		
}





uint8_t modestate;


int main(void)


{ 
		TIME myTime;
		char sbuf[40];
		int button1_state =0;
		int button2_state =0;
		int button3_state =0;
		int fan_mode ;			// start : 1/stop : 0

		init_pwm();
		init_led();
		init_fnd();
		init_button();
		init_uart0();		// uart0 를 초기화
		init_UART1();		// uart1 를 초기화
		I2C_LCD_init();		// LCD 초기화
		stdout = &OUTPUT;	// 2. for printf fprintf(stdout,"test"); ==> printf    : printf 해주기 위한 작업
		init_timer0();
		sei();				// 전역적으로 인터럽트 허용
	
	
	
	
//printf("Hello Stopwatch !!!\n");	// 시험용으로 적는건 맨 앞에 적어두는게 보기 편하다.

	while (1)
	{
			
		
		
		if(mode_state == 0)
		{
			I2C_LCD_write_string_XY(0, 0, "                  ");
			I2C_LCD_write_string_XY(0, 0, "==  MODE SET ==  ");
			I2C_LCD_write_string_XY(1, 0, "                  ");
			I2C_LCD_write_string_XY(1, 0, " 1.AUTO 2.MANUAL ");
			
			
			if(get_button1())  // Auto 선택
			{
				I2C_LCD_write_string_XY(0, 0, "                  ");
				I2C_LCD_write_string_XY(0, 0, "==  AUTO SET ==  ");
				I2C_LCD_write_string_XY(1, 0, "                  ");
				I2C_LCD_write_string_XY(1, 0, " 1.FAST 2.common ");
				mode_state = 1;
				auto_state = 1;
				manual_state = 0;
 
			}
			if(get_button2())  // Manual 선택
			{
				
				I2C_LCD_write_string_XY(0, 0, "                  ");
				I2C_LCD_write_string_XY(0, 0, "== MANUAL SET ==  ");
				I2C_LCD_write_string_XY(1, 0, "                  ");
				I2C_LCD_write_string_XY(1, 0, " choose ==> bt2  ");
				mode_state = 1;
				auto_state = 0;
				manual_state = 1;
				
			}
		}
		
		
			if(mode_state == 1 && auto_state == 1)  // Auto_state 1 일 때
			{
		
					if(get_button2())
					{
						fast_indicator=1;

						FAST_time();	

					}

 

				if( get_button3())
				{
					common_indicator=1;
					common_time();
				
				}
				
			}
			
			if(mode_state == 1 && manual_state == 1)  // Manual_state 1 일 때
			{
			   if(get_button2())
			   {
				   MANUAL_time();
			   }
				
				
			}
	}
}

void init_timer0()
{
	TCNT0=6;   // TCNT 6~256 ==> 정확히 1ms 마다 TIMT 0 OVF INT
	// 분주비를 64로 설정 P269 표13-1
	TCCR0 |= (1 << CS02) | (0 << CS01) | (0 << CS00);

	TIMSK |= (1 << TOIE0);			// 오버플로 인터럽트 허용
	
	sei();							// 전역적으로 인터럽트 허용
}

void init_pwm()
{
	DDRE |= (1 << 3) | (1 << 4) | (1 << 5);
	// 모드 5 : 8비트 고속 pwm timer 3
	TCCR3A |= (1 << WGM30);
	TCCR3B |= (1 << WGM32);	// p348 표 15-6
	// 비반전 모드 top : 0xff 비교일치값 OCR3c
	TCCR3A |= (1 << COM3C1);
	// 분주비 64 : 16000000/64 ==> 250000Hz(2250KHz)
	// 256 / 250000Hz ==> 1.02ms
	// 127 / 250000 => 0.5ms
	TCCR3B |= (1 << CS31) | (1 << CS30);	// 분주비 64
	OCR3C = 0;
	DDRA = 0xff;
	DDRF = 0x01;
}

void FAST_time()
{
	char sbuf[40];
	int fan_mode;

	   if(before_finishing==0 && check_running==0)// can not setting during 세탁, 일시정지
	   {
		   wash=1;
		   RIN =1;
		   dry =1;
		   total_time =(wash*10)+(RIN*10)+(dry*10);
		   set_time =total_time;
	   }
		fs_ms_count=0;
		
		 for(fast_sec_count = 0; fast_sec_count < total_time;)
		 {
			 
			 if (fs_ms_count >= 1000)   // 1000ms ==> 1sec
			 {
				 
				fs_ms_count=0;
				fast_sec_count++;
				
				 I2C_LCD_write_string_XY(0, 0, "                ");
				 I2C_LCD_write_string_XY(1, 0, "                ");
				 power_AUTO();
				 

			 }		


			I2C_LCD_write_string_XY(0, 0, "                  ");
			sprintf(sbuf, "[FAST]time : %d s",total_time - fast_sec_count);
			I2C_LCD_write_string_XY(0, 0,sbuf );
			I2C_LCD_write_string_XY(1, 0, "                  ");
			sprintf(sbuf, "w: %d,R : %d D : %d ",wash,RIN,dry);
			I2C_LCD_write_string_XY(1, 0, sbuf);
			
		
			if(total_time == fast_sec_count)
			{
			
				I2C_LCD_write_string_XY(0, 0, "                ");
				I2C_LCD_write_string_XY(0, 0, "      end       ");
				I2C_LCD_write_string_XY(1, 0, "                ");
				power_done();
			
			}
		
		 }
}

void common_time()
{
	char sbuf[40];
	if(before_finishing==0 && check_running==0)// can not setting during 세탁, 일시정지
	{
		wash=1;
		RIN =2;
		dry =2;
		total_time =(wash*10)+(RIN*10)+(dry*10);
		set_time =total_time;
	}
	common_ms_count =0;
	 for(common_sec_count = 0; common_sec_count < total_time;)
	 {
		 
		 if (common_ms_count >= 1000)   // 1000ms ==> 1sec
		 {
			 common_ms_count=0;
			 common_sec_count++;
			 I2C_LCD_write_string_XY(0, 0, "                ");
			 I2C_LCD_write_string_XY(1, 0, "                ");
			 power_common();
			 
		 }
		I2C_LCD_write_string_XY(0, 0, "                  ");
		sprintf(sbuf, "[common]time:%d",total_time-common_sec_count);
		I2C_LCD_write_string_XY(0, 0, sbuf);
		I2C_LCD_write_string_XY(1, 0, "                  ");
		sprintf(sbuf, "w: %d,R : %d D : %d ",wash,RIN,dry);
		I2C_LCD_write_string_XY(1, 0, sbuf);
		
		if(total_time == common_sec_count)
		{
			
			I2C_LCD_write_string_XY(0, 0, "                ");
			I2C_LCD_write_string_XY(0, 0, "      end       ");
			I2C_LCD_write_string_XY(1, 0, "                ");
			power_done();
			
			
		}
	 }
}

void MANUAL_time()
{
	char sbuf[40];
	int set_count=0;
	char s[4];
	
	while(1)
	{
		
		
		if (get_button2())
		{
			if (set_count==0)
			wash_count++;
			else if (set_count==1)
			rinse_count++;
			else if (set_count==2)
			dry_count++;
		}
		if (get_button3())
		{
			set_count++;
			set_count %= 3;

			switch(set_count)
			{
				case 0:
				sprintf(s, "%d", wash_count);
				break;
				case 1:
				sprintf(s, "%d", rinse_count);
				break;
				case 2:
				sprintf(s, "%d", dry_count);
				break;
			}
			I2C_LCD_write_string_XY(1,(set_count+1)*3,s);
		}
		if (get_button1())
		{
			power_MANUAL();
			break;
		}
			wash_time = wash_count * 5;
			rinse_time = rinse_count * 5;
			dry_time = dry_count * 5;
			sec_count = 0;
			 I2C_LCD_write_string_XY(0, 0, "                ");
			sprintf(sbuf, "[choose]bt count");
			I2C_LCD_write_string_XY(0,0,sbuf);
			 I2C_LCD_write_string_XY(1, 0, "                ");
			sprintf(sbuf, "W:%d R:%d D:%d", wash_count, rinse_count, dry_count);
			I2C_LCD_write_string_XY(1,0,sbuf);
			
		
	}
}

void power_AUTO()
{
	PORTE &= 0b11111011;
	PORTE |= 0b00000001;
	OCR3C=210;
}

void power_common()
{

	PORTE &= 0b11111011;
	PORTE |= 0b00000001;
	OCR3C=170;
}
void power_MANUAL_mode()
{
	switch(spin)
	{
		case 0:
		// 정회전 : PE2:0 PE0 1
		PORTE &= 0b11111011;
		PORTE |= 0b00000001;
		break;
		case 1:
		// 모터정지 : PE0:1 PE2:1
		PORTE |= 0b00000101;
		break;
		case 2:
		// 역회전 : PE2:1 PE0 0
		PORTE &= 0b11111110;
		PORTE |= 0b00000100;
		break;
		default:
		// 모터정지 : PE0:1 PE2:1
		PORTE |= 0b00000101;
		break;
	}
	OCR3C=160;
}

void power_MANUAL()
{
	int sum;
	sum = wash_count + rinse_count + dry_count;
	int laundry = wash_count * 5 + rinse_count * 5 + dry_count * 5;
	char sbuf[40];
	MANUAL_sec_count = 0;
	for(MANUAL_sec_count = 0; MANUAL_sec_count <laundry;)
	{
		
		if (MANUAL_ms_count >= 1000)   // 1000ms ==> 1sec
		{
			
			MANUAL_ms_count=0;
			MANUAL_sec_count++;
			
			I2C_LCD_write_string_XY(0, 0, "                ");
			I2C_LCD_write_string_XY(1, 0, "                ");

		}
	
		switch (sum)
		{
			case 0:
			for(;laundry > MANUAL_sec_count;)
			{
				 I2C_LCD_write_string_XY(0, 0, "                ");
				sprintf(sbuf, " ==mode start==  ");
				I2C_LCD_write_string_XY(0,0,sbuf);
				 I2C_LCD_write_string_XY(1, 0, "                ");
				sprintf(sbuf, "time =%d", laundry -  MANUAL_sec_count);
				I2C_LCD_write_string_XY(1,0,sbuf);
				 power_MANUAL_mode();
				if(laundry == MANUAL_sec_count)
				{
					I2C_LCD_write_string_XY(0, 0, "                ");
					I2C_LCD_write_string_XY(0, 0, "      end       ");
					I2C_LCD_write_string_XY(1, 0, "                ");
					power_done();
				}
			
			}
			break;
			
			case 1:
			for(;laundry > MANUAL_sec_count;)
			{
				 I2C_LCD_write_string_XY(0, 0, "                ");
				sprintf(sbuf, " ==mode start==  ");
				I2C_LCD_write_string_XY(0,0,sbuf);
				 I2C_LCD_write_string_XY(1, 0, "                ");
				sprintf(sbuf, "time = %d", laundry -  MANUAL_sec_count);
				I2C_LCD_write_string_XY(1,0,sbuf);
				 power_MANUAL_mode();
				if(laundry == MANUAL_sec_count)
				{
					I2C_LCD_write_string_XY(0, 0, "                ");
					I2C_LCD_write_string_XY(0, 0, "      end       ");
					I2C_LCD_write_string_XY(1, 0, "                ");
					power_done();
				}

			
			}
			

			break;
		
			case 2:
			for(;laundry >  MANUAL_sec_count;)
			{
				 I2C_LCD_write_string_XY(0, 0, "                ");
				sprintf(sbuf, " ==mode start==  ");
				I2C_LCD_write_string_XY(0,0,sbuf);
				 I2C_LCD_write_string_XY(1, 0, "                ");
				sprintf(sbuf, "time =%d", laundry -  MANUAL_sec_count);
				I2C_LCD_write_string_XY(1,0,sbuf);
				 power_MANUAL_mode();
				if(laundry == MANUAL_sec_count)
				{
					I2C_LCD_write_string_XY(0, 0, "                ");
					I2C_LCD_write_string_XY(0, 0, "      end       ");
					I2C_LCD_write_string_XY(1, 0, "                ");
					power_done();
				}
			
			}
			
			break;
		
			case 3:
			for(;laundry >  MANUAL_sec_count;)
			{
				 I2C_LCD_write_string_XY(0, 0, "                ");
				sprintf(sbuf, " ==mode start==  ");
				I2C_LCD_write_string_XY(0,0,sbuf);
				 I2C_LCD_write_string_XY(1, 0, "                ");
				sprintf(sbuf, "time =%d", laundry -  MANUAL_sec_count);
				I2C_LCD_write_string_XY(1,0,sbuf);
				 power_MANUAL_mode();
				if(laundry == MANUAL_sec_count)
				{
					I2C_LCD_write_string_XY(0, 0, "                ");
					I2C_LCD_write_string_XY(0, 0, "      end       ");
					I2C_LCD_write_string_XY(1, 0, "                ");
					power_done();
				}
			
			}
			if(laundry == MANUAL_sec_count)
			{
				power_done();
			}
			break;
		
			case 4:
			for(;laundry >  MANUAL_sec_count;)
			{
				I2C_LCD_write_string_XY(0, 0, "                ");
				sprintf(sbuf, " ==mode start==  ");
				I2C_LCD_write_string_XY(0,0,sbuf);
				 I2C_LCD_write_string_XY(1, 0, "                ");
				sprintf(sbuf, "time =%d", laundry -  MANUAL_sec_count);
				I2C_LCD_write_string_XY(1,0,sbuf);
				 power_MANUAL_mode();
				if(laundry == MANUAL_sec_count)
				{
					I2C_LCD_write_string_XY(0, 0, "                ");
					I2C_LCD_write_string_XY(0, 0, "      end       ");
					I2C_LCD_write_string_XY(1, 0, "                ");
					power_done();
				}
			
			}
			
			break;
		
			case 5:
			for(;laundry >  MANUAL_sec_count;)
			{    I2C_LCD_write_string_XY(0, 0, "                ");
				sprintf(sbuf, " ==mode start==  ");
				I2C_LCD_write_string_XY(0,0,sbuf);
				 I2C_LCD_write_string_XY(1, 0, "                ");
				sprintf(sbuf, "time =%d", laundry -  MANUAL_sec_count);
				I2C_LCD_write_string_XY(1,0,sbuf);
				power_common();
				if(laundry == MANUAL_sec_count)
				{
					I2C_LCD_write_string_XY(0, 0, "                ");
					I2C_LCD_write_string_XY(0, 0, "      end       ");
					I2C_LCD_write_string_XY(1, 0, "                ");
					power_done();
				}
			
			}
			
			break;
			case 6:
			for(;laundry > MANUAL_sec_count;)
			{
				 I2C_LCD_write_string_XY(0, 0, "                ");
				sprintf(sbuf, " ==mode start==  ");
				I2C_LCD_write_string_XY(0,0,sbuf);
				 I2C_LCD_write_string_XY(1, 0, "                ");
				sprintf(sbuf, "time =%d", laundry -  MANUAL_sec_count);
				I2C_LCD_write_string_XY(1,0,sbuf);
				power_common();
				if(laundry == MANUAL_sec_count)
				{
					I2C_LCD_write_string_XY(0, 0, "                ");
					I2C_LCD_write_string_XY(0, 0, "      end       ");
					I2C_LCD_write_string_XY(1, 0, "                ");
					power_done();
				}
			
			}
			
			break;
			case 7:
			for(;laundry >  MANUAL_sec_count;)
			{
				 I2C_LCD_write_string_XY(0, 0, "                ");
				sprintf(sbuf, " ==mode start==  ");
				I2C_LCD_write_string_XY(0,0,sbuf);
				 I2C_LCD_write_string_XY(1, 0, "                ");
				sprintf(sbuf, "time =%d", laundry -  MANUAL_sec_count);
				I2C_LCD_write_string_XY(1,0,sbuf);
				power_common();
				if(laundry == MANUAL_sec_count)
				{
					I2C_LCD_write_string_XY(0, 0, "                ");
					I2C_LCD_write_string_XY(0, 0, "      end       ");
					I2C_LCD_write_string_XY(1, 0, "                ");
					power_done();
				}
			
			}
			
			break;
			case 8:
			for(;laundry >  MANUAL_sec_count;)
			{
				 I2C_LCD_write_string_XY(0, 0, "                ");
				sprintf(sbuf, " ==mode start==  ");
				I2C_LCD_write_string_XY(0,0,sbuf);
				 I2C_LCD_write_string_XY(1, 0, "                ");
				sprintf(sbuf, "time =%d", laundry -  MANUAL_sec_count);
				I2C_LCD_write_string_XY(1,0,sbuf);
				power_common();
				if(laundry == MANUAL_sec_count)
				{
					I2C_LCD_write_string_XY(0, 0, "                ");
					I2C_LCD_write_string_XY(0, 0, "      end       ");
					I2C_LCD_write_string_XY(1, 0, "                ");
					power_done();
				}
			
			}
			break;
			
			case 9:
			for(;laundry >  MANUAL_sec_count;)
			{
				I2C_LCD_write_string_XY(0, 0, "                ");
				sprintf(sbuf, " ==mode start==  ");
				I2C_LCD_write_string_XY(0,0,sbuf);
				I2C_LCD_write_string_XY(1, 0, "                ");
				sprintf(sbuf, "time =%d", laundry -  MANUAL_sec_count);
				I2C_LCD_write_string_XY(1,0,sbuf);
				power_common();
				if(laundry == MANUAL_sec_count)
				{
					I2C_LCD_write_string_XY(0, 0, "                ");
					I2C_LCD_write_string_XY(0, 0, "      end       ");
					I2C_LCD_write_string_XY(1, 0, "                ");
					power_done();
				}
				
			}
			break;
			case 10:
			for(;laundry >  MANUAL_sec_count;)
			{
				I2C_LCD_write_string_XY(0, 0, "                ");
				sprintf(sbuf, " ==mode start==  ");
				I2C_LCD_write_string_XY(0,0,sbuf);
				I2C_LCD_write_string_XY(1, 0, "                ");
				sprintf(sbuf, "time =%d", laundry -  MANUAL_sec_count);
				I2C_LCD_write_string_XY(1,0,sbuf);
				power_common();
				if(laundry == MANUAL_sec_count)
				{
					I2C_LCD_write_string_XY(0, 0, "                ");
					I2C_LCD_write_string_XY(0, 0, "      end       ");
					I2C_LCD_write_string_XY(1, 0, "                ");
					power_done();
				}
				
			}
			
			break;
		}
		break;
	}
	
}

void power_done()
{
	OCR3C = 0;
}

void shift_left_led_on(void)
{
	for(int i = 0; i < 8; i++)
	{
		PORTA = 0b00000001 << i;
		_delay_ms(300);
	}
}