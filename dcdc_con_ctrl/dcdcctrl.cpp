
#include "attiny2313.hpp"
#include "alg.hpp"


// PWM設定
namespace conf
{
	using namespace pwm;
	typedef tccr_generator<
		pwmout_noninv, pwmout_inv,
		wgm_highspeed, ps_256
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

	for(;;){
	}

	return 0;
}

