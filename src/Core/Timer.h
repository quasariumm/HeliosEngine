#pragma once

namespace Engine
{

struct Timer
{
	typedef std::chrono::high_resolution_clock clock;

	Timer() { Reset(); }
    template<typename T>
	T Elapsed() const
	{
		const clock::time_point t2 = clock::now();
		const std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - start);
		return static_cast<T>(time_span.count());
	}
	void Reset() { start = clock::now(); }
	clock::time_point start;
};

}