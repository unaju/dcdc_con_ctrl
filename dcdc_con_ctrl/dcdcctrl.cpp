
#include "attiny2313.hpp"
#include "alg.hpp"

/*	設定について
	
	CPUは8MHz内部発振
	ヒューズビットは
	-U lfuse:w:0xe4:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m

	ポート設定は
	PWM出力:PB2(OC0A)
	コンパレータのVref:PB0(AIN0), V-:PB1(AIN1)
*/


void loop(void)
{
	// 出力できるか試す
	bool v = a_comp::read();
	PORTD = (PORTD & ~(1<<PD0)) | v;
}

int	main(void)
{
	// 入出力設定. D,BはOut
	DDRB = 0xFF;
	DDRD = 0xFF;

	// タイマー割り込み無し
	pwm::disable_timer_interrupt();

	// PWM設定
	pwm::init0< pwm::tccr_generator<
		pwm::pwmout_noninv, pwm::pwmout_none,
		pwm::wgm_highspeed, pwm::ps_1
	> >();
	OCR0A = 200;

	// コンパレータ設定
	a_comp::enable<0, 0, 0, 0, a_comp::int_tgl>();

	// ループ部分
	for(;;) loop();

	return 0;
}

