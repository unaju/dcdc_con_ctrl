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


// 古いの
#if 0

// Waveform Generation Model.
// 0:default, 1:phase(8bit), 2:CTC, 3:HighSpeedPWM(8bit), 5:phase, 7:HighSpeedPWM
enum	WGM
{
	wgm_default = 0,	// 0-255まで増加. パルス長可変.
	wgm_phase_8bit = 1, 
	wgm_CTC = 2,		// d=0.5で一定, 周期可変. 割り込み可.
	wgm_pwm_8bit = 3,
	wgm_phase = 5,
	wgm_pwm = 7
};
	

// PWM出力形式設定
enum PwmOut
{
	pwmout_none = 0,
	pwmout_toggle,
	pwmout_h2l, // High to Low
	pwmout_l2h // Low to High
};
/*
	valueA,BをTCCRnA,Bに入れる
	out_type_a, _bはA,Bの出力設定(A,Bはxとする)
	nxとピンの対応は	0A:PB6, 0B:PD5, 1A:PB1, 1B:PB2, 2A:PB3, 2B:PD3
	デューティ比はOCRxnに0-255で設定する.
*/
template <
	PwmOut outA, PwmOut outB,
	WGM wgm, // 0:default, 1:phase(8bit), 2:CTC, 3:HighSpeedPWM(8bit), 5:phase, 7:HighSpeedPWM
	CkSep cs,
	int foc = 0 // 後で実装
> struct	pwm_setting
{
	// 設定する値. コンパイル時定数。
	static const int8_t
		valueA =
			((outA & 0x3) << COM0A0) |
			((outB & 0x3) << COM0B0) |
			((wgm & 0x3) << WGM00),
		valueB = 
			((foc & 0x3) << FOC0B) |
			((wgm & 0x4) << WGM02) |
			((cs & 0x7) << CS00);

	// portのDDRでの出力設定はしないため先にしておく必要アリ. initしたとこからPWM.
	static void	init0() { TCCR0A = valueA; TCCR0B = valueB; }
	//static void	init1() { TCCR1A = valueA; TCCR1B = valueB; }
	//static void	init2() { TCCR2A = valueA; TCCR2B = valueB; }
};
#endif

};
