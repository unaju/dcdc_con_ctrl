
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

// 遅いPWM(0-255までカウント)のモードを使うか
#define	LOW_SPEED_PWM	0

// 型とかの設定
#if LOW_SPEED_PWM
typedef	pwm::tccr_generator<
	pwm::pwmout_noninv, pwm::pwmout_none,
	pwm::wgm_highspeed, pwm::ps_1
>	tccr_t;
#else
typedef	pwm::fastpwm_8bit<pwm::pwmout_noninv, pwm::ps_1>	pwm_t;
#endif // LOW_SPEED_PWM

// ループする部分
void loop(void)
{
#if LOW_SPEED_PWM
	// 64回 電圧を比較して平滑化
	int v = 0;
	for(int i = 0; i < 64; ++i) {
		// 基準電圧の方が電圧が高いなら+1, 低いなら-1
		v += a_comp::read() ? +1 : -1;
	}
	// 新しいPWM出力値を設定
	v = OCR0A + ((v < 0) ? -1 : 1);
	OCR0A = (v < 0) ? 0 : (v > 255) ? 255 : v;

#else
#endif // LOW_SPEED_PWM
}

// 初期化部分
int	main(void)
{
	// 入出力設定. D,BはOut
	DDRB = 0xFF;
	DDRD = 0xFF;

	// タイマー割り込み無し
	pwm::disable_timer_interrupt();

	// PWM設定
#if LOW_SPEED_PWM
	pwm::init0<tccr_t>();
#else
	pwm_t::init();
	pwm_t::setMax(64);
	pwm_t::setVal(40);
#endif // LOW_SPEED_PWM

	// コンパレータ設定
	a_comp::enable<0, 0, 0, 0, a_comp::int_tgl>();

	// ループ部分
	for(;;) loop();

	return 0;
}

