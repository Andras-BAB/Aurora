#ifdef _WIN32
	#ifdef _WIN64
		#define AURORA_WINDOWS_PLATFORM
	#else
		#error "x86 Builds are not supported!"
	#endif

#elif defined(__linux__)
	#define AURORA_LINUX_PLATFORM
	#error "Linux currently not supported!"

#elif defined(__APPLE__) || defined(__MACH__)
	#error "Apple platforms are not supported!"

#elif defined(__ANDROID__)
	#define AURORA_ANDROID_PLATFORM
	#error "Android is not supported!"

#else
	#error "Unknown platform!"

#endif
