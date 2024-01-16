#include "pch.h"

namespace masxtra {

	// both ctime and localtime have thread-safety and/or re-entrancy issues.  This function
	// is not immune to this since it uses localtime.  This method is not production code.
	std::string ctime(const time_t& time)
	{
		static const char* time_format("%a %b %d %H:%M:%S %Y");
		static const size_t size = 1024;
		char buffer[size];
		size_t success;

#if defined(_MSC_VER)
        struct tm temp_tm;
		int err = localtime_s(&temp_tm, &time);
		if (err != 0) return time_format;
		success = strftime(buffer, size, time_format, &temp_tm);

#elif defined(_GNUC_)
        struct tm temp_tm;
		localtime_r(&time, &temp_tm));
		success = strftime(buffer, size, time_format, &temp_tm);

#else
		success = strftime(buffer, size, time_format, localtime(&time));
#endif

		if (0 == success) return time_format;

		return buffer;
	}

}


