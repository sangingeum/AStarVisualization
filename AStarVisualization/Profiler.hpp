#pragma once
#include <fstream>
#include <format>
#include <string_view>
#include <chrono>

namespace prof {
	// Singleton Profiler
	// RAII style
	class Profiler
	{
	private:
		std::ofstream m_outFile;
	public:
		// Times are given in microseconds
		static void processTrace(std::string_view name, long long duration, long long startTime, size_t threadID) {
			static Profiler profiler;
			profiler.writeTrace(name, duration, startTime, threadID);
		}
	private:
		Profiler(const std::string& fileName = "profile_result.json") {
			m_outFile.open(fileName);
			m_outFile << "{\"traceEvents\":[";
		}
		Profiler(const Profiler&) = delete;
		Profiler& operator=(const Profiler&) = delete;
		~Profiler() {
			size_t pos = m_outFile.tellp();
			// Erase the last comma
			m_outFile.seekp(pos - 1);
			m_outFile << "]}";
			m_outFile.flush();
			m_outFile.close();
		}
		void writeTrace(std::string_view name, long long duration, long long startTime, size_t threadID) {
			m_outFile << "{" + std::format("\"name\":\"{}\",\"dur\":{},\"ph\":\"X\",\"pid\":0,\"tid\":{},\"ts\":{}", name, duration, threadID, startTime) + "},";
		}
	};


	// InstrumentationTimer
	// RAII style
	class InstrumentationTimer {
	private:
		long long m_startTime;
		std::string_view m_name;
	public:
		InstrumentationTimer(std::string_view name)
			: m_startTime(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()), m_name(name) {
			// Fix start time
			static long long lastStartTime = 0;
			if (lastStartTime == m_startTime)
				++m_startTime;
			lastStartTime = m_startTime;
		}
		~InstrumentationTimer() {
			// Get endtime in microseconds
			auto endTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			// Get Thread id
			auto threadID = std::hash<std::thread::id>()(std::this_thread::get_id());

			// Process Trace
			Profiler::processTrace(m_name, endTime - m_startTime, m_startTime, threadID);
		}
	};
}


// Define macros for profiling
// Set PROFILE to 1 if you want to profile your code
// Set it to 0 otherwise
#define PROFILE 0
#if PROFILE
#define PROFILE_SCOPE(name) prof::InstrumentationTimer tracer##__LINE__(name)
#else
#define PROFILE_SCOPE(name)
#endif
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)
