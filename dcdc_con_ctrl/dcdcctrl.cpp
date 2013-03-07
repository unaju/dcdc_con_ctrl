
#include "attiny2313.hpp"
#include "alg.hpp"

/*	ポート設定は
	PWM出力:PB2(OC0A)
	コンパレータのVref:PB0(AIN0), V-:PB1(AIN1)
*/


// PWM設定
namespace conf
{
	using namespace pwm;
	typedef tccr_generator<
		pwmout_noninv, pwmout_none,
		wgm_highspeed, ps_1
	>	pwmconf;


};



int	main(void)
{
	// 入出力設定. D,BはOut
	DDRB = 0xFF;
	DDRD = 0xFF;

	// タイマー割り込み無し
	pwm::disable_timer_interrupt();

	// pwm設定
	pwm::init0<conf::pwmconf>();
	PTB.b3 = 1;
	for(;;){
	}

	return 0;
}

