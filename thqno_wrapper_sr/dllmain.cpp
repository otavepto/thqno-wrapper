// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "dbg_log.hpp"
#include "loadlib_hooks.hpp"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        dbg_init();
        loadlibs_hook_init(hModule);
    }
    break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        loadlibs_hook_deinit();
        dbg_close();
        break;
    }
    return TRUE;
}

