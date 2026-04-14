#pragma once

#include <chrono>

namespace Aurora {

	class Timer {
	public:
		Timer() {
			Reset();
		}

		void Reset() {
			m_Start = std::chrono::high_resolution_clock::now();
		}

		float Elapsed() {
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001f;
		}

		float ElapsedMillis() {
			return Elapsed() * 1000.0f;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
	};

	class ScopedTimer {
	public:
		ScopedTimer(std::string_view name) : m_Name(name) {}
		~ScopedTimer() {
			float time = m_Timer.ElapsedMillis();
			AU_CORE_TRACE("{0} - {1}ms", m_Name, time);
		}
	private:
		Timer m_Timer;
		std::string m_Name;
	};

}

#if defined(__clang__) || defined(__GNUC__)
	#define AU_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
	#define AU_FUNC_SIG __FUNCSIG__
#else
	#define AU_FUNC_SIG __func__
#endif

#ifdef AU_PROFILE
	#define AU_PROFILE_FUNCTION() Aurora::ScopedTimer timer##__LINE__(AU_FUNC_SIG)
#else
	#define AU_PROFILE_FUNCTION()
#endif