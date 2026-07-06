// zeugenschutz base | join discord.gg/x3solutions
#include "anim.h"
#include "imgui.h"
#include <cmath>
#include <unordered_map>

namespace anim {

struct S { float v; float vel; };
static std::unordered_map<uint32_t, S>        table;
static std::unordered_map<uint32_t, RGB>      colors;

float smooth(uint32_t id, float target, float speed) {
    auto& s = table[id];
    float dt = ImGui::GetIO().DeltaTime;
    if (dt > 0.1f) dt = 0.1f;
    s.v += (target - s.v) * (1.0f - expf(-speed * dt));
    return s.v;
}

float spring(uint32_t id, float target, float k, float damp) {
    auto& s = table[id];
    float dt = ImGui::GetIO().DeltaTime;
    if (dt > 0.05f) dt = 0.05f;
    float acc = (target - s.v) * k - s.vel * damp;
    s.vel += acc * dt;
    s.v   += s.vel * dt;
    return s.v;
}

RGB smooth_color(uint32_t id, const RGB& target, float speed) {
    auto& c = colors[id];
    float dt = ImGui::GetIO().DeltaTime;
    if (dt > 0.1f) dt = 0.1f;
    float k = 1.0f - expf(-speed * dt);
    c.r += (target.r - c.r) * k;
    c.g += (target.g - c.g) * k;
    c.b += (target.b - c.b) * k;
    c.a += (target.a - c.a) * k;
    return c;
}

void clear() {
    table.clear();
    colors.clear();
}

}
