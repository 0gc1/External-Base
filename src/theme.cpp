// zeugenschutz base | join discord.gg/x3solutions
#include "theme.h"

namespace theme {

ImU32 u32(const ImVec4& c) { return ImGui::ColorConvertFloat4ToU32(c); }

ImVec4 lerp(const ImVec4& a, const ImVec4& b, float t) {
    return {
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t,
        a.w + (b.w - a.w) * t,
    };
}

ImVec4 alpha(const ImVec4& c, float a) { return { c.x, c.y, c.z, a }; }

void apply() {
    ImGuiStyle& s = ImGui::GetStyle();

    s.WindowRounding = 10.f;
    s.ChildRounding = 8.f;
    s.FrameRounding = 6.f;
    s.GrabRounding = 4.f;
    s.PopupRounding = 8.f;
    s.ScrollbarRounding = 6.f;
    s.TabRounding = 6.f;

    s.WindowPadding    = { 14, 12 };
    s.FramePadding     = { 8,  4 };
    s.ItemSpacing      = { 8,  6 };
    s.ItemInnerSpacing = { 6,  4 };
    s.IndentSpacing    = 18.0f;

    s.WindowBorderSize = 1.0f;
    s.ChildBorderSize  = 1.0f;
    s.FrameBorderSize  = 0.0f;
    s.PopupBorderSize  = 1.0f;

    s.WindowTitleAlign = { 0.0f, 0.5f };
    s.ScrollbarSize    = 10.0f;
    s.GrabMinSize      = 12.0f;
    s.WindowMinSize    = { 420, 300 };

    ImVec4* c = s.Colors;
    c[ImGuiCol_Text]                 = text;
    c[ImGuiCol_TextDisabled]         = text_dim;
    c[ImGuiCol_WindowBg]             = panel;
    c[ImGuiCol_ChildBg]              = element;
    c[ImGuiCol_PopupBg]              = panel;
    c[ImGuiCol_Border]               = border;
    c[ImGuiCol_BorderShadow]         = { 0, 0, 0, 0 };
    c[ImGuiCol_FrameBg]              = element;
    c[ImGuiCol_FrameBgHovered]       = hover;
    c[ImGuiCol_FrameBgActive]        = active;
    c[ImGuiCol_TitleBg]              = element;
    c[ImGuiCol_TitleBgActive]        = element;
    c[ImGuiCol_TitleBgCollapsed]     = element;
    c[ImGuiCol_MenuBarBg]            = element;
    c[ImGuiCol_ScrollbarBg]          = { 0, 0, 0, 0 };
    c[ImGuiCol_ScrollbarGrab]        = border;
    c[ImGuiCol_ScrollbarGrabHovered] = accent_d;
    c[ImGuiCol_ScrollbarGrabActive]  = accent;
    c[ImGuiCol_CheckMark]            = accent;
    c[ImGuiCol_SliderGrab]           = accent;
    c[ImGuiCol_SliderGrabActive]     = accent;
    c[ImGuiCol_Button]               = element;
    c[ImGuiCol_ButtonHovered]        = hover;
    c[ImGuiCol_ButtonActive]         = active;
    c[ImGuiCol_Header]               = element;
    c[ImGuiCol_HeaderHovered]        = hover;
    c[ImGuiCol_HeaderActive]         = active;
    c[ImGuiCol_Separator]            = border;
    c[ImGuiCol_SeparatorHovered]     = accent_d;
    c[ImGuiCol_SeparatorActive]      = accent;
    c[ImGuiCol_ResizeGrip]           = alpha(accent, 0.15f);
    c[ImGuiCol_ResizeGripHovered]    = alpha(accent, 0.35f);
    c[ImGuiCol_ResizeGripActive]     = accent;
    c[ImGuiCol_Tab]                  = element;
    c[ImGuiCol_TabHovered]           = hover;
    c[ImGuiCol_TabSelected]          = accent_d;
    c[ImGuiCol_TabDimmed]            = element;
    c[ImGuiCol_TabDimmedSelected]    = element;
    c[ImGuiCol_TableHeaderBg]        = element;
    c[ImGuiCol_TableBorderLight]     = border;
    c[ImGuiCol_TableBorderStrong]    = border;
    c[ImGuiCol_TableRowBg]           = { 0, 0, 0, 0 };
    c[ImGuiCol_TableRowBgAlt]        = { 1, 1, 1, 0.02f };
    c[ImGuiCol_PlotHistogram]        = accent;
    c[ImGuiCol_PlotHistogramHovered] = accent_d;
    c[ImGuiCol_DragDropTarget]       = accent;
    c[ImGuiCol_NavCursor]            = accent;
}

}
