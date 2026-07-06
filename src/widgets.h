// zeugenschutz base | join discord.gg/x3solutions
#pragma once
#include "imgui.h"

namespace w {
    void card_begin(const char* title, float height);
    void card_end();

    bool toggle(const char* label, bool* val);
    bool slider(const char* label, float* val, float vmin, float vmax, const char* fmt = "%.1f");
}
