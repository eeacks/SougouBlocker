#include <Windows.h>
#include <winhttp.h>

#include "minhook/include/MinHook.h"
#include "Hijack.h"

const wchar_t* pBlackListedProcess[] = {
    L"SGBizLauncher.exe", L"SogouCloud.exe", L"SGPicFaceTool.exe", L"PinyinUp.exe",
    L"SGMedalLoader.exe", L"userNetSchedule.exe", L"SGDownload.exe", L"SGWebRender.exe", 
    L"SGWangzai.exe", L"SGSmartAssistant.exe",

    // Path
    L"SogouInput\\Components", L"SogouInput\\SogouExe"
};
const char* pSuspectModules[] = { "SGCurlHelper.dll" };

void SuspendCurrentProcess()
{
    using fnNtSuspendProcess = NTSTATUS(__stdcall*)(HANDLE);
    static fnNtSuspendProcess NtSuspendProcess = (fnNtSuspendProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtSuspendProcess");
    NtSuspendProcess((HANDLE)-1);
}

void DieAtExecute(void* pModuleBase)
{
    IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)pModuleBase;
    IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)((uintptr_t)pModuleBase + dos->e_lfanew);

    // not sure if this is proper
    DWORD old;
    if (!VirtualProtect((void*)((uintptr_t)pModuleBase + 0x1000), nt->OptionalHeader.SizeOfImage - 0x1000, PAGE_NOACCESS, &old))
    {
        ExitProcess(0); // dont risk
    }
    return;
}

HINTERNET WINAPI hkWinHttpOpen
(
    _In_opt_z_ LPCWSTR pszAgentW,
    _In_ DWORD dwAccessType,
    _In_opt_z_ LPCWSTR pszProxyW,
    _In_opt_z_ LPCWSTR pszProxyBypassW,
    _In_ DWORD dwFlags
)
{
    return 0;
}

HINTERNET WINAPI hkHttpOpenRequestA(
    _In_ HINTERNET hConnect,
    _In_opt_ LPCSTR lpszVerb,
    _In_opt_ LPCSTR lpszObjectName,
    _In_opt_ LPCSTR lpszVersion,
    _In_opt_ LPCSTR lpszReferrer,
    _In_opt_z_ LPCSTR FAR* lplpszAcceptTypes,
    _In_ DWORD dwFlags,
    _In_opt_ DWORD_PTR dwContext
)
{
    return 0;
}

SOCKET __stdcall hksocket(int af, int type, int protocol)
{
    return SOCKET_ERROR;
}

using fnCreateProcessW = BOOL(__stdcall*)(LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);
fnCreateProcessW pCreateProcessW;
BOOL __stdcall hkCreateProcessW(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
    for (int i = 0; i < sizeof(pBlackListedProcess) / sizeof(const char*); i++)
    {
        if (wcsstr(lpCommandLine, pBlackListedProcess[i]))
        {
            return false;
        }
    }

    return pCreateProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}


void Thread()
{
    const wchar_t* pCommandLine = GetCommandLineW();
    for (int i = 0; i < sizeof(pBlackListedProcess) / sizeof(const char*); i++)
    {
        if (wcsstr(pCommandLine, pBlackListedProcess[i]))
        {
            ExitProcess(0);
            // SuspendCurrentProcess();
            return; // lol
        }
    }

    for (int i = 0; i < sizeof(pSuspectModules) / sizeof(const char*); i++)
    {
        void* pSuspectModule = GetModuleHandleA(pSuspectModules[i]);
        if (pSuspectModule)
        {
            DieAtExecute(pSuspectModule);
        }
    }
    MH_Initialize();
    void* _DiscardedOG = nullptr;
    if (GetModuleHandleA("winhttp.dll"))
    {
        MH_CreateHookApi(L"winhttp.dll", "WinHttpOpen", hkWinHttpOpen, &_DiscardedOG);
    }
    if (GetModuleHandleA("wininet.dll"))
    {
        MH_CreateHookApi(L"wininet.dll", "HttpOpenRequestA", hkHttpOpenRequestA, &_DiscardedOG);
    }
    if (GetModuleHandleA("ws2_32.dll"))
    {
        MH_CreateHookApi(L"ws2_32.dll", "socket", hksocket, &_DiscardedOG);
    }
    if (GetModuleHandleA("kernel32.dll"))
    {
        MH_CreateHookApi(L"kernel32.dll", "CreateProcessW", hkCreateProcessW, (void**)&pCreateProcessW);
    }
    MH_EnableHook(MH_ALL_HOOKS);
    return;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if(ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        VersionHijack::Initialize();
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Thread, 0, 0, 0);
    }
    return TRUE;
}

