#include "helpers.hpp"
#include <algorithm>


const std::wstring get_exe_path()
{
	static bool initialized = false;
	static std::wstring path(4096, L'\0');
	if (!initialized) {
		GetModuleFileNameW(GetModuleHandleW(nullptr), &path[0], (DWORD)(path.size() / sizeof(path[0])));
		if (path[0]) {
			initialized = true;
			path = path.substr(0, path.find_last_of('\\') + 1);
		}
	}

	return path;
}

bool ends_with_i(std::wstring target, std::wstring query)
{
	if (target.size() < query.size()) {
		return false;
	}

	std::transform(target.begin(), target.end(), target.begin(),
		[](wchar_t c) { return std::tolower(c); });

	std::transform(query.begin(), query.end(), query.begin(),
		[](wchar_t c) { return std::tolower(c); });

	return target.compare(target.length() - query.length(), query.length(), query) == 0;

}

bool ends_with_i(PUNICODE_STRING target, std::wstring query)
{
	return ends_with_i(std::wstring(target->Buffer, (PWSTR)((char*)target->Buffer + target->Length)), query);
}

bool starts_with_i(std::string target, std::string query)
{
	if (target.size() < query.size()) {
		return false;
	}

	std::transform(target.begin(), target.end(), target.begin(),
		[](unsigned char c) { return std::tolower(c); });

	std::transform(query.begin(), query.end(), query.begin(),
		[](unsigned char c) { return std::tolower(c); });

	return target.compare(0, query.length(), query) == 0;

}

