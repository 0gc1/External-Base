// zeugenschutz base | join discord.gg/x3solutions
#pragma once
#include "imgui.h"

namespace theme {

    inline ImVec4 bg       = { 0.039f, 0.043f, 0.055f, 1.000f };
    inline ImVec4 panel    = { 0.070f, 0.075f, 0.098f, 0.960f };
    inline ImVec4 element  = { 0.102f, 0.106f, 0.133f, 1.000f };
    inline ImVec4 hover    = { 0.118f, 0.122f, 0.153f, 1.000f };
    inline ImVec4 active   = { 0.157f, 0.165f, 0.204f, 1.000f };
    inline ImVec4 border   = { 0.141f, 0.145f, 0.176f, 1.000f };
    inline ImVec4 text     = { 0.847f, 0.851f, 0.878f, 1.000f };
    inline ImVec4 text_dim = { 0.541f, 0.549f, 0.576f, 1.000f };
    inline ImVec4 accent   = { 0.949f, 0.306f, 0.420f, 1.000f };
    inline ImVec4 accent_d = { 0.690f, 0.220f, 0.302f, 1.000f };
    inline ImVec4 accent_s = { 0.949f, 0.306f, 0.420f, 0.130f };
    inline ImVec4 danger   = { 0.910f, 0.290f, 0.290f, 1.000f };
    inline ImVec4 ok       = { 0.290f, 0.850f, 0.470f, 1.000f };

    ImU32 u32(const ImVec4& c);
    ImVec4 lerp(const ImVec4& a, const ImVec4& b, float t);
    ImVec4 alpha(const ImVec4& c, float a);

    void apply();
}
