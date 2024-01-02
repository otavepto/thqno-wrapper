#include "pch.h"
#include "dbg_log.hpp"
#include "helpers.hpp"
#include <fstream>
#include <cstdarg>
#include <chrono>

FILE* out_file;

auto const static start_time = std::chrono::system_clock::now();

void dbg_init()
{

#ifdef THQNO_DBG
	auto path = get_exe_path();
	if (path.size()) {
		path = path + L"DBG_THQNO_WRAPPER.txt";
	}
	else {
		MessageBoxA(nullptr, "Failed to get exe path", "THQNO_Wrapper", MB_OK);
		ExitProcess(1);
	}

	auto err = _wfopen_s(&out_file, path.c_str(), L"a");
	if (err) {
		MessageBoxA(nullptr, "Failed to open dbg log file", "THQNO_Wrapper", MB_OK);
		ExitProcess(1);
	}

	fprintf_s(out_file, "*** Log file opened ***\n");
	fflush(out_file);
#endif

}

void dbg_write(const char *fmt, ...)
{

#ifdef THQNO_DBG
	if (out_file) {
		auto elapsed = std::chrono::system_clock::now() - start_time;
		fprintf_s(out_file, "[%llu ms] [tid: %u] ",
			std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count(),
			GetCurrentThreadId());

		va_list args;
		va_start(args, fmt);
		vfprintf_s(out_file, fmt, args);
		va_end(args);

		fprintf_s(out_file, "\n");
		fflush(out_file);
	}
#endif

}

void dbg_close()
{

#ifdef THQNO_DBG
	if (out_file) {
		fprintf_s(out_file, "\nLog file closed\n\n");
		fclose(out_file);
	}
#endif

}
