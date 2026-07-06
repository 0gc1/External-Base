// zeugenschutz base | join discord.gg/x3solutions
#include "mem.h"
#include <tlhelp32.h>


namespace mem {

static DWORD find_pid(const wchar_t* name)
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32W e{};
    e.dwSize = sizeof(e);
    DWORD hit = 0;
    if (Process32FirstW(snap, &e)) {
        do {
            if (_wcsicmp(e.szExeFile, name) == 0) { hit = e.th32ProcessID; break; }
        }
        while (Process32NextW(snap, &e));
    }

    CloseHandle(snap);
    return hit;
}


static uintptr_t find_base(DWORD p, std::wstring* out_path)
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32, p);
    if(snap == INVALID_HANDLE_VALUE) return 0;

    MODULEENTRY32W m{};
    m.dwSize = sizeof(m);
    uintptr_t b = 0;

    if (Module32FirstW(snap, &m)) {
        b = (uintptr_t)m.modBaseAddr;
        if (out_path) *out_path = m.szExePath;
    }
    CloseHandle(snap);
    return b;
}
bool attach(const wchar_t* name) {
    dwPid = find_pid(name);
    if (!dwPid) return false;

    hProc = OpenProcess(PROCESS_VM_READ|PROCESS_VM_WRITE|PROCESS_VM_OPERATION|PROCESS_QUERY_INFORMATION, FALSE, dwPid);
    if (!hProc) return false;

    base = find_base(dwPid, &exe_path);
    return base != 0;
}

void detach()
{
    if (hProc) CloseHandle(hProc);
    hProc = NULL;
    dwPid = 0;
    base  = 0;
    exe_path.clear();
}

std::string extract_version(const std::wstring& path) {
    auto pos = path.find(L"version-");
    if (pos == std::wstring::npos) return {};
    auto end = path.find(L'\\', pos);
    auto slice = (end == std::wstring::npos) ? path.substr(pos) : path.substr(pos, end - pos);

    std::string out;
    out.reserve(slice.size());


    for (wchar_t c : slice) out.push_back((char)(c & 0x7F));
    return out;
}


std::string read_string(uintptr_t addr) {
    char buf[128]{};
    ReadProcessMemory(hProc, (LPCVOID)addr, buf, sizeof(buf) - 1, NULL);
    return std::string(buf);
}
std::string read_lenstr(uintptr_t addr)
{
    size_t len = read<size_t>(addr + 0x10);
    if (len == 0 || len > 256) return {};
    uintptr_t data = (len >= 16) ? read<uintptr_t>(addr) : addr;

    std::string s;
    s.resize(len);
    ReadProcessMemory(hProc, (LPCVOID)data, s.data(), len, NULL);
    return s;
}

}
