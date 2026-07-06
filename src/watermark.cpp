// zeugenschutz base | join discord.gg/x3solutions
#include "watermark.h"
#include "offsets.h"
#include "imgui.h"
#include <cstdio>
#include <ctime>


namespace wm {

void draw()
{
    if (!enabled) return;

    ImGuiIO& io = ImGui::GetIO();

    time_t t = time(NULL);
    tm lt;
    localtime_s(&lt, &t);

    const char* ver = off::version.empty() ? "?" : off::version.c_str();
    char line[96];
    snprintf(line, sizeof(line),
             "zeugenschutz   roblox %s   %02d:%02d:%02d",
             ver, lt.tm_hour, lt.tm_min, lt.tm_sec);


    ImVec2 ts = ImGui::CalcTextSize(line);
    float pad_x = 10.f;
    float pad_y = 6.f;
    float w = ts.x + pad_x*2.f;
    float h = ts.y + pad_y*2.f;
    float x = io.DisplaySize.x - w - 14.f;
    float y = 14.f;

    auto* dl = ImGui::GetForegroundDrawList();
    dl->AddRectFilled({ x, y }, { x + w, y + h }, IM_COL32(11, 12, 15, 215), 6.f);
    dl->AddRect({ x, y }, { x + w, y + h }, IM_COL32(242, 78, 107, 140), 6.f, 0, 1.f);
    dl->AddText({ x + pad_x, y + pad_y }, IM_COL32(232, 232, 236, 255), line);
}

}
