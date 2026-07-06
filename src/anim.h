// zeugenschutz base | join discord.gg/x3solutions
#pragma once
#include <cstdint>

namespace anim {
    float smooth(uint32_t id, float target, float speed = 12.f);
    float spring(uint32_t id, float target, float k = 220.f, float damp = 22.f);

    struct RGB { float r, g, b, a; };
    RGB   smooth_color(uint32_t id, const RGB& target, float speed = 12.f);

    void  clear();
}
