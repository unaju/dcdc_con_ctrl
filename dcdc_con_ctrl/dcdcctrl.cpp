
#include "attiny2313.hpp"
#include "alg.hpp"

/*	�|�[�g�ݒ��
	PWM�o��:PB2(OC0A)
	�R���p���[�^��Vref:PB0(AIN0), V-:PB1(AIN1)
*/


// PWM�ݒ�
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
	// ���o�͐ݒ�. D,B��Out
	DDRB = 0xFF;
	DDRD = 0xFF;

	// �^�C�}�[���荞�ݖ���
	pwm::disable_timer_interrupt();

	// pwm�ݒ�
	pwm::init0<conf::pwmconf>();
	PTB.b3 = 1;
	for(;;){
	}

	return 0;
}

