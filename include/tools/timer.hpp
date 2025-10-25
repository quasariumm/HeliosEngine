#pragma once

namespace Engine
{
struct Timer
{
	using clock_t = std::chrono::high_resolution_clock;

	Timer() { Reset(); }


	template <typename T>
	T Elapsed() const
	{
		const clock_t::time_point t2       = clock_t::now();
		const auto                timeSpan = std::chrono::duration_cast<std::chrono::duration<
			double>>(t2 - start);
		return static_cast<T>(timeSpan.count());
	}


	void                Reset() { start = clock_t::now(); }
	clock_t::time_point start;
};
}
