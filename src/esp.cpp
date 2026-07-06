// zeugenschutz base | join discord.gg/x3solutions
#include "esp.h"
#include "mem.h"
#include "offsets.h"
#include "overlay.h"
#include "imgui.h"
#include <cmath>


namespace esp {

static bool focused()
{
    HWND fg = GetForegroundWindow();
    return fg == overlay::target || fg == overlay::hwnd;
}


void draw(const std::vector<rbx::Player>& list) {
    if (!enabled) return;
    if (!rbx::visual_eng) return;
    if (!focused()) return;

    Mat4 mv = rbx::view_matrix();
    ImGuiIO& io = ImGui::GetIO();
    float sw = io.DisplaySize.x;
    float sh = io.DisplaySize.y;

    Vec3 cam{};
    if (rbx::camera && off::CameraPos)
        cam = mem::read<Vec3>(rbx::camera + off::CameraPos);

    auto* dl = ImGui::GetBackgroundDrawList();

    for (const auto& p : list) {
        if (!p.hrp) continue;
        if (p.health <= 0.f) continue;
        bool me = (p.addr == rbx::local_player);
        if (me && !self) continue;

        Vec3 delta = p.pos - cam;
        float dist = sqrtf(delta.dot(delta));
        if (dist > max_dist) continue;

        Vec3 head = { p.pos.x, p.pos.y + 3.f,  p.pos.z };
        Vec3 feet = { p.pos.x, p.pos.y - 3.5f, p.pos.z };

        Vec2 sh_top, sh_bot;
        if (!rbx::w2s(head, sh_top, mv, sw, sh)) continue;
        if (!rbx::w2s(feet, sh_bot, mv, sw, sh)) continue;

        float bh = sh_bot.y - sh_top.y;
        if (bh < 4.f) continue;

        float bw = bh * 0.5f;
        float bx = sh_top.x - bw*0.5f;
        float by = sh_top.y;


        if (box) {
            ImU32 c   = p.friendly ? IM_COL32(60,220,100,255) : IM_COL32(242,78,107,255);
            ImU32 sha = IM_COL32(0,0,0,180);
            dl->AddRect({bx-1, by-1}, {bx+bw+1, by+bh+1}, sha, 0.f, 0, 1.f);
            dl->AddRect({bx+1, by+1}, {bx+bw-1, by+bh-1}, sha, 0.f, 0, 1.f);
            dl->AddRect({bx, by}, {bx+bw, by+bh}, c, 0.f, 0, 1.4f);
        }
        if (name_tag && !p.name.empty()) {
            const char* n = p.name.c_str();
            ImVec2 tsz = ImGui::CalcTextSize(n);
            float tx = bx + bw*0.5f - tsz.x*0.5f;
            float ty = by - tsz.y - 2.f;
            dl->AddText({tx+1, ty+1}, IM_COL32(0,0,0,210), n);
            dl->AddText({tx, ty}, IM_COL32(240,240,245,255), n);
        }
    }
}

}
