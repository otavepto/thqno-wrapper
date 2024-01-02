#include "loadlib_hooks.hpp"
#include <winternl.h>
#include "dbg_log.hpp"
#include "helpers.hpp"
#include "MinHook.h"


static const wchar_t original_dll_w[] = L"wrapper\\THQNOnline\\steam\\steam_api64.dll";
static const wchar_t this_dll_w[]     = L"steam_api64.dll";
static const wchar_t thqno_dll_w[]    = L"thqno_api64.dll";

static HMODULE this_hmod;
static HMODULE original_hmod;
static void *NtCreateFile_base_addr;

decltype(GetProcAddress)* oGetProcAddress;
FARPROC WINAPI GetProcAddress_hook(
	HMODULE hModule,
	LPCSTR  lpProcName
)
{
	auto to_load = hModule;
	if (lpProcName && lpProcName[0]) {
		if (starts_with_i(lpProcName, "steam")) {
			to_load = original_hmod;
			dbg_write("%s() redirecting steam export '%s'", __FUNCTION__, lpProcName);
		}
	}

	return oGetProcAddress(to_load, lpProcName);
}

decltype(NtCreateFile)* oNtCreateFile;
NTSTATUS
NTAPI
NtCreateFile_hook(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	PLARGE_INTEGER AllocationSize,
	ULONG FileAttributes,
	ULONG ShareAccess,
	ULONG CreateDisposition,
	ULONG CreateOptions,
	PVOID EaBuffer,
	ULONG EaLength
)
{
	if (ObjectAttributes && ObjectAttributes->ObjectName && ObjectAttributes->ObjectName->Buffer && ObjectAttributes->ObjectName->Buffer[0]) {
		if (ends_with_i(ObjectAttributes->ObjectName, this_dll_w)) {
			memcpy_s(
				(char*)ObjectAttributes->ObjectName->Buffer + ObjectAttributes->ObjectName->Length - (sizeof(this_dll_w) - sizeof(this_dll_w[0])),
				sizeof(this_dll_w) - sizeof(this_dll_w[0]),
				thqno_dll_w,
				sizeof(thqno_dll_w) - sizeof(thqno_dll_w[0])
			);
			dbg_write("%s() redirecting steam API file", __FUNCTION__);
			// the devs left this message
			dbg_write("---");
			dbg_write("Have you ever heard the tragedy of Darth Plagueis the Wise? I thought not, it's not a story the Jedi would tell you...");
			dbg_write("---");
		}
	}

	return oNtCreateFile(
		FileHandle,
		DesiredAccess,
		ObjectAttributes,
		IoStatusBlock,
		AllocationSize,
		FileAttributes,
		ShareAccess,
		CreateDisposition,
		CreateOptions,
		EaBuffer,
		EaLength);
}



void loadlibs_hook_init(HMODULE my_hmod)
{
	this_hmod = my_hmod;

	std::wstring thqno_lib = get_exe_path() + thqno_dll_w;
	auto att = GetFileAttributesW(thqno_lib.c_str());
	if (att == FILE_ATTRIBUTE_DIRECTORY || att == INVALID_FILE_ATTRIBUTES) {
		dbg_write("%s() couldn't find thqno dll", __FUNCTION__);
		std::wstring err = std::wstring(L"Couldn't find thqno dll ") + thqno_lib;
		MessageBoxW(nullptr, err.c_str(), L"THQNO_Wrapper", MB_OK);
		ExitProcess(1);
	}

	std::wstring original_lib = get_exe_path() + original_dll_w;
	original_hmod = LoadLibraryW(original_lib.c_str());
	if (!original_hmod) {
		dbg_write("%s() failed to load original steam dll", __FUNCTION__);
		std::wstring err = std::wstring(L"Couldn't load original dll ") + original_lib;
		MessageBoxW(nullptr, err.c_str(), L"THQNO_Wrapper", MB_OK);
		ExitProcess(1);
	}

	MH_Initialize();

	auto ntdll_hmod = LoadLibraryA("ntdll.dll");
	if (!ntdll_hmod) {
		dbg_write("%s() failed to load ntdll.dll", __FUNCTION__);
		MessageBoxA(nullptr, "Couldn't load ntdll.dll", "THQNO_Wrapper", MB_OK);
		ExitProcess(1);
	}

	NtCreateFile_base_addr = GetProcAddress(ntdll_hmod, "NtCreateFile");
	if (!NtCreateFile_base_addr) {
		dbg_write("%s() failed to GetProcAddress of NtCreateFile", __FUNCTION__);
		MessageBoxA(nullptr, "Couldn't GetProcAddress of NtCreateFile", "THQNO_Wrapper", MB_OK);
		ExitProcess(1);
	}

	int res = 0;
	res |= (int)MH_CreateHook(NtCreateFile_base_addr, NtCreateFile_hook, (LPVOID*)&oNtCreateFile);
	res |= (int)MH_CreateHook(GetProcAddress, GetProcAddress_hook, (LPVOID*)&oGetProcAddress);
	if (res != MH_STATUS::MH_OK) {
		dbg_write("%s() failed to hook APIs", __FUNCTION__);
		MessageBoxA(nullptr, "Couldn't hook APIs", "THQNO_Wrapper", MB_OK);
		ExitProcess(1);
	}

	res = MH_QueueEnableHook(MH_ALL_HOOKS);
	if (res != MH_STATUS::MH_OK) {
		dbg_write("%s() failed to queue APIs hooks", __FUNCTION__);
		MessageBoxA(nullptr, "Couldn't queue APIs hooks", "THQNO_Wrapper", MB_OK);
		ExitProcess(1);
	}

	res = MH_EnableHook(MH_ALL_HOOKS);
	if (res != MH_STATUS::MH_OK) {
		dbg_write("%s() failed to enable APIs hooks", __FUNCTION__);
		MessageBoxA(nullptr, "Couldn't enable APIs hooks", "THQNO_Wrapper", MB_OK);
		ExitProcess(1);
	}
}

void loadlibs_hook_deinit()
{
	FreeLibrary(original_hmod);

	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(NtCreateFile_base_addr);
	MH_RemoveHook(GetProcAddress);
	MH_Uninitialize();
}