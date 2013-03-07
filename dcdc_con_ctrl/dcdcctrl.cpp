
#include "attiny2313.hpp"
#include "alg.hpp"


// PWM�ݒ�
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
	// ���o�͐ݒ�. D,B��Out
	DDRB = 0xFF;
	DDRD = 0xFF;

	// �^�C�}�[���荞�ݖ���
	pwm::disable_timer_interrupt();

	// pwm�ݒ�
	pwm::init0<conf::pwmconf>();

	for(;;){
	}

	return 0;
}

