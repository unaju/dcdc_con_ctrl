// マイコンの設定。VSでの開発用に色々細工。
#pragma once

// VSでの開発用. __attribute__の無効化.
#ifdef	_MSC_VER
#define	__attribute__(x)	
#endif

// CPU設定
#define	F_CPU	8000000UL

// AVR関係のヘッダ. いらないのは(恐らく)最適化で消えるから問題なし.
#include <avr/io.h>
//#include <avr/sleep.h> # attiny2313では使えない
#include <avr/interrupt.h>
#include <util/delay.h>

// VSでの開発用. MSVCでのみ定義されるマクロを利用しincludeすると錯覚させる.
// http://msdn.microsoft.com/ja-jp/library/ms253913(v=vs.90).aspx
#ifdef	_MSC_VER
#include <avr/iotn2313.h>
#endif
	
// よく使う型
typedef	unsigned char	uchar;
struct port { union { bool b0:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; uchar B; }; };
#define	PTB	((port&)(PORTB))
	
	
// PWM管理
// 周期はOCRnx(n∈{A,B},x∈{0,1})で設定
// n=0は8bitで二重バッファ
// initで初期化. 引数の型のTCCRで設定数値
namespace	pwm
{
	// クロック設定. ps_(数値)は()内の数値で分周.
	enum PwmClock {
		ck_stop = 0, // stop
		ps_1, ps_8, ps_64, ps_256, ps_1024, // F_CPU/X. Xはps_の後の数値
		ck_fall, ck_raise // 外部クロック(T0)の下降/上昇端
	};

	// 出力形式. それぞれ 無し, トグル, 非反転, 反転
	enum PwmOut { pwmout_none = 0, pwmout_toggle, pwmout_noninv, pwmout_inv };

	// Waveform Generation Model.
	// 0:default, 1:phase(8bit), 2:CTC, 3:HighSpeedPWM(8bit), 5:phase, 7:HighSpeedPWM
	enum	WGM
	{
		wgm_default = 0,
		wgm_phase_8bit, 
		wgm_CTC,		// d=0.5で一定, 周期可変. 割り込み可.
		wgm_highspeed,	// 8bit高速PWM
		wgm_phase,
		wgm_pwm
	};
	
	// TCCRnA,TCCRnBを生成
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

	// 初期化
	template<class tccr_gen>
	void	init0 (void) { TCCR0A = tccr_gen::valueA; TCCR0B = tccr_gen::valueB; }
	template<class tccr_gen>
	void	init1 (void) { TCCR1A = tccr_gen::valueA; TCCR1B = tccr_gen::valueB; }

	// タイマーカウント割り込みを禁止
	void	disable_timer_interrupt(void) { TIMSK = 0; }

};


// アナログコンパレータ. V+<=>V- = (AIN0|Vref)<=>AIN1.
// Vrefは内部基準電圧1.1V. 安定まで40us.
namespace a_comp
{
	// コンパレータ無効化
	void	disable(void) { ACSR = 1 << ACD; }

	// 割り込み条件
	enum ACmpIntMD { int_tgl = 0, int_fall = 2, int_raise = 3 };

	// コンパレータ有効化
	template<
		bool comp_Vref, // 1でVref, 0でAIN0をV+に入力
		bool interrupt_flag, // 割り込み要求
		bool interrupt_enable, // 割り込み許可
		bool cap, // capture
		ACmpIntMD acimd // 割り込みのタイミング
	>
	void	enable(void) {
		ACSR = 
			(1 << ACD) | // 有効化
			(comp_Vref << ACBG) |
			(interrupt_flag << ACI) |
			(interrupt_enable << ACIE) |
			(cap << ACIC) |
			(acimd << ACIS0);
	}

	// 結果を読む. AIN1 < (AIN0|Vref) で比較.
	bool	read(void) { return (ACSR & (1<<ACO)) != 0; }
};




