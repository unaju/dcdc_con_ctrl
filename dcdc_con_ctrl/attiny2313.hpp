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
typedef	volatile uint8_t	vui8t; // OCRとかの型
	
	
// ポートにset/get
template<int number>
void	setB(bool bit) { PORTB = (PORTB & ~(1<<number)) | (bit << number); }
template<int number>
void	setD(bool bit) { PORTD = (PORTD & ~(1<<number)) | (bit << number); }
template<int number>
bool	getB(void) { return bit_is_set(PORTB,number); }
template<int number>
bool	getD(void) { return bit_is_set(PORTD,number); }


// PWM管理
// 周期はOCRnx(n∈{A,B},x∈{0,1})で設定
// n=0は8bitで二重バッファ
// initで初期化
// 周期・デューティは出力はOCRnxのxでのみ独立	？
// そのため出力A,Bでは反転か非反転かくらいしか変えられない	？
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
	enum	WGM
	{
		wgm_default = 0,
		wgm_phase_8bit, 
		wgm_CTC,		// d=0.5で一定, 周期可変. 割り込み可.
		wgm_highspeed,	// 8bit高速PWM. TOP=0xFF固定.
		wgm_phase = 5,
		wgm_fast = 7, // TOP=OCR0Aの高速PWM. 最速.
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
				( ((wgm >> 2) & 0x1)  << WGM02) ) | // 3bit目抽出
				((ck & 0x7) << CS00)
		};
	};

	// 初期化. クラスでの特殊化はメリットがあまりなかった.
	template<class tccr_gen>
	void	init0 (void) { TCCR0A = tccr_gen::valueA; TCCR0B = tccr_gen::valueB; }
	template<class tccr_gen>
	void	init1 (void) { TCCR1A = tccr_gen::valueA; TCCR1B = tccr_gen::valueB; }

	// タイマーカウント割り込みを禁止
	void	disable_timer_interrupt(void) { TIMSK = 0; }

	/*	8bit,周波数/デューティ可変のPWMを扱う. 出力ポートはOC0Bのみ.
		カウントが0～OCR0Aまでのため,トリガはOCR0Bでしか変更できない(=BにしかPWM出力できない)
		そのためOC0Aはnoneに設定.
		気の利いた名前は思いつかなかった. fullspeedってのもなんか長いし.
	*/
	template<PwmOut outB, PwmClock ck>
	struct fastpwm_8bit
	{
		typedef tccr_generator<pwmout_none, outB, wgm_fast, ck>	tccr_t;
		static void init(void) { init0<tccr_t>(); }

		// カウント最大値(OCR0A)を設定. PWM周波数は (AVR周波数/分周)/(OCR0A)になる.
		static void setMax(uint8_t max_v) { OCR0A = max_v; }
		// 波形折り返し点(OCR0B)を設定
		static void setVal(uint8_t val) { OCR0B = val; }
		// 0 <= val <= OCR0A までの数値にvalを区切ってから設定
		static void setVal(int val) { OCR0B = (val < 0) ? 0 : (val > OCR0A) ? OCR0A : val; }
	};
	
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
		// ピンをinputにして出力を0に
		static const int outpin = (1<<PB1) | (comp_Vref ? 0 : (1<<PB0));
		DDRB &= ~outpin; // inputに
		PORTB &= ~outpin; // portは0にしておかないと確実にHighZにならない

		// コンパレータの設定
		ACSR = 
			(comp_Vref << ACBG) |
			(interrupt_flag << ACI) |
			(interrupt_enable << ACIE) |
			(cap << ACIC) |
			(acimd << ACIS0);
	}

	// 結果を読む. AIN1 < (AIN0|Vref) で比較.
	bool	read(void) { return bit_is_set(ACSR,ACO); }
};




