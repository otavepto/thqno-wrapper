#pragma once
#include "pch.h"
#include <string>
#include <winternl.h>

const std::wstring get_exe_path();

// ends with, case insensitive
bool ends_with_i(std::wstring target, std::wstring query);

// ends with, case insensitive
bool ends_with_i(PUNICODE_STRING target, std::wstring query);

// starts with, case insensitive
bool starts_with_i(std::string target, std::string query);
