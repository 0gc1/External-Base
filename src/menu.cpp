// zeugenschutz base | join discord.gg/x3solutions
#include "menu.h"
#include "esp.h"
#include "mem.h"
#include "offsets.h"
#include "overlay.h"
#include "theme.h"
#include "anim.h"
#include "watermark.h"
#include "config.h"
#include "widgets.h"
#include "imgui.h"
#include "imgui_internal.h"

#include <cmath>
#include <cctype>
#include <cstring>
#include <string>


namespace menu {

static int active_tab = 0;

static const char* kTabs[] = {
    "ESP",
    "Players",
    "Misc",
    "Credits",
    "Info",
};
static constexpr int kTabCount = (int)(sizeof(kTabs) / sizeof(kTabs[0]));


static bool contains_ci(const std::string& hay, const char* needle)
{
    if (!needle || !*needle) return true;
    size_t nlen = std::strlen(needle);
    if (nlen > hay.size()) return false;
    for (size_t i = 0; i + nlen <= hay.size(); ++i) {
        size_t k = 0;
        while (k < nlen &&
               std::tolower((unsigned char)hay[i + k]) ==
               std::tolower((unsigned char)needle[k])) ++k;
        if (k == nlen) return true;
    }
    return false;
}

static void tab_esp()
{
    w::card_begin("Toggles", 190.f);
    w::toggle("Enabled",  &esp::enabled);
    w::toggle("Self ESP", &esp::self);
    w::toggle("Box",      &esp::box);
    w::toggle("Name",     &esp::name_tag);
    w::card_end();

    ImGui::Dummy({ 0, 10 });

    w::card_begin("Range", 88.f);
    w::slider("Distance", &esp::max_dist, 100.f, 5000.f, "%.0f");
    w::card_end();
}


static void tab_players(const std::vector<rbx::Player>& list)
{
    w::card_begin("Search", 88.f);
    static char query[64] = "";
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint("##plsearch", "search player...", query, sizeof(query));
    w::card_end();

    ImGui::Dummy({ 0, 8 });

    w::card_begin("List", 220.f);
    if (ImGui::BeginTable("plist", 4,
        ImGuiTableFlags_RowBg|ImGuiTableFlags_Borders|ImGuiTableFlags_ScrollY,
        { 0, 0 }))
    {
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("HP");
        ImGui::TableSetupColumn("Team");
        ImGui::TableSetupColumn("Dist");
        ImGui::TableHeadersRow();

        Vec3 me{};
        if (rbx::camera && off::CameraPos)
            me = mem::read<Vec3>(rbx::camera + off::CameraPos);

        for (const auto& p : list) {
            if (p.addr == rbx::local_player) continue;
            if (!contains_ci(p.name, query)) continue;

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(p.name.empty() ? "?" : p.name.c_str());
            ImGui::TableNextColumn();
            if (p.max_health > 0.f) ImGui::Text("%.0f / %.0f", p.health, p.max_health);
            else                    ImGui::TextUnformatted("-");

            ImGui::TableNextColumn();
            ImGui::TextColored(p.friendly ? ImVec4(0.4f, 0.9f, 0.5f, 1.f)
                                          : ImVec4(0.95f, 0.4f, 0.4f, 1.f),
                               p.friendly ? "friendly" : "enemy");

            ImGui::TableNextColumn();
            if (p.hrp) {
                Vec3 d = p.pos - me;
                ImGui::Text("%.0f", std::sqrt(d.dot(d)));
            } else {
                ImGui::TextUnformatted("-");
            }
        }
        ImGui::EndTable();
    }
    w::card_end();
}

static void tab_misc() {
    w::card_begin("Overlay", 84.f);
    w::toggle("Watermark", &wm::enabled);
    w::card_end();

    ImGui::Dummy({ 0, 10 });

    w::card_begin("Config", 90.f);
    if (ImGui::Button("Save")) cfg::save();
    ImGui::SameLine();
    if (ImGui::Button("Load")) cfg::load();
    w::card_end();
}


static void tab_credits()
{
    w::card_begin("Base", 110.f);
    ImGui::BulletText("zeugenschutz");
    ImGui::BulletText("author of the X3 external");
    w::card_end();

    ImGui::Dummy({ 0, 8 });

    w::card_begin("Offsets", 78.f);
    ImGui::BulletText("rbxoffsets.xyz");
    w::card_end();

    ImGui::Dummy({ 0, 8 });

    w::card_begin("Rendering", 110.f);
    ImGui::BulletText("Dear ImGui  (ocornut)");
    ImGui::BulletText("Direct3D 9  /  Win32");
    w::card_end();
    ImGui::Dummy({ 0, 8 });

    w::card_begin("Contact", 78.f);
    ImGui::TextDisabled("discord.gg/x3solutions");
    w::card_end();
}

static void tab_info() {
    w::card_begin("Runtime", 100.f);
    ImGui::Text("Version %s", off::version.empty() ? "?" : off::version.c_str());
    ImGui::Text("Loaded  %zu offsets", off::map.size());
    w::card_end();

    ImGui::Dummy({ 0, 8 });

    w::card_begin("Pointers", 140.f);
    ImGui::Text("DM    0x%llX", (unsigned long long)rbx::datamodel);
    ImGui::Text("LP    0x%llX", (unsigned long long)rbx::local_player);
    ImGui::Text("Cam   0x%llX", (unsigned long long)rbx::camera);
    ImGui::Text("VE    0x%llX", (unsigned long long)rbx::visual_eng);
    w::card_end();
}


static void draw_content(const std::vector<rbx::Player>& list) {
    switch (active_tab) {
        case 0: tab_esp(); break;
        case 1: tab_players(list); break;
        case 2: tab_misc(); break;
        case 3: tab_credits(); break;
        case 4: tab_info(); break;
    }
}

static void draw_sidebar()
{
    const float row_h = 36.f;
    const float row_gap = 2.f;
    const float bar_w = 3.f;
    const float rad = 6.f;
    const float text_pad_x = 18.f;

    if (overlay::font_head) ImGui::PushFont(overlay::font_head);
    ImGui::TextUnformatted("zeugenschutz");
    if (overlay::font_head) ImGui::PopFont();

    ImGui::TextDisabled("external base");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImDrawList* dl = ImGui::GetWindowDrawList();
    float col_w = ImGui::GetContentRegionAvail().x;

    ImU32 hi     = ImGui::ColorConvertFloat4ToU32(theme::accent_s);
    ImU32 bar    = ImGui::ColorConvertFloat4ToU32(theme::accent);
    ImU32 tx_dim = ImGui::ColorConvertFloat4ToU32(theme::text_dim);
    ImU32 tx_on  = ImGui::ColorConvertFloat4ToU32(theme::text);

    for (int i = 0; i < kTabCount; ++i) {
        ImGui::PushID(i);
        ImVec2 p = ImGui::GetCursorScreenPos();

        ImGui::InvisibleButton("row", ImVec2(col_w, row_h));
        bool hov = ImGui::IsItemHovered();
        if (ImGui::IsItemClicked(0)) active_tab = i;

        bool is_active = (active_tab == i);

        if (is_active) {
            dl->AddRectFilled(p, ImVec2(p.x + col_w, p.y + row_h), hi, rad);
            dl->AddRectFilled(ImVec2(p.x, p.y + 7.f),
                              ImVec2(p.x + bar_w, p.y + row_h - 7.f),
                              bar, 1.5f);
        }
        else if (hov) {
            dl->AddRectFilled(p, ImVec2(p.x + col_w, p.y + row_h),
                              IM_COL32(255, 255, 255, 12), rad);
        }

        ImVec2 ts = ImGui::CalcTextSize(kTabs[i]);
        dl->AddText(ImVec2(p.x + text_pad_x, p.y + (row_h - ts.y) * 0.5f),
                    is_active ? tx_on : tx_dim,
                    kTabs[i]);

        ImGui::PopID();
        ImGui::Dummy(ImVec2(0, row_gap));
    }
}


void draw(const std::vector<rbx::Player>& list)
{
    if (!overlay::menu_open) return;

    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(
        ImVec2(io.DisplaySize.x*0.5f - 340.f, io.DisplaySize.y*0.5f - 230.f),
        ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({ 680, 460 }, ImGuiCond_FirstUseEver);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    bool keep = overlay::menu_open;
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize
                           | ImGuiWindowFlags_NoCollapse
                           | ImGuiWindowFlags_NoTitleBar;

    if (!ImGui::Begin("##zsroot", &keep, flags)) {
        ImGui::End();
        ImGui::PopStyleVar();
        overlay::menu_open = keep;
        return;
    }

    ImGui::BeginChild("##side", ImVec2(180, 0),
                      ImGuiChildFlags_None,
                      ImGuiWindowFlags_NoScrollbar);
    ImGui::Dummy({ 0, 10 });
    ImGui::Indent(14.f);
    draw_sidebar();
    ImGui::Unindent(14.f);
    ImGui::EndChild();

    ImGui::SameLine(0, 0);

    static int last_shown = active_tab;
    static float fade = 1.f;
    if (last_shown != active_tab) {
        last_shown = active_tab;
        fade = 0.f;
    }
    float dt = io.DeltaTime;
    fade += (1.f - fade) * (dt * 16.f);
    if (fade > 1.f) fade = 1.f;

    ImGui::BeginChild("##body", ImVec2(0, 0),
                      ImGuiChildFlags_None,
                      ImGuiWindowFlags_None);

    ImGui::Dummy({ 0, 10 });
    float body_w = ImGui::GetContentRegionAvail().x;
    const float side_pad = 18.f;

    ImGui::SetCursorPosX(side_pad);
    ImGui::BeginChild("##inner",
                      ImVec2(body_w - side_pad*2.f, 0),
                      ImGuiChildFlags_None,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, fade);
    draw_content(list);
    ImGui::PopStyleVar();

    ImGui::EndChild();
    ImGui::EndChild();

    ImGui::End();
    ImGui::PopStyleVar();

    if (!keep) overlay::menu_open = false;
}

}
