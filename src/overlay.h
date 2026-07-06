#pragma once
#include <windows.h>
#include <d3d9.h>

struct ImFont;

namespace overlay {
    inline HWND hwnd = nullptr;
    inline HWND target = nullptr;
    inline IDirect3DDevice9* device = nullptr;
    inline IDirect3D9* d3d = nullptr;
    inline bool menu_open = true;

    inline ImFont* font_body = nullptr;
    inline ImFont* font_head = nullptr;

    bool create();
    void destroy();
    void begin_frame();
    void end_frame();
    bool alive();
}

