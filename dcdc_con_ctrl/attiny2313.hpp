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
struct port { union { bool b0:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; uchar B; }; };
#define	PTB	((port&)(PORTB))
	
	
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

};


// �A�i���O�R���p���[�^. V+<=>V- = (AIN0|Vref)<=>AIN1.
// Vref�͓�����d��1.1V. ����܂�40us.
namespace a_comp
{
	// �R���p���[�^������
	void	disable(void) { ACSR = 1 << ACD; }

	// ���荞�ݏ���
	enum ACmpIntMD { int_tgl = 0, int_fall = 2, int_raise = 3 };

	// �R���p���[�^�L����
	template<
		bool comp_Vref, // 1��Vref, 0��AIN0��V+�ɓ���
		bool interrupt_flag, // ���荞�ݗv��
		bool interrupt_enable, // ���荞�݋���
		bool cap, // capture
		ACmpIntMD acimd // ���荞�݂̃^�C�~���O
	>
	void	enable(void) {
		ACSR = 
			(1 << ACD) | // �L����
			(comp_Vref << ACBG) |
			(interrupt_flag << ACI) |
			(interrupt_enable << ACIE) |
			(cap << ACIC) |
			(acimd << ACIS0);
	}

	// ���ʂ�ǂ�. AIN1 < (AIN0|Vref) �Ŕ�r.
	bool	read(void) { return (ACSR & (1<<ACO)) != 0; }
};




