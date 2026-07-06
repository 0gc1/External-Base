// zeugenschutz base | join discord.gg/x3solutions
#include <windows.h>
#include <chrono>
#include <thread>
#include <cstdio>

#include "mem.h"
#include "offsets.h"
#include "rbx.h"
#include "overlay.h"
#include "esp.h"
#include "menu.h"
#include "watermark.h"
#include "config.h"


static bool bootstrap()
{
    printf("[*] waiting for RobloxPlayerBeta.exe ...\n");
    while (!mem::attach(L"RobloxPlayerBeta.exe")) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    printf("[+] pid %lu  base 0x%llX\n", mem::dwPid, (unsigned long long)mem::base);

    std::string ver = mem::extract_version(mem::exe_path);
    if (ver.empty()) { printf("[-] version tag missing in path\n"); return false; }
    printf("[+] %s\n", ver.c_str());

    if (!off::fetch(ver)) { printf("[-] rbxoffsets fetch failed\n"); return false; }
    printf("[+] %zu offsets\n", off::map.size());

    if (!overlay::create()) { printf("[-] overlay init failed\n"); return false; }
    printf("[+] overlay up\n");
    return true;
}


int main()
{
    SetConsoleTitleW(L"zeugenschutz base");
    if (!bootstrap()) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return 1;
    }

    cfg::load();

    bool prev_ins = false;

    while (overlay::alive()) {
        bool ins = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;
        if (ins && !prev_ins) overlay::menu_open = !overlay::menu_open;
        prev_ins = ins;

        rbx::refresh();
        auto plist = rbx::players();

        overlay::begin_frame();
        esp::draw(plist);
        wm::draw();
        menu::draw(plist);
        overlay::end_frame();
    }

    cfg::save();
    overlay::destroy();
    mem::detach();
    return 0;
}
