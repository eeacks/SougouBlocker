#include <Windows.h>
#include <winhttp.h>

#include "minhook/include/MinHook.h"
#include "Hijack.h"

const char* pBlackListedProcess[] = { "SGBizLauncher.exe", "SogouCloud.exe", "SGPicFaceTool.exe", "PinyinUp.exe", "SGMedalLoader.exe", "userNetSchedule.exe", "SGDownload.exe" };
const char* pSuspectModules[] = { "SGCurlHelper.dll" };

void SuspendCurrentProcess()
{
    using fnNtSuspendProcess = NTSTATUS(__stdcall*)(HANDLE);
    static fnNtSuspendProcess NtSuspendProcess = (fnNtSuspendProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtSuspendProcess");
    NtSuspendProcess((HANDLE)-1);
}

void SetDieAtExecute(void* pModuleBase)
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

HINTERNET
WINAPI
hkWinHttpOpen
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

void Thread()
{
    const char* pCommandLine = GetCommandLineA();
    for (int i = 0; i < sizeof(pBlackListedProcess) / sizeof(const char*); i++)
    {
        if (strstr(pCommandLine, pBlackListedProcess[i]))
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
            SetDieAtExecute(pSuspectModule);
        }
    }

    MH_Initialize();
    void* _DiscardedOG = nullptr;
    bool bAnyHook = false;
    if (GetModuleHandleA("winhttp.dll"))
    {
        MH_CreateHookApi(L"winhttp.dll", "WinHttpOpen", hkWinHttpOpen, &_DiscardedOG);
        bAnyHook = true;
    }
    if (GetModuleHandleA("wininet.dll"))
    {
        MH_CreateHookApi(L"wininet.dll", "HttpOpenRequestA", hkHttpOpenRequestA, &_DiscardedOG);
        bAnyHook = true;
    }
    if(bAnyHook)
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

