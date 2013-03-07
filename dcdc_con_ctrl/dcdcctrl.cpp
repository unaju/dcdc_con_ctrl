
#include "attiny2313.hpp"
#include "alg.hpp"

/*	�ݒ�ɂ���
	
	CPU��8MHz�������U
	�q���[�Y�r�b�g��
	-U lfuse:w:0xe4:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m

	�|�[�g�ݒ��
	PWM�o��:PB2(OC0A)
	�R���p���[�^��Vref:PB0(AIN0), V-:PB1(AIN1)
*/


void loop(void)
{
	// �o�͂ł��邩����
	bool v = a_comp::read();
	PORTD = (PORTD & ~(1<<PD0)) | v;
}

int	main(void)
{
	// ���o�͐ݒ�. D,B��Out
	DDRB = 0xFF;
	DDRD = 0xFF;

	// �^�C�}�[���荞�ݖ���
	pwm::disable_timer_interrupt();

	// PWM�ݒ�
	pwm::init0< pwm::tccr_generator<
		pwm::pwmout_noninv, pwm::pwmout_none,
		pwm::wgm_highspeed, pwm::ps_1
	> >();
	OCR0A = 200;

	// �R���p���[�^�ݒ�
	a_comp::enable<0, 0, 0, 0, a_comp::int_tgl>();

	// ���[�v����
	for(;;) loop();

	return 0;
}

