#pragma once

namespace alg
{
	template<typename T>	T min(T a, T b) { return (a < b) ? a : b; }
	template<typename T>	T max(T a, T b) { return (a > b) ? a : b; }

	template<typename T>	T abs(T val) { return (val > 0) ? val : -val; } 
};


