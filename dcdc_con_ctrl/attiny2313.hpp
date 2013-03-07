// �}�C�R���̐ݒ�BVS�ł̊J���p�ɐF�X�׍H�B
#pragma once

// VS�ł̊J���p. __attribute__�̖�����.
#ifdef	_MSC_VER
#define	__attribute__(x)	
#endif

// CPU�ݒ�
#define	F_CPU	8000000UL

// AVR�֌W�̃w�b�_. ����Ȃ��̂�(���炭)�œK���ŏ����邩����Ȃ�.
#include <avr/io.h>
//#include <avr/sleep.h> # attiny2313�ł͎g���Ȃ�
#include <avr/interrupt.h>
#include <util/delay.h>

// VS�ł̊J���p. MSVC�ł̂ݒ�`�����}�N���𗘗p��include����ƍ��o������.
// http://msdn.microsoft.com/ja-jp/library/ms253913(v=vs.90).aspx
#ifdef	_MSC_VER
#include <avr/iotn2313.h>
#endif
	
// �悭�g���^
typedef	unsigned char	uchar;
	
	
// PWM�Ǘ�
// ������OCRnx(n��{A,B},x��{0,1})�Őݒ�
// n=0��8bit�œ�d�o�b�t�@
// init�ŏ�����. �����̌^��TCCR�Őݒ萔�l
namespace	pwm
{
	// �N���b�N�ݒ�. ps_(���l)��()���̐��l�ŕ���.
	enum PwmClock {
		ck_stop = 0, // stop
		ps_1, ps_8, ps_64, ps_256, ps_1024, // F_CPU/X. X��ps_�̌�̐��l
		ck_fall, ck_raise // �O���N���b�N(T0)�̉��~/�㏸�[
	};

	// �o�͌`��. ���ꂼ�� ����, �g�O��, �񔽓], ���]
	enum PwmOut { pwmout_none = 0, pwmout_toggle, pwmout_noninv, pwmout_inv };

	// Waveform Generation Model.
	// 0:default, 1:phase(8bit), 2:CTC, 3:HighSpeedPWM(8bit), 5:phase, 7:HighSpeedPWM
	enum	WGM
	{
		wgm_default = 0,
		wgm_phase_8bit, 
		wgm_CTC,		// d=0.5�ň��, ������. ���荞�݉�.
		wgm_highspeed,	// 8bit����PWM
		wgm_phase,
		wgm_pwm
	};
	
	// TCCRnA,TCCRnB�𐶐�
	template<PwmOut outA, PwmOut outB, WGM wgm, PwmClock ck>
	struct tccr_generator
	{
		enum {
			valueA =
				((outA & 0x3) << COM0A0) |
				((outB & 0x3) << COM0B0) |
				((wgm & 0x3) << WGM00),
			valueB = 
				((wgm & 0x4) << WGM02) |
				((ck & 0x7) << CS00)
		};
	};

	// ������
	template<class tccr_gen>
	void	init0 (void) { TCCR0A = tccr_gen::valueA; TCCR0B = tccr_gen::valueB; }
	template<class tccr_gen>
	void	init1 (void) { TCCR1A = tccr_gen::valueA; TCCR1B = tccr_gen::valueB; }

	// �^�C�}�[�J�E���g���荞�݂��֎~
	void	disable_timer_interrupt(void) { TIMSK = 0; }


// �Â���
#if 0

// Waveform Generation Model.
// 0:default, 1:phase(8bit), 2:CTC, 3:HighSpeedPWM(8bit), 5:phase, 7:HighSpeedPWM
enum	WGM
{
	wgm_default = 0,	// 0-255�܂ő���. �p���X����.
	wgm_phase_8bit = 1, 
	wgm_CTC = 2,		// d=0.5�ň��, ������. ���荞�݉�.
	wgm_pwm_8bit = 3,
	wgm_phase = 5,
	wgm_pwm = 7
};
	

// PWM�o�͌`���ݒ�
enum PwmOut
{
	pwmout_none = 0,
	pwmout_toggle,
	pwmout_h2l, // High to Low
	pwmout_l2h // Low to High
};
/*
	valueA,B��TCCRnA,B�ɓ����
	out_type_a, _b��A,B�̏o�͐ݒ�(A,B��x�Ƃ���)
	nx�ƃs���̑Ή���	0A:PB6, 0B:PD5, 1A:PB1, 1B:PB2, 2A:PB3, 2B:PD3
	�f���[�e�B���OCRxn��0-255�Őݒ肷��.
*/
template <
	PwmOut outA, PwmOut outB,
	WGM wgm, // 0:default, 1:phase(8bit), 2:CTC, 3:HighSpeedPWM(8bit), 5:phase, 7:HighSpeedPWM
	CkSep cs,
	int foc = 0 // ��Ŏ���
> struct	pwm_setting
{
	// �ݒ肷��l. �R���p�C�����萔�B
	static const int8_t
		valueA =
			((outA & 0x3) << COM0A0) |
			((outB & 0x3) << COM0B0) |
			((wgm & 0x3) << WGM00),
		valueB = 
			((foc & 0x3) << FOC0B) |
			((wgm & 0x4) << WGM02) |
			((cs & 0x7) << CS00);

	// port��DDR�ł̏o�͐ݒ�͂��Ȃ����ߐ�ɂ��Ă����K�v�A��. init�����Ƃ�����PWM.
	static void	init0() { TCCR0A = valueA; TCCR0B = valueB; }
	//static void	init1() { TCCR1A = valueA; TCCR1B = valueB; }
	//static void	init2() { TCCR2A = valueA; TCCR2B = valueB; }
};
#endif

};
