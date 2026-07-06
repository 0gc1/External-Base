// zeugenschutz base | join discord.gg/x3solutions
#pragma once
#include "rbx.h"
#include <vector>

namespace esp {
    inline bool enabled = true;
    inline bool self = false;
    inline bool box = true;
    inline bool name_tag = true;
    inline float max_dist = 1500.f;

    void draw(const std::vector<rbx::Player>& list);
}

