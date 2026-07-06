// zeugenschutz base | join discord.gg/x3solutions
#include "overlay.h"
#include "mem.h"
#include "theme.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"
#include <dwmapi.h>
#include <tlhelp32.h>
#include <string>
#include <cctype>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);


namespace overlay {

static WNDCLASSEXW wc{};
static D3DPRESENT_PARAMETERS pp{};
static bool running = true;
static bool device_lost = false;

static void reset_device()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = device->Reset(&pp);
    if (hr == D3DERR_INVALIDCALL) return;
    ImGui_ImplDX9_CreateDeviceObjects();
}

static LRESULT WINAPI wnd_proc(HWND h, UINT m, WPARAM wp, LPARAM lp)
{
    if (ImGui_ImplWin32_WndProcHandler(h, m, wp, lp)) return true;
    switch (m) {
        case WM_SIZE:
            if (device && wp != SIZE_MINIMIZED) {
                pp.BackBufferWidth  = LOWORD(lp);
                pp.BackBufferHeight = HIWORD(lp);
                device_lost = true;
            }
            return 0;
        case WM_DESTROY: PostQuitMessage(0); running = false; return 0;
    }
    return DefWindowProcW(h, m, wp, lp);
}


struct find_ctx { DWORD pid; HWND best; int area; bool need_vis; };

static BOOL CALLBACK enum_by_pid(HWND h, LPARAM lp)
{
    find_ctx* c = (find_ctx*)lp;
    if (c->need_vis && !IsWindowVisible(h)) return TRUE;

    DWORD wp = 0;
    GetWindowThreadProcessId(h, &wp);
    if (wp != c->pid) return TRUE;

    RECT rc;
    if (!GetClientRect(h, &rc)) return TRUE;
    int a = (rc.right - rc.left) * (rc.bottom - rc.top);
    int s = a > 0 ? a : 1;
    if (s > c->area) { c->area = s; c->best = h; }
    return TRUE;
}

struct tenum_ctx { HWND best; int area; };
static BOOL CALLBACK enum_thread_wnd(HWND h, LPARAM lp)
{
    tenum_ctx* c = (tenum_ctx*)lp;
    RECT rc;
    if (!GetClientRect(h, &rc)) return TRUE;
    int a = (rc.right - rc.left) * (rc.bottom - rc.top);
    int s = a > 0 ? a : 1;
    if (s > c->area) { c->area = s; c->best = h; }
    return TRUE;
}

static HWND find_by_thread_enum(DWORD pid)
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snap == INVALID_HANDLE_VALUE) return NULL;

    THREADENTRY32 te{};
    te.dwSize = sizeof(te);
    HWND best = NULL;
    int best_area = 0;

    if (Thread32First(snap, &te)) {
        do {
            if (te.th32OwnerProcessID != pid) continue;
            tenum_ctx tc{ NULL, 0 };
            EnumThreadWindows(te.th32ThreadID, enum_thread_wnd, (LPARAM)&tc);
            if (tc.area > best_area) { best_area = tc.area; best = tc.best; }
        }
        while (Thread32Next(snap, &te));
    }
    CloseHandle(snap);
    return best;
}

static std::string exe_name_lower(DWORD pid)
{
    if (!pid) return {};
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return {};
    PROCESSENTRY32W pe{}; pe.dwSize = sizeof(pe);
    std::string out;
    if (Process32FirstW(snap, &pe)) {
        do {
            if (pe.th32ProcessID == pid) {
                for (const wchar_t* p = pe.szExeFile; *p; ++p)
                    out.push_back((char)tolower((unsigned char)(*p & 0x7F)));
                break;
            }
        }
        while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return out;
}

struct title_ctx { HWND best; int area; DWORD pid; HWND ignore; };

static BOOL CALLBACK enum_by_title(HWND h, LPARAM lp)
{
    title_ctx* c = (title_ctx*)lp;
    if (h == c->ignore) return TRUE;
    if (!IsWindowVisible(h)) return TRUE;

    char title[128] = { 0 };
    int len = GetWindowTextA(h, title, sizeof(title));
    if (len <= 0) return TRUE;

    int a = 0, b = len - 1;
    while (a <= b && (title[a] == ' ' || title[a] == '\t')) ++a;
    while (b >= a && (title[b] == ' ' || title[b] == '\t')) --b;
    int tl = b - a + 1;
    if (tl <= 0 || tl > 64) return TRUE;

    // substring "roblox" case-insensitive — X3/Wave/Fluxus bootstrapper
    // rename to "X3 Roblox" etc, so exact match would reject them.
    bool hit = false;
    for (int i = a; i + 6 <= a + tl; ++i) {
        if (_strnicmp(title + i, "roblox", 6) == 0) { hit = true; break; }
    }
    if (!hit) return TRUE;

    RECT rc;
    if (!GetClientRect(h, &rc)) return TRUE;
    int cw = rc.right - rc.left;
    int ch = rc.bottom - rc.top;
    if (cw < 300 || ch < 200) return TRUE;

    DWORD wp = 0;
    GetWindowThreadProcessId(h, &wp);
    if (!wp) return TRUE;

    std::string exe = exe_name_lower(wp);
    if (exe.rfind("roblox", 0) != 0) return TRUE;
    if (exe.find("studio") != std::string::npos) return TRUE;

    int ar = cw * ch;
    if (ar > c->area) { c->area = ar; c->best = h; c->pid = wp; }
    return TRUE;
}

static HWND find_by_title(DWORD* out_pid)
{
    title_ctx c{ NULL, 0, 0, hwnd };
    EnumWindows(enum_by_title, (LPARAM)&c);
    if (out_pid) *out_pid = c.pid;
    return c.best;
}


static HWND find_roblox()
{
    DWORD pid = mem::dwPid;

    // pass 1 — happy path
    if (pid != 0) {
        find_ctx c1{ pid, NULL, 0, true };
        EnumWindows(enum_by_pid, (LPARAM)&c1);
        if (c1.best) return c1.best;
    }

    // pass 4 — BEFORE 2/3 so zombie/wrong-PID gets patched before
    // pass 2 latches onto an invisible helper in the dead process.
    {
        DWORD found = 0;
        HWND h = find_by_title(&found);
        if (h) {
            if (found && found != mem::dwPid) mem::dwPid = found;
            return h;
        }
    }

    // pass 2 — drop visible requirement
    if (pid != 0) {
        find_ctx c2{ pid, NULL, 0, false };
        EnumWindows(enum_by_pid, (LPARAM)&c2);
        if (c2.best) return c2.best;

        // pass 3 — thread-window enum
        HWND h = find_by_thread_enum(pid);
        if (h) return h;
    }

    return NULL;
}


bool create()
{
    target = find_roblox();
    if (!target) { printf("[-] roblox window not found\n"); return false; }

    wc = { sizeof(wc), CS_CLASSDC, wnd_proc, 0, 0, GetModuleHandleW(NULL), NULL, NULL, NULL, NULL, L"zsov", NULL };
    RegisterClassExW(&wc);

    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);

    hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT,
        wc.lpszClassName, L"", WS_POPUP,
        0, 0, sw, sh,
        NULL, NULL, wc.hInstance, NULL);
    if (!hwnd) { printf("[-] CreateWindowExW failed err=%lu\n", GetLastError()); return false; }

    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    MARGINS m = { -1 };
    DwmExtendFrameIntoClientArea(hwnd, &m);


    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d) return false;

    pp.Windowed = TRUE;
    pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    pp.BackBufferFormat = D3DFMT_A8R8G8B8;
    pp.EnableAutoDepthStencil = TRUE;
    pp.AutoDepthStencilFormat = D3DFMT_D16;
    pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
                          D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &device) < 0) return false;

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr;

    {
        char wdir[MAX_PATH];
        UINT n = GetWindowsDirectoryA(wdir, MAX_PATH);
        if (n > 0 && n < MAX_PATH) {
            std::string fonts = std::string(wdir) + "\\Fonts\\";
            auto try_load = [&](const char* file, float sz) -> ImFont* {
                std::string p = fonts + file;
                if (GetFileAttributesA(p.c_str()) == INVALID_FILE_ATTRIBUTES) return nullptr;
                return io.Fonts->AddFontFromFileTTF(p.c_str(), sz);
            };
            font_body = try_load("segoeui.ttf", 15.f);
            if (!font_body) font_body = try_load("bahnschrift.ttf", 15.f);
            font_head = try_load("segoeuib.ttf", 20.f);
            if (!font_head) font_head = try_load("bahnschrift.ttf", 20.f);
            if (font_body) io.FontDefault = font_body;
        }
    }

    theme::apply();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(device);
    return true;
}

void destroy() {
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (device) { device->Release(); device = nullptr; }
    if (d3d)    { d3d->Release();    d3d    = nullptr; }
    if (hwnd)   DestroyWindow(hwnd);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
}


bool alive()
{
    MSG msg;
    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (!IsWindow(target)) {
        target = find_roblox();
        if (!target) return false;
    }

    RECT rc; GetClientRect(target, &rc);
    POINT tl{ 0, 0 }; ClientToScreen(target, &tl);
    int rw = rc.right - rc.left;
    int rh = rc.bottom - rc.top;
    if (rw > 0 && rh > 0 && !IsIconic(target)) {
        SetWindowPos(hwnd, HWND_TOPMOST, tl.x, tl.y, rw, rh, SWP_NOACTIVATE | SWP_NOSENDCHANGING);
    }

    LONG ex = GetWindowLongW(hwnd, GWL_EXSTYLE);
    if (menu_open) SetWindowLongW(hwnd, GWL_EXSTYLE, ex & ~WS_EX_TRANSPARENT);
    else           SetWindowLongW(hwnd, GWL_EXSTYLE, ex |  WS_EX_TRANSPARENT);

    return running;
}

void begin_frame() {
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}
void end_frame()
{
    ImGui::EndFrame();

    if (device_lost) {
        HRESULT hr = device->TestCooperativeLevel();
        if (hr == D3DERR_DEVICELOST) { Sleep(10); return; }
        if (hr == D3DERR_DEVICENOTRESET) reset_device();
        device_lost = false;
    }

    device->SetRenderState(D3DRS_ZENABLE,           FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE,  FALSE);
    device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0);

    if (device->BeginScene() >= 0) {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }
    HRESULT pr = device->Present(NULL, NULL, NULL, NULL);
    if (pr == D3DERR_DEVICELOST) device_lost = true;
}

}
