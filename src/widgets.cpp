// zeugenschutz base | join discord.gg/x3solutions
#include "widgets.h"
#include "anim.h"
#include "theme.h"
#include "imgui_internal.h"

#include <cstring>
#include <cstdio>

namespace w {

static const float HEAD_H = 30.f;
static const float PAD_X = 14.f;
static const float PAD_Y = 8.f;
static const float RAD = 8.f;

static ImDrawListSplitter g_split;
static int g_depth = 0;

void card_begin(const char* title, float height) {
    ImGuiWindow* win = ImGui::GetCurrentWindow();
    ImVec2 start = win->DC.CursorPos;
    float w = ImGui::GetContentRegionAvail().x;

    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, RAD);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.f);

    char cid[80];
    snprintf(cid, sizeof(cid), "##card_%s", title);
    ImGui::BeginChild(cid, ImVec2(w, height), false,
                      ImGuiWindowFlags_NoScrollbar |
                      ImGuiWindowFlags_NoScrollWithMouse |
                      ImGuiWindowFlags_NoMove);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 cp = ImGui::GetWindowPos();
    ImVec2 cp_max(cp.x + w, cp.y + height);

    ImU32 body_bg = IM_COL32(16, 17, 22, 255);
    dl->AddRectFilled(cp, cp_max, body_bg, RAD);

    ImU32 head_bg = IM_COL32(22, 24, 30, 255);
    dl->AddRectFilled(cp, ImVec2(cp_max.x, cp.y + HEAD_H),
                      head_bg, RAD, ImDrawFlags_RoundCornersTop);

    ImU32 strip = ImGui::ColorConvertFloat4ToU32(theme::accent);
    dl->AddRectFilled(ImVec2(cp.x, cp.y + 8.f),
                      ImVec2(cp.x + 3.f, cp.y + HEAD_H - 8.f),
                      strip, 2.f);

    ImU32 line = ImGui::ColorConvertFloat4ToU32(theme::border);
    dl->AddLine(ImVec2(cp.x, cp.y + HEAD_H),
                ImVec2(cp_max.x, cp.y + HEAD_H),
                line, 1.f);

    ImU32 title_col = ImGui::ColorConvertFloat4ToU32(theme::text);
    ImVec2 ts = ImGui::CalcTextSize(title);
    dl->AddText(ImVec2(cp.x + PAD_X, cp.y + (HEAD_H - ts.y) * 0.5f),
                title_col, title);

    ImU32 border = ImGui::ColorConvertFloat4ToU32(theme::border);
    dl->AddRect(cp, cp_max, border, RAD, 0, 1.f);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.f, 6.f));

    ImGui::SetCursorPos(ImVec2(PAD_X, HEAD_H + PAD_Y));
    float body_w = w - PAD_X * 2.f;
    float body_h = height - HEAD_H - PAD_Y * 2.f;
    if (body_w < 1.f) body_w = 1.f;
    if (body_h < 1.f) body_h = 1.f;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::BeginChild("##body", ImVec2(body_w, body_h), false,
                      ImGuiWindowFlags_NoScrollbar |
                      ImGuiWindowFlags_NoScrollWithMouse |
                      ImGuiWindowFlags_NoBackground);
    ImGui::PopStyleVar();

    ImGui::Dummy(ImVec2(0, 0));
    ++g_depth;

    (void)start;
}

void card_end() {
    if (g_depth <= 0) return;
    --g_depth;

    ImGui::EndChild();
    ImGui::PopStyleVar();

    ImGui::EndChild();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
}

bool toggle(const char* label, bool* val) {
    ImGuiWindow* win = ImGui::GetCurrentWindow();
    if (win->SkipItems) return false;

    ImGuiID id = win->GetID(label);

    const float SQ = 18.f;
    const float RAD_S = 4.f;
    const float row_h = ImMax(ImGui::GetFrameHeight(), SQ);
    const float avail_w = ImGui::GetContentRegionAvail().x;

    ImVec2 row_start = win->DC.CursorPos;
    ImRect bb(row_start, ImVec2(row_start.x + avail_w, row_start.y + row_h));

    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;

    bool hov, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hov, &held);
    if (pressed) *val = !*val;

    float t = anim::smooth((uint32_t)id, *val ? 1.f : 0.f, 18.f);
    float ht = anim::smooth((uint32_t)id + 1u, hov ? 1.f : 0.f, 22.f);
    float pop = anim::spring((uint32_t)id + 2u, *val ? 1.f : 0.f, 300.f, 24.f);
    float check_s = ImClamp(pop, 0.f, 1.1f);

    ImDrawList* dl = win->DrawList;

    const char* dlabel = label;
    char short_label[128];
    if (const char* hash = strstr(label, "##")) {
        int n = (int)(hash - label);
        if (n > (int)sizeof(short_label) - 1) n = sizeof(short_label) - 1;
        memcpy(short_label, label, n);
        short_label[n] = 0;
        dlabel = short_label;
    }

    ImVec4 lbl_from = theme::text_dim;
    ImVec4 lbl_to = theme::text;
    ImVec4 lbl_lerp = {
        lbl_from.x + (lbl_to.x - lbl_from.x) * (t * 0.5f + ht * 0.4f),
        lbl_from.y + (lbl_to.y - lbl_from.y) * (t * 0.5f + ht * 0.4f),
        lbl_from.z + (lbl_to.z - lbl_from.z) * (t * 0.5f + ht * 0.4f),
        1.f
    };
    ImU32 lbl_col = ImGui::ColorConvertFloat4ToU32(lbl_lerp);

    float text_h = ImGui::GetTextLineHeight();
    dl->AddText(ImVec2(row_start.x, row_start.y + (row_h - text_h) * 0.5f),
                lbl_col, dlabel);

    ImVec2 box_min(row_start.x + avail_w - SQ, row_start.y + (row_h - SQ) * 0.5f);
    ImVec2 box_max(box_min.x + SQ, box_min.y + SQ);

    ImVec4 off_a = theme::element;
    ImVec4 off_b = theme::hover;
    ImVec4 off_lerp = {
        off_a.x + (off_b.x - off_a.x) * (ht * 0.5f),
        off_a.y + (off_b.y - off_a.y) * (ht * 0.5f),
        off_a.z + (off_b.z - off_a.z) * (ht * 0.5f),
        1.f
    };
    ImU32 body_off = ImGui::ColorConvertFloat4ToU32(off_lerp);
    ImU32 body_on = ImGui::ColorConvertFloat4ToU32(theme::accent);

    ImVec4 mix = {
        off_lerp.x + (theme::accent.x - off_lerp.x) * t,
        off_lerp.y + (theme::accent.y - off_lerp.y) * t,
        off_lerp.z + (theme::accent.z - off_lerp.z) * t,
        1.f
    };
    ImU32 body = ImGui::ColorConvertFloat4ToU32(mix);
    dl->AddRectFilled(box_min, box_max, body, RAD_S);

    float border_a = (1.f - t) * (0.55f + ht * 0.25f);
    if (border_a > 0.02f) {
        ImU32 bcol = ImGui::ColorConvertFloat4ToU32(theme::border);
        bcol = (bcol & 0x00FFFFFF) | ((unsigned int)(border_a * 255.f) << 24);
        dl->AddRect(box_min, box_max, bcol, RAD_S, 0, 1.f);
    }

    if (check_s > 0.03f) {
        ImVec2 c((box_min.x + box_max.x) * 0.5f, (box_min.y + box_max.y) * 0.5f);
        auto pt = [&](float nx, float ny) {
            return ImVec2(c.x + nx * SQ * check_s, c.y + ny * SQ * check_s);
        };
        ImVec2 pl = pt(-0.26f,  0.02f);
        ImVec2 pm = pt(-0.06f,  0.20f);
        ImVec2 pr = pt( 0.26f, -0.18f);

        ImU32 ink = IM_COL32(255, 255, 255, (int)(t * 255));
        dl->AddLine(pl, pm, ink, 2.f);
        dl->AddLine(pm, pr, ink, 2.f);
    }

    (void)body_off; (void)body_on; (void)held;
    return pressed;
}

bool slider(const char* label, float* val, float vmin, float vmax, const char* fmt) {
    ImGuiWindow* win = ImGui::GetCurrentWindow();
    if (win->SkipItems) return false;

    ImGuiID id = win->GetID(label);

    const float ROW_H = 24.f;
    const float GAP_LV = 6.f;
    const float GAP_VB = 10.f;
    const float BAR_H = 6.f;
    const float BAR_MIN = 60.f;

    const char* dlabel = label;
    char short_label[128];
    if (const char* hash = strstr(label, "##")) {
        int n = (int)(hash - label);
        if (n > (int)sizeof(short_label) - 1) n = sizeof(short_label) - 1;
        memcpy(short_label, label, n);
        short_label[n] = 0;
        dlabel = short_label;
    }

    char val_buf[64];
    snprintf(val_buf, sizeof(val_buf), fmt, *val);
    ImVec2 lbl_sz = ImGui::CalcTextSize(dlabel);
    ImVec2 val_sz = ImGui::CalcTextSize(val_buf);

    ImVec2 p = win->DC.CursorPos;
    float row_w = ImGui::GetContentRegionAvail().x;

    ImRect bb(p, ImVec2(p.x + row_w, p.y + ROW_H));
    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id)) return false;

    ImDrawList* dl = win->DrawList;
    float text_y = p.y + (ROW_H - lbl_sz.y) * 0.5f;

    ImU32 tx = ImGui::ColorConvertFloat4ToU32(theme::text);
    ImU32 ac = ImGui::ColorConvertFloat4ToU32(theme::accent);
    dl->AddText(ImVec2(p.x, text_y), tx, dlabel);

    float val_x = p.x + lbl_sz.x + GAP_LV;
    float bar_x1 = val_x + val_sz.x + GAP_VB;
    float bar_x2 = p.x + row_w;
    float bar_w = bar_x2 - bar_x1;
    if (bar_w < BAR_MIN) {
        bar_x1 = bar_x2 - BAR_MIN;
        bar_w = BAR_MIN;
    }
    float bar_y = p.y + (ROW_H - BAR_H) * 0.5f;

    dl->AddText(ImVec2(val_x, text_y), ac, val_buf);

    ImRect bar_bb(ImVec2(bar_x1, p.y + 2.f), ImVec2(bar_x2, p.y + ROW_H - 2.f));
    ImGuiID bar_id = id + 1u;
    ImGui::KeepAliveID(bar_id);

    bool over = ImGui::IsMouseHoveringRect(bar_bb.Min, bar_bb.Max)
             && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
    if (over && ImGui::IsMouseClicked(0)) {
        ImGui::SetActiveID(bar_id, win);
        ImGui::SetFocusID(bar_id, win);
        ImGui::FocusWindow(win);
    }
    bool held = ImGui::GetActiveID() == bar_id;
    if (held && !ImGui::IsMouseDown(0)) ImGui::ClearActiveID();

    bool changed = false;
    if (held && bar_w > 0.f) {
        float t = (ImGui::GetIO().MousePos.x - bar_x1) / bar_w;
        t = ImClamp(t, 0.f, 1.f);
        float nv = vmin + t * (vmax - vmin);
        if (*val != nv) { *val = nv; changed = true; }
    }

    float norm = (vmax > vmin) ? (*val - vmin) / (vmax - vmin) : 0.f;
    float anim_norm = held ? norm : anim::smooth((uint32_t)id + 100u, norm, 22.f);
    anim_norm = ImClamp(anim_norm, 0.f, 1.f);
    float hov_t = anim::smooth((uint32_t)id + 200u, (over || held) ? 1.f : 0.f, 24.f);
    float bar_r = BAR_H * 0.5f;

    ImVec4 track_a = theme::element;
    ImVec4 track_b = theme::hover;
    ImVec4 track_l = {
        track_a.x + (track_b.x - track_a.x) * (hov_t * 0.4f),
        track_a.y + (track_b.y - track_a.y) * (hov_t * 0.4f),
        track_a.z + (track_b.z - track_a.z) * (hov_t * 0.4f),
        1.f
    };
    ImU32 track_col = ImGui::ColorConvertFloat4ToU32(track_l);
    dl->AddRectFilled(ImVec2(bar_x1, bar_y), ImVec2(bar_x2, bar_y + BAR_H),
                      track_col, bar_r);

    if (anim_norm > 0.001f) {
        float fx2 = bar_x1 + bar_w * anim_norm;
        float min_fill = bar_r * 2.f;
        if (fx2 < bar_x1 + min_fill) fx2 = bar_x1 + min_fill;
        dl->AddRectFilled(ImVec2(bar_x1, bar_y), ImVec2(fx2, bar_y + BAR_H),
                          ac, bar_r);
    }

    return changed;
}

}
